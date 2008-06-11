/***************************************************************************
 * rubyobject.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
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

#include "rubyobject.h"
#include "rubyvariant.h"

using namespace Kross;

static VALUE callExecuteException(VALUE self, VALUE error)
{
    #ifdef KROSS_RUBY_OBJECT_DEBUG
        krossdebug( QString("RubyScript::callExecuteException script=%1 error=%2")
                    .arg( STR2CSTR(rb_inspect(self)) ).arg( STR2CSTR(rb_inspect(error)) ) );
    #else
        Q_UNUSED(self);
        Q_UNUSED(error);
    #endif

    VALUE info = rb_gv_get("$!");
    VALUE bt = rb_funcall(info, rb_intern("backtrace"), 0);
    VALUE message = RARRAY(bt)->ptr[0];

    QString errormessage = QString("%1: %2 (%3)")
                            .arg( STR2CSTR(message) )
                            .arg( STR2CSTR(rb_obj_as_string(info)) )
                            .arg( rb_class2name(CLASS_OF(info)) );
    fprintf(stderr, "%s\n", errormessage.toLatin1().data());

    QString tracemessage;
    for(int i = 1; i < RARRAY(bt)->len; ++i) {
        if( TYPE(RARRAY(bt)->ptr[i]) == T_STRING ) {
            QString s = QString("%1\n").arg( STR2CSTR(RARRAY(bt)->ptr[i]) );
            Q_ASSERT( ! s.isNull() );
            tracemessage += s;
            fprintf(stderr, "\t%s", s.toLatin1().data());
        }
    }

    ruby_nerrs++;
/*
    VALUE rubyscriptvalue = rb_funcall(self, rb_intern("const_get"), 1, ID2SYM(rb_intern("RUBYSCRIPTOBJ")));
    RubyScript* rubyscript;
    Data_Get_Struct(rubyscriptvalue, RubyScript, rubyscript);
    Q_ASSERT(rubyscript);
    rubyscript->setError(errormessage, tracemessage);
*/
    return Qnil;
}

static VALUE callFunction2(VALUE args)
{
    #ifdef KROSS_RUBY_OBJECT_DEBUG
        krossdebug( QString("RubyObject::callFunction2 args=%1").arg( STR2CSTR(rb_inspect(args)) ) );
    #endif
    Q_ASSERT( TYPE(args) == T_ARRAY );
    VALUE self = rb_ary_entry(args, 0);
    Q_ASSERT( ! NIL_P(self) );
    ID functionId = rb_ary_entry(args, 1);
    VALUE arguments = rb_ary_entry(args, 2);
    Q_ASSERT( TYPE(arguments) == T_ARRAY );
    return rb_funcall2(self, functionId, RARRAY(arguments)->len, RARRAY(arguments)->ptr);
}

class RubyObject::Private
{
public:
    Private() : rbobject(0) {};
    Private(const VALUE& object) : rbobject(object) {};
    const VALUE rbobject;
    QStringList calls;
    #ifdef KROSS_RUBY_OBJECT_DEBUG
        QString debug;
    #endif
};

RubyObject::RubyObject()
    : Kross::Object()
    , d(new Private)
{
    #ifdef KROSS_RUBY_OBJECT_DEBUG
        d->debug = QString("type=NIL");
        krossdebug( QString("RubyObject::RubyObject() constructor: %1").arg(d->debug) );
    #endif
}

RubyObject::RubyObject(const VALUE& object)
    : Kross::Object()
    , d(new Private(object))
{
    #ifdef KROSS_RUBY_OBJECT_DEBUG
        d->debug = QString("type=%1 value=%2").arg(TYPE(object)).arg(STR2CSTR(rb_inspect(object)));
        krossdebug( QString("RubyObject::RubyObject(const VALUE& object) constructor: %1").arg(d->debug) );
    #endif

    VALUE args[] = { Qfalse };
    VALUE methods;
    const char* method;
    methods = rb_class_instance_methods(1, args, CLASS_OF(object));
    for (int i = 0; i < RARRAY(methods)->len; i++) {
        method = StringValuePtr(RARRAY(methods)->ptr[i]);
        krossdebug( QString("RubyObject::RubyObject() method=%1").arg( method ));
        d->calls << method;
    }
}

RubyObject::~RubyObject()
{
    #ifdef KROSS_RUBY_OBJECT_DEBUG
        krossdebug( QString("RubyObject::~RubyObject() destructor: %1").arg(d->debug) );
    #endif
    delete d;
}

QVariant RubyObject::callMethod(const QString& name, const QVariantList& args)
{
    #ifdef KROSS_RUBY_OBJECT_DEBUG
        krossdebug( QString("RubyObject::call(%1): %2").arg(name).arg(d->debug) );
    #endif

    QVariant result;
    const int rnargs = args.size();
    VALUE *rargs = new VALUE[rnargs];
    for(int i = 0; i < rnargs; ++i) {
        rargs[i] = RubyType<QVariant>::toVALUE( args[i] );
    }

    VALUE vargs = rb_ary_new2(3);
    rb_ary_store(vargs, 0, d->rbobject); //self
    rb_ary_store(vargs, 1, rb_intern(name.toLatin1()));
    rb_ary_store(vargs, 2, rb_ary_new4(rnargs, rargs));
    VALUE v = rb_rescue2((VALUE(*)(...))callFunction2, vargs, (VALUE(*)(...))callExecuteException, d->rbobject, rb_eException, 0);
    result = RubyType<QVariant>::toVariant(v);

    delete[] rargs;
    return result;
}

QStringList RubyObject::methodNames()
{
    return d->calls;
}

VALUE RubyObject::rbObject()
{
    return d->rbobject;
}

