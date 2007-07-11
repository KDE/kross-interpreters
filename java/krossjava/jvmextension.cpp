/***************************************************************************
 * jvmextension.cpp
 * This file is part of the KDE project
 *
 * copyright (C)2007 by Vincent Verhoeven <verhoevenv@gmail.com>
 * copyright (C)2007 by Sebastian Sauer <mail@dipe.org>
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

#include "jvmextension.h"
#include "jvminterpreter.h"
#include "jvmclasswriter.h"
#include "jvmvariant.h"
//#include <kross/core/metatype.h>

//#include <QMap>
#include <QString>
#include <QPointer>
#include <QFile>
#include <QMetaObject>
#include <QMetaMethod>
#include <QHash>
#include <QVarLengthArray>

using namespace Kross;

namespace Kross {

    /// @internal d-pointer class.
    class JVMExtension::Private {
        public:
            /// The wrapped QObject.
            QPointer<QObject> m_object;

            /// For debugging.
            QString debuginfo;

            /// The cached list of methods.
            QHash<QByteArray, int> m_methods;
            /// The cached list of properties.
            QHash<QByteArray, int> m_properties;
            /// The cached list of enumerations.
            QHash<QByteArray, int> m_enumerations;
    };

}

JVMExtension::JVMExtension(JVMInterpreter* interpreter, const QString& name, QObject* object)
    : d(new Private())
{
    d->m_object = object;

    d->debuginfo = object ? QString("name=%1 class=%2").arg(object->objectName()).arg(object->metaObject()->className()) : "NULL";
    krossdebug(QString("JVMExtension Ctor %1").arg(d->debuginfo));

    //testcase (TODO: something like this should go into JVMExtension)
    QFile toclass( QString("%1.class").arg(name) ); //e.g. "TestObject.class"
    toclass.open(QIODevice::ReadOnly);
    interpreter->addExtension(name, this, toclass.readAll());
    toclass.close();

/*
    //this is a testcase
    QFile ifacefile( QString("My%1.class").arg(name) );
    ifacefile.open(QIODevice::WriteOnly);
    QDataStream data(&ifacefile);
    ClassFileWriter writer(this);
    writer.writeInterface(data);
    ifacefile.close();
*/

    //TODO what we could do here is to create a class on the fly and register native
    //callbacks using the env->RegisterNatives method to be able to provide a Java
    //wrapper that calls slots and get/set properties fo the QObject.

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
                const QByteArray name = e.key(k);
                d->m_enumerations.insert(name, e.value(k));
            }
        }
    }
}

JVMExtension::~JVMExtension()
{
    krossdebug( QString("JVMExtension Dtor %1").arg(d->debuginfo) );
    delete d;
}

QObject* JVMExtension::object() const
{
    return d->m_object;
}
#if 0
jobject JVMExtension::callQMethod(JNIEnv* env, jstring method, jobjectArray args)
{
    //TODO: copy-pasting krossruby code will be easier :)
    QByteArray mname = JavaType<QString>::toVariant(method,env).toLatin1();
    int methodindex = d->m_methods[mname];
    QMetaMethod metamethod = d->m_object->metaObject()->method(methodindex);

    QList<QByteArray> typelist = metamethod.parameterTypes();
    int returntype = QVariant::nameToType( metamethod.typeName() );

    int typelistcount = typelists.count();
    QVarLengthArray<MetaType*> variantargs( typelistcount );
    QVarLengthArray<void*> voidstarargs( typelistcount );

    if(d->hasreturnvalue)
    {
        MetaType* returntype = RubyMetaTypeFactory::create( d->types[0], d->metatypes[0] );
        variantargs[0] = returntype;
        voidstarargs[0] = returntype->toVoidStar();
    }
    else
    {
        variantargs[0] = 0;
        voidstarargs[0] = (void*)0;
    }

    //TODO: arguments & return value
    QMetaObject::invokeMethod(d->m_object, mname);

    return 0;
}
#endif

