/***************************************************************************
 * rubyinterpreter.cpp
 * This file is part of the KDE project
 * copyright (C)2005,2007 by Cyrille Berger (cberger@cberger.net)
 * copyright (C)2006,2007 by Sebastian Sauer (mail@dipe.org)
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 * You should have received a copy of the GNU Library General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 ***************************************************************************/

#include "rubyextension.h"
#include "rubycallcache.h"
#include "rubyvariant.h"
#include "rubyinterpreter.h"
#include "rubyfunction.h"

#include <kross/core/metatype.h>

#include <QMap>
#include <QString>
#include <QPointer>
#include <QMetaObject>
#include <QMetaMethod>
#include <QHash>
#include <QVarLengthArray>

using namespace Kross;

namespace Kross {

    /// @internal d-pointer class.
    class RubyExtensionPrivate {
        friend class RubyExtension;

        /// The wrapped QObject.
        //QObject* m_object;
        QPointer<QObject> m_object;

        #ifdef KROSS_RUBY_EXTENSION_CTORDTOR_DEBUG
            /// \internal string for debugging.
            QString debuginfo;
        #endif

        /// The wrapped krossobject VALUE type.
        static VALUE s_krossObject;
        //static VALUE s_krossException;

        /// The cached list of methods.
        QHash<QByteArray, int> m_methods;
        /// The cached list of properties.
        QHash<QByteArray, int> m_properties;
        /// The cached list of enumerations.
        QHash<QByteArray, int> m_enumerations;

        /// The \a RubyFunction instances.
        QHash<QByteArray, RubyFunction*> m_functions;

        /// The list of \a RubyCallCache instances.
        QList< RubyCallCache* > m_cachelist;
    };

    VALUE RubyExtensionPrivate::s_krossObject = 0;
    //VALUE RubyExtensionPrivate::s_krossException = 0;

}

RubyExtension::RubyExtension(QObject* object)
    : d(new RubyExtensionPrivate())
{
    d->m_object = object;

    #ifdef KROSS_RUBY_EXTENSION_CTORDTOR_DEBUG
        d->debuginfo = object ? QString("name=%1 class=%2").arg(object->objectName()).arg(object->metaObject()->className()) : "NULL";
        krossdebug(QString("RubyExtension Ctor %1").arg(d->debuginfo));
    #endif

    if(d->m_object) {
        const QMetaObject* metaobject = d->m_object->metaObject();

        { // initialize methods.
            const int count = metaobject->methodCount();
            for(int i = 0; i < count; ++i) {
                QMetaMethod member = metaobject->method(i);
                const QString signature = member.signature();
                const QByteArray name = signature.left(signature.indexOf('(')).toLatin1();
                if(! d->m_methods.contains(name))
                    d->m_methods.insert(name, i);
            }
        }

        { // initialize properties
            const int count = metaobject->propertyCount();
            for(int i = 0; i < count; ++i) {
                QMetaProperty prop = metaobject->property(i);
                d->m_properties.insert(prop.name(), i);
                if(prop.isWritable())
                    d->m_properties.insert(QByteArray(prop.name()).append('='), i);
            }
        }

        { // initialize enumerations
            const int count = metaobject->enumeratorCount();
            for(int i = 0; i < count; ++i) {
                QMetaEnum e = metaobject->enumerator(i);
                const int kc = e.keyCount();
                for(int k = 0; k < kc; ++k) {
                    const QByteArray name = /*e.name() +*/ e.key(k);
                    d->m_enumerations.insert(name, e.value(k));
                }
            }
        }
    }
}

RubyExtension::~RubyExtension()
{
    #ifdef KROSS_RUBY_EXTENSION_CTORDTOR_DEBUG
        krossdebug(QString("RubyExtension Dtor %1 functioncount=%2 cachecount=%3").arg(d->debuginfo).arg(d->m_functions.count()).arg(d->m_cachelist.count()));
    #endif
    qDeleteAll(d->m_functions);
    //qDeleteAll(d->m_cachelist);
    delete d;
}

QObject* RubyExtension::object() const
{
    return d->m_object;
}

RubyFunction* RubyExtension::createFunction(QObject* sender, const QByteArray& signal, const VALUE& method)
{
    RubyFunction* function = new RubyFunction(sender, signal, method);
    d->m_functions.insertMulti(signal, function);
    return function;
}