jobject JVMExtension::callQMethod(JNIEnv* env, jstring method, int argc, jobject args[])
{
    QByteArray mname = JavaType<QString>::toVariant(method,env).toLatin1();
    int methodindex = d->m_methods[mname];
    QMetaMethod metamethod = d->m_object->metaObject()->method(methodindex);

    QList<QByteArray> typelist = metamethod.parameterTypes();
    bool hasreturnvalue = strcmp(metamethod.typeName(),"") != 0;
    int typelistcount = typelist.count();

    QVarLengthArray<int> types( typelistcount + 1 );
    QVarLengthArray<int> metatypes( typelistcount + 1 );

    // set the return type
    if(hasreturnvalue) {
        types[0] = QVariant::nameToType( metamethod.typeName() );
        if( types[0] == QVariant::Invalid || types[0] == QVariant::UserType ) {
            metatypes[0] = QMetaType::type( metamethod.typeName() );
            #ifdef KROSS_JVM_EXTENSION_DEBUG
                krossdebug( QString("JVMExtension::callQMethod return typeName=%1 typeId=%2").arg(metamethod.typeName()).arg(metatypes[0]) );
            #endif
        }
        else {
            metatypes[0] = QMetaType::Void; //FIXME: disable before release
            #ifdef KROSS_JVM_EXTENSION_DEBUG
                krossdebug( QString("JVMExtension::callQMethod return typeName=%1 typeId=%2 (with metatype=QMetaType::Void)").arg(metamethod.typeName()).arg(metatypes[0]) );
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
            #ifdef KROSS_JVM_EXTENSION_DEBUG
                krossdebug( QString("JVMExtension::callQMethod argument typeName=%1 typeId=%2").arg(metamethod.typeName()).arg(metatypes[idx]) );
            #endif
        }
        else {
            metatypes[idx] = QMetaType::Void; //FIXME: disable before release
            #ifdef KROSS_JVM_EXTENSION_DEBUG
                krossdebug( QString("JVMExtension::callQMethod argument typeName=%1 typeId=%2 set metatype=QMetaType::Void").arg(metamethod.typeName()).arg(metatypes[idx]) );
            #endif
        }

    }

    typelistcount = types.count();

    QVarLengthArray<MetaType*> variantargs( typelistcount );
    QVarLengthArray<void*> voidstarargs( typelistcount );

    #ifdef KROSS_JVM_EXTENSION_DEBUG
        krossdebug( QString("JVMExtension::callQMethod signature=%1 typeName=%2 argc=%3 typelistcount=%4").arg(metamethod.signature()).arg(metamethod.typeName()).arg(argc).arg(typelistcount) );
        for(int i = 0; i < types.count(); ++i)
            krossdebug( QString("  argument index=%1 typeId=%2 typeName=%3 metaTypeId=%4").arg(i).arg(types[i]).arg(QVariant::typeToName( (QVariant::Type)types[i] )).arg(metatypes[i]) );
    #endif

    Q_ASSERT(argc >= typelistcount - 1);  //typelistcount also contains the return type

    // set the return value
    if(hasreturnvalue)
    {
        MetaType* returntype = JVMMetaTypeFactory::create( env, types[0], metatypes[0] );
        variantargs[0] = returntype;
        voidstarargs[0] = returntype->toVoidStar();
    }
    else
    {
        variantargs[0] = 0;
        voidstarargs[0] = (void*)0;
    }

    // set the arguments values
    for(int idx = 1; idx < typelistcount; ++idx)
    {
        //krossdebug( QString("-----------> %1").arg( STR2CSTR( rb_inspect(argv[idx]) ) ) );
        MetaType* metatype = JVMMetaTypeFactory::create( env, types[idx], metatypes[idx], args[idx - 1] );
        if(! metatype) { // Seems JVMMetaTypeFactory::create returned an invalid JavaType.
            krosswarning( QString("JVMExtension::callMetaMethod Aborting cause JVMMetaTypeFactory::create returned NULL.") );
            for(int i = 0; i < idx; ++i) // Clear already allocated instances.
                delete variantargs[i];
            return 0; // abort execution.
        }
        variantargs[idx] = metatype;
        voidstarargs[idx] = metatype->toVoidStar();
    }

    // call the method now
    int r = d->m_object->qt_metacall(QMetaObject::InvokeMetaMethod, methodindex, &voidstarargs[0]);
    #ifdef KROSS_JVM_EXTENSION_DEBUG
        krossdebug( QString("RESULT nr=%1").arg(r) );
    #else
        Q_UNUSED(r);
    #endif

    // free the arguments
    for(int idx = 1; idx < typelistcount; ++idx)
    {
        delete variantargs[idx];
    }

    // eval the return-value
    if(hasreturnvalue)
    {
        QVariant result(variantargs[0]->typeId(), variantargs[0]->toVoidStar());
        #ifdef KROSS_JVM_EXTENSION_DEBUG
            QMetaMethod metamethod = d->m_object->metaObject()->method(methodindex);
            krossdebug( QString("JVMExtension::callQMethod Returnvalue typeId=%1 metamethod.typename=%2 variant.toString=%3 variant.typeName=%4").arg(variantargs[0]->typeId()).arg(metamethod.typeName()).arg(result.toString()).arg(result.typeName()) );
        #endif
        // free the return argument
        delete variantargs[0];
        // return the result
        return result.isNull() ? 0 : JavaType<QVariant>::toJObject(result, env);
    }
    return 0;
}

/*
ClassFile {
        u4 magic;
        u2 minor_version;
        u2 major_version;
        u2 constant_pool_count;
        cp_info constant_pool[constant_pool_count-1];
        u2 access_flags;
        u2 this_class;
        u2 super_class;
        u2 interfaces_count;
        u2 interfaces[interfaces_count];
        u2 fields_count;
        field_info fields[fields_count];
        u2 methods_count;
        method_info methods[methods_count];
        u2 attributes_count;
        attribute_info attributes[attributes_count];
    }
*/