VALUE RubyExtension::method_missing(int argc, VALUE *argv, VALUE self)
{
    if( argc < 1 )
        return 0;

    #ifdef KROSS_RUBY_EXTENSION_DEBUG
        krossdebug("RubyExtension::method_missing Converting self to RubyExtension");
    #endif

    RubyExtension* extension = toExtension(self);
    Q_ASSERT(extension);
    return RubyExtension::call_method_missing(extension, argc, argv, self);
}

VALUE RubyExtension::clone(VALUE self)
{
    #ifdef KROSS_RUBY_EXTENSION_DEBUG
        krossdebug("Cloning...");
    #endif
    RubyExtension* extension = toExtension(self);
    Q_ASSERT(extension);
    if( extension->d->m_methods.contains("clone") ) {
        return extension->callMetaMethod("clone", 1, &self, self);
    }
    return Qnil; // TODO: is it useful to call the ruby clone function if no clone function is available ?
}

VALUE RubyExtension::toVoidPtr(VALUE self)
{
      RubyExtension* extension = toExtension(self);
      Q_ASSERT(extension);
      return Data_Wrap_Struct( rb_cObject, 0, 0, extension->object());
}

VALUE RubyExtension::fromVoidPtr(VALUE /*self*/, VALUE obj)
{
    QObject* qobj;
    Data_Get_Struct(obj, QObject, qobj);
    return qobj ? RubyExtension::toVALUE( new RubyExtension(qobj), true /*owner*/ ) : Qnil;
}

VALUE RubyExtension::callFindChild(int argc, VALUE *argv, VALUE self)
{
    VALUE name = ( argc == 1 && TYPE(argv[0]) == T_STRING ) ? argv[0] : ( argc >= 2 && TYPE(argv[1]) == T_STRING ) ? argv[1] : Qnil;
    if( TYPE(name) == T_NIL ) {
        rb_raise(rb_eTypeError, "Expected the objectName as argument.");
        return Qnil;
    }
    RubyExtension* extension = toExtension(self);
    Q_ASSERT(extension);
    QObject* object = extension->d->m_object->findChild<QObject*>(RubyType<QString>::toVariant(name));
    return object ? RubyExtension::toVALUE( new RubyExtension(object), true /*owner*/ ) : Qnil;
}

VALUE RubyExtension::propertyNames(VALUE self)
{
    RubyExtension* extension = toExtension(self);
    Q_ASSERT(extension);
    const QMetaObject* metaobject = extension->d->m_object->metaObject();
    VALUE list = rb_ary_new();
    for(int i = 0; i < metaobject->propertyCount(); ++i)
        rb_ary_push(list, RubyType<QString>::toVALUE( metaobject->property(i).name() ));
    return list;
}

VALUE RubyExtension::property(int argc, VALUE *argv, VALUE self)
{
    VALUE name = argc == 1 ? argv[0] : Qnil;
    if( TYPE(name) != T_STRING ) {
        rb_raise(rb_eTypeError, "Expected the properties name as argument.");
        return Qnil;
    }
    RubyExtension* extension = toExtension(self);
    Q_ASSERT(extension);
    return RubyType<QVariant>::toVALUE( extension->d->m_object->property(STR2CSTR(name)) );
}

VALUE RubyExtension::setProperty(int argc, VALUE *argv, VALUE self)
{
    VALUE name = argc >= 2 ? argv[0] : Qnil;
    VALUE value = argc >= 2 ? argv[1] : Qnil;
    if( TYPE(name) != T_STRING || argc < 2 ) {
        rb_raise(rb_eTypeError, "Expected the properties name and value as arguments.");
        return Qnil;
    }
    RubyExtension* extension = toExtension(self);
    Q_ASSERT(extension && extension->d->m_object);
    return RubyType<bool>::toVALUE( extension->d->m_object->setProperty(STR2CSTR(name), RubyType<QVariant>::toVariant(value)) );
}

VALUE RubyExtension::callConnect(int argc, VALUE *argv, VALUE self)
{
    #ifdef KROSS_RUBY_EXTENSION_CALLCONNECT_DEBUG
        krossdebug(QString("RubyExtension::callConnect"));
    #endif
    /*
    http://www.ruby-doc.org/doxygen/1.8.4/eval_8c-source.html#l08914
    http://renaud.waldura.com/doc/ruby/idioms.shtml
    http://www.ruby-doc.org/doxygen/1.8.4/struct_m_e_t_h_o_d.html
    http://www.rubycentral.com/book/ext_ruby.html
    */

    if( argc < 2 ) {
        rb_raise(rb_eTypeError, "Expected at least 2 arguments.");
        return Qfalse;
    }

    RubyExtension* selfextension = toExtension(self);
    Q_ASSERT(selfextension);

    int idx; // next argument to check
    QObject* sender; // the sender object
    QByteArray sendersignal; // the sender signal
    switch( TYPE(argv[0]) ) {
        case T_STRING: { // connect(signal, ...)
            sender = selfextension->object();
            sendersignal = RubyType<QByteArray>::toVariant(argv[0]);
            idx = 1;
        } break;
        case T_DATA: { // connect(sender, signal, ...)
            if( ! RubyExtension::isRubyExtension(argv[0]) ) {
                rb_raise(rb_eTypeError, "First argument needs to be a signalname or a sender-object.");
                return Qfalse;
            }
            if( ! TYPE(argv[1]) != T_STRING ) {
                rb_raise(rb_eTypeError, "Second argument needs to be a signalname.");
                return Qfalse;
            }
            RubyExtension* senderextension = toExtension(argv[0]);
            Q_ASSERT(senderextension);
            sender = senderextension->object();
            sendersignal = RubyType<QByteArray>::toVariant(argv[1]);
            idx = 2;
            if( argc <= idx ) {
                rb_raise(rb_eTypeError, ::QString("Expected at least %1 arguments.").arg(idx+1).toLatin1().constData());
                return Qfalse;
            }
        } break;
        default: {
            rb_raise(rb_eTypeError, "First argument needs to be a signalname or a sender-object.");
            return Qfalse;
        } break;
    }

    QObject* receiver = 0; // the receiver object
    QByteArray receiverslot; // the receiver slot
    if( TYPE(argv[idx]) == T_DATA ) {
#if(RUBY_VERSION_MAJOR==1 && RUBY_VERSION_MINOR==8 && RUBY_VERSION_TEENY==4)
        //Ruby sucks sometimes; http://blade.nagaokaut.ac.jp/cgi-bin/scat.rb/ruby/ruby-talk/201285
        { // just always assume the user provides a method here for now...
#else
        if( rb_obj_is_kind_of(argv[idx], rb_cMethod) ) { // connect with ruby method
#endif
            RubyFunction* function = selfextension->createFunction(sender, sendersignal, argv[idx]);
            receiver = function;
            receiverslot = sendersignal;
        }
        /*TODO
        else if( RubyExtension::isRubyExtension(args[idx]) ) { // connect(..., receiver, signal)
            RubyExtension* receiverextension = toExtension(args[idx]);
            Q_ASSERT(receiverextension);
            receiver = receiverextension->object();
        }
        */
#if(!(RUBY_VERSION_MAJOR==1 && RUBY_VERSION_MINOR==8 && RUBY_VERSION_TEENY==4))
        else {
            rb_raise(rb_eTypeError, ::QString("The argument number %1 is invalid.").arg(idx).toLatin1().constData());
            return Qfalse;
        }
#endif
    }

    // Dirty hack to replace SIGNAL() and SLOT() macros. If the user doesn't
    // defined them explicit, we assume it's wanted to connect from a signal to
    // a slot. This seems to be the most flexible solution so far...
    if( ! sendersignal.startsWith('1') && ! sendersignal.startsWith('2') )
        sendersignal.prepend('2'); // prepending 2 means SIGNAL(...)
    if( ! receiverslot.startsWith('1') && ! receiverslot.startsWith('2') )
        receiverslot.prepend('1'); // prepending 1 means SLOT(...)

    #ifdef KROSS_RUBY_EXTENSION_CALLCONNECT_DEBUG
        krossdebug( QString("RubyExtension::doConnect sender=%1 signal=%2 receiver=%3 slot=%4").arg(sender->objectName()).arg(sendersignal.constData()).arg(receiver->objectName()).arg(receiverslot.constData()).toLatin1().constData() );
    #endif
    if(! QObject::connect(sender, sendersignal, receiver, receiverslot) ) {
        krosswarning( QString("RubyExtension::doConnect Failed to connect").toLatin1().constData() );
        return Qfalse;
    }
    return Qtrue;
}

VALUE RubyExtension::callDisconnect(int argc, VALUE *argv, VALUE self)
{
    //TODO
    Q_UNUSED(argc);
    Q_UNUSED(argv);
    Q_UNUSED(self);
    return Qfalse;
}

VALUE RubyExtension::callMetaMethod(const QByteArray& funcname, int argc, VALUE *argv, VALUE self)
{
    const int argumentcount = argc - 1;

    #ifdef KROSS_RUBY_EXTENSION_DEBUG
        krossdebug(QString("RubyExtension::callMetaMethod method=%1 argumentcount=%2").arg(funcname.constData()).arg(argumentcount));
        for(int i = 1; i < argc; i++) {
            QVariant v = RubyType<QVariant>::toVariant(argv[i]);
            krossdebug(QString("  argument #%1: variant.toString=%2 variant.typeName=%3").arg(i).arg(v.toString()).arg(v.typeName()));
        }
    #endif

    int methodindex = d->m_methods[funcname];
    if(methodindex < 0) {
        krosswarning(QString("No such function '%1'").arg(funcname.constData()));
        rb_raise(rb_eTypeError, "No such function");
        return Qnil;
    }

    QObject* object = d->m_object;
    QMetaMethod metamethod = object->metaObject()->method( methodindex );
    if(metamethod.parameterTypes().size() != argumentcount) {
        bool found = false;
        const int count = object->metaObject()->methodCount();
        for(++methodindex; methodindex < count; ++methodindex) {
            metamethod = object->metaObject()->method( methodindex );
            const QString signature = metamethod.signature();
            const QByteArray name = signature.left(signature.indexOf('(')).toLatin1();
            if(name == funcname && metamethod.parameterTypes().size() == argumentcount) {
                found = true;
                break;
            }
        }
        if(! found) {
            krosswarning(QString("The function '%1' does not expect %2 arguments.").arg(funcname.constData()).arg(argumentcount));
            return Qfalse;
        }
    }

    #ifdef KROSS_RUBY_EXTENSION_DEBUG
        krossdebug( QString("QMetaMethod idx=%1 sig=%2 tag=%3 type=%4").arg(methodindex).arg(metamethod.signature()).arg(metamethod.tag()).arg(metamethod.typeName()) );
    #endif

    QList<QByteArray> typelist = metamethod.parameterTypes();
    const int typelistcount = typelist.count();
    bool hasreturnvalue = strcmp(metamethod.typeName(),"") != 0;

    // exact 1 returnvalue + 0..9 arguments
    Q_ASSERT(typelistcount <= 10);
    //QVarLengthArray<MetaType*> variantargs( typelistcount + 1 );
    //QVarLengthArray<void*> voidstarargs( typelistcount + 1 );
    QVarLengthArray<int> types( typelistcount + 1 );
    QVarLengthArray<int> metatypes( typelistcount + 1 );

    // set the return type
    if(hasreturnvalue) {
        types[0] = QVariant::nameToType( metamethod.typeName() );
        if( types[0] == QVariant::Invalid || types[0] == QVariant::UserType ) {
            metatypes[0] = QMetaType::type( metamethod.typeName() );
            #ifdef KROSS_RUBY_EXTENSION_DEBUG
                krossdebug( QString("RubyExtension::callMetaMethod return typeName=%1 typeId=%2").arg(metamethod.typeName()).arg(metatypes[0]) );
            #endif
        }
        else {
            metatypes[0] = QMetaType::Void; //FIXME: disable before release
            #ifdef KROSS_RUBY_EXTENSION_DEBUG
                krossdebug( QString("RubyExtension::callMetaMethod return typeName=%1 typeId=%2 (with metatype=QMetaType::Void)").arg(metamethod.typeName()).arg(metatypes[0]) );
            #endif
        }
    }
    else {
        types[0] = QVariant::Invalid; //FIXME: disable before release
        metatypes[0] = QMetaType::Void;
    }

    // set the arguments types
    for(int idx = 1; idx <= typelistcount; ++idx) {
        const char* typeName = typelist[idx - 1].constData();
        types[idx] = QVariant::nameToType(typeName);
        if( types[idx] == QVariant::Invalid || types[idx] == QVariant::UserType ) {
            metatypes[idx] = QMetaType::type(typeName);
            #ifdef KROSS_RUBY_EXTENSION_DEBUG
                krossdebug( QString("  RubyExtension::callMetaMethod argument idx=%1 typeName=%2 typeId=%3").arg(idx).arg(typeName).arg(metatypes[idx]) );
            #endif
        }
        else {
            metatypes[idx] = QMetaType::Void; //FIXME: disable before release
            #ifdef KROSS_RUBY_EXTENSION_DEBUG
                krossdebug( QString("  RubyExtension::callMetaMethod argument idx=%1 typeName=%2 typeId=%3 set metatype=QMetaType::Void").arg(idx).arg(typeName).arg(metatypes[idx]) );
            #endif
        }
    }

    // Create a cache of the function call
    RubyCallCache* callobj = new RubyCallCache(object, methodindex, hasreturnvalue, types, metatypes);
    QByteArray varcallcache = QByteArray("@callcache") + funcname;
    rb_iv_set(self, varcallcache, callobj->toValue());
    rb_define_variable("$krossinternallastclass", &self);
    rb_eval_string("def $krossinternallastclass." + funcname + "(*args)\n "+ varcallcache +".cacheexec(nil,*args)\nend");
    d->m_cachelist.append( callobj );
    //krossdebug( QString("|||>>> %1").arg(d->debuginfo) );
    return callobj->execfunction(argc, argv);
}

VALUE RubyExtension::call_method_missing(RubyExtension* extension, int argc, VALUE *argv, VALUE self)
{
    QByteArray name = rb_id2name(SYM2ID(argv[0]));

    // look if the name is a method
    if( extension->d->m_methods.contains(name) ) {
        return extension->callMetaMethod(name, argc, argv, self);
    }

    // look if the name is a property
    if( extension->d->m_properties.contains(name) /* && extension->d->m_object */ ) {
        const QMetaObject* metaobject = extension->d->m_object->metaObject();
        QMetaProperty property = metaobject->property( extension->d->m_properties[name] );
        if( name.endsWith('=') ) { // setter
            if(argc < 2) {
                rb_raise(rb_eNameError, QString("Expected value-argument for \"%1\" setter.").arg(name.constData()).toLatin1().constData());
                return Qnil;
            }
            QVariant v = RubyType<QVariant>::toVariant(argv[1]);
            if(! property.write(extension->d->m_object, v)) {
                rb_raise(rb_eNameError, QString("Setting attribute \"%1\" failed.").arg(name.constData()).toLatin1().constData());
                return Qnil;
            }
            return Qnil;
        }
        else { // getter
            if(! property.isReadable()) {
                rb_raise(rb_eNameError, QString("Attribute \"%1\" is not readable.").arg(name.constData()).toLatin1().constData());
                return Qnil;
            }
            return RubyType<QVariant>::toVALUE( property.read(extension->d->m_object) );
        }
    }

    // look if the name is an enumeration
    if( extension->d->m_enumerations.contains(name) ) {
        return RubyType<int>::toVALUE( extension->d->m_enumerations[name] );
    }

    // look if it's a dynamic property
    if( extension->d->m_object->dynamicPropertyNames().contains(name) ) {
        return RubyType<QVariant>::toVALUE( extension->d->m_object->property(name) );
    }

    // look if the name refers to a child object
    QObject* object = extension->d->m_object->findChild<QObject*>(name);
    if( object ) {
        return RubyExtension::toVALUE( new RubyExtension(object), true /*owner*/ );
    }

    rb_raise(rb_eNameError, QString("No such method or variable \"%1\".").arg(name.constData()).toLatin1().constData());
    return Qnil;
}

void RubyExtension::delete_object(void* object)
{
    #ifdef KROSS_RUBY_EXTENSION_CTORDTOR_DEBUG
        krossdebug("RubyExtension::delete_object");
    #endif
    RubyExtension* extension = static_cast< RubyExtension* >(object);
    delete extension;
    extension = 0;
}

#if 0
void RubyExtension::delete_exception(void* object)
{
    Kross::Exception* exc = static_cast<Kross::Exception*>(object);
    exc->_KShared_unref();
}
#endif

bool RubyExtension::isRubyExtension(VALUE value)
{
    VALUE result = rb_funcall(value, rb_intern("kind_of?"), 1, RubyExtensionPrivate::s_krossObject );
    if( TYPE(result) == T_TRUE )
        return true;
    result = rb_funcall(value, rb_intern("const_defined?"), 1, ID2SYM(rb_intern("MODULEOBJ") ));
    if(TYPE(result) == T_TRUE) {
        value = rb_funcall(value, rb_intern("const_get"), 1, ID2SYM(rb_intern("MODULEOBJ")));
        result = rb_funcall(value, rb_intern("kind_of?"), 1, RubyExtensionPrivate::s_krossObject );
        if(TYPE(result) == T_TRUE)
            return true;
    }
    return false;
}

#if 0
bool RubyExtension::isOfExceptionType(VALUE value)
{
    VALUE result = rb_funcall(value, rb_intern("kind_of?"), 1, RubyExtensionPrivate::s_krossException );
    return (TYPE(result) == T_TRUE);
}
Kross::Exception* RubyExtension::convertToException(VALUE value)
{
    if( isOfExceptionType(value) ) {
        Kross::Exception* exception;
        Data_Get_Struct(value, Kross::Exception, exception);
        return exception;
    }
    return 0;
}
VALUE RubyExtension::convertFromException(Kross::Exception::Ptr exc)
{
    if(RubyExtensionPrivate::s_krossException == 0)
        RubyExtensionPrivate::s_krossException = rb_define_class_under(RubyInterpreter::krossModule(), "KrossException", rb_eRuntimeError);
    //exc->_KShared_ref(); //TODO
    return Data_Wrap_Struct(RubyExtensionPrivate::s_krossException, 0, RubyExtension::delete_exception, exc.data() );
}
#endif

RubyExtension* RubyExtension::toExtension(VALUE value)
{
    VALUE result = rb_funcall(value, rb_intern("kind_of?"), 1, RubyExtensionPrivate::s_krossObject );
    if( TYPE(result) != T_TRUE ) {
        if( TYPE(value) != T_MODULE )
            return 0;
        result = rb_funcall(value, rb_intern("const_defined?"), 1, ID2SYM(rb_intern("MODULEOBJ") ));
        if(TYPE(result) != T_TRUE)
            return 0;
        value = rb_funcall( value, rb_intern("const_get"), 1, ID2SYM(rb_intern("MODULEOBJ")) );
        result = rb_funcall(value, rb_intern("kind_of?"), 1, RubyExtensionPrivate::s_krossObject );
        if(TYPE(result) != T_TRUE)
            return 0;
    }
    RubyExtension* extension;
    Data_Get_Struct(value, RubyExtension, extension);
    return extension;
}

VALUE RubyExtension::toVALUE(RubyExtension* extension, bool owner)
{
    QObject* object = extension->d->m_object;
    #ifdef KROSS_RUBY_EXTENSION_DEBUG
        krossdebug( QString("RubyExtension::toVALUE RubyExtension.QObject=%1 owner=%2").arg( object ? QString("%1 %2").arg(object->objectName()).arg(object->metaObject()->className()).arg(owner) : "NULL" ).arg(owner) );
    #endif
    if( ! object )
        return 0;
    Q_ASSERT( RubyExtensionPrivate::s_krossObject );
    return Data_Wrap_Struct(RubyExtensionPrivate::s_krossObject, 0, owner ? RubyExtension::delete_object : 0, extension);
}

void RubyExtension::init()
{
    RubyExtensionPrivate::s_krossObject = rb_define_class_under(RubyInterpreter::krossModule(), "Object", rb_cObject);
    rb_define_method(RubyExtensionPrivate::s_krossObject, "method_missing",  (VALUE (*)(...))RubyExtension::method_missing, -1);
    rb_define_method(RubyExtensionPrivate::s_krossObject, "clone", (VALUE (*)(...))RubyExtension::clone, 0);
    rb_define_method(RubyExtensionPrivate::s_krossObject, "findChild", (VALUE (*)(...))RubyExtension::callFindChild, -1);
    rb_define_method(RubyExtensionPrivate::s_krossObject, "propertyNames", (VALUE (*)(...))RubyExtension::propertyNames, 0);
    rb_define_method(RubyExtensionPrivate::s_krossObject, "property", (VALUE (*)(...))RubyExtension::property, -1);
    rb_define_method(RubyExtensionPrivate::s_krossObject, "setProperty", (VALUE (*)(...))RubyExtension::setProperty, -1);
    rb_define_method(RubyExtensionPrivate::s_krossObject, "connect", (VALUE (*)(...))RubyExtension::callConnect, -1);
    rb_define_method(RubyExtensionPrivate::s_krossObject, "disconnect", (VALUE (*)(...))RubyExtension::callDisconnect, -1);
    rb_define_method(RubyExtensionPrivate::s_krossObject, "toVoidPtr", (VALUE (*)(...))RubyExtension::toVoidPtr, 0);
    rb_define_module_function(RubyExtensionPrivate::s_krossObject, "fromVoidPtr", (VALUE (*)(...))RubyExtension::fromVoidPtr, 1);
}
