/***************************************************************************
 * rubyscript.h
 * This file is part of the KDE project
 * copyright (C)2005;2007 by Cyrille Berger (cberger@cberger.net)
 * copyright (C)2006 by Sebastian Sauer (mail@dipe.org)
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

#include "rubyscript.h"
#include "rubyvariant.h"

#include <ruby.h>
#include <env.h>
#include <rubysig.h>
#include <node.h>

#include <kross/core/action.h>

#include "rubyconfig.h"
#include "rubyextension.h"
#include "rubyinterpreter.h"

extern NODE *ruby_eval_tree;

using namespace Kross;

namespace Kross {

    namespace Internals {
        namespace Script {
            static VALUE method_added(VALUE self, VALUE unit)
            {
                rb_funcall(self, rb_intern("module_function"), unit);
                return self;
            }
        }
    }
    /// \internal
    class RubyScriptPrivate {
        friend class RubyScript;
        RubyScriptPrivate() : m_script(0), m_hasBeenSuccessFullyExecuted(false)
        {
            if(RubyScriptPrivate::s_krossScript == 0)
            {
                RubyScriptPrivate::s_krossScript = rb_define_class_under(RubyInterpreter::krossModule(), "Script", rb_cModule);
                rb_define_method(RubyScriptPrivate::s_krossScript, "method_added", (VALUE (*)(...))Internals::Script::method_added, 1);
            }
        }
        VALUE m_script;
        QStringList m_functions;
        static VALUE s_krossScript;
        bool m_hasBeenSuccessFullyExecuted;
    };

}

VALUE RubyScriptPrivate::s_krossScript = 0;

RubyScript::RubyScript(Kross::Interpreter* interpreter, Kross::Action* Action)
    : Kross::Script(interpreter, Action), d(new RubyScriptPrivate())
{
    d->m_script = rb_funcall(RubyScriptPrivate::s_krossScript, rb_intern("new"), 0);
    rb_global_variable(&d->m_script);
}

RubyScript::~RubyScript()
{
}

static VALUE callWrappedExecute(VALUE args)
{
    Check_Type(args, T_ARRAY);
    VALUE script = rb_ary_entry(args, 0);
    VALUE id = rb_ary_entry(args, 1);
    VALUE src = rb_ary_entry(args, 2);
    VALUE fileName = rb_ary_entry(args, 3);
    //krossdebug(QString("1: %1").arg(STR2CSTR( rb_inspect(script) )));
    //krossdebug(QString("2: %1").arg(STR2CSTR( rb_inspect(id) )));
    //krossdebug(QString("3: %1").arg(STR2CSTR( rb_inspect(fileName) )));
    //krossdebug(QString("4: %1").arg(STR2CSTR( rb_inspect(src) )));
    return rb_funcall(script, id, 2, src, fileName);
}

static VALUE handleExecuteExecption(VALUE args, VALUE error)
{
    Q_UNUSED(args);
    Q_UNUSED(error);
    VALUE info = rb_gv_get("$!");
    VALUE bt = rb_funcall(info, rb_intern("backtrace"), 0);
    VALUE message = RARRAY(bt)->ptr[0];
    fprintf(stderr,"%s: %s (%s)\n", STR2CSTR(message), STR2CSTR(rb_obj_as_string(info)), rb_class2name(CLASS_OF(info)));
    for(int i = 1; i < RARRAY(bt)->len; ++i)
        if( TYPE(RARRAY(bt)->ptr[i]) == T_STRING )
            fprintf(stderr,"\tfrom %s\n", STR2CSTR(RARRAY(bt)->ptr[i]));
    return Qnil;
}

void RubyScript::execute()
{
    #ifdef KROSS_RUBY_SCRIPT_DEBUG
        krossdebug("RubyScript::execute()");
    #endif

    ruby_nerrs = 0;
    ruby_errinfo = Qnil;
    VALUE src = RubyType<QString>::toVALUE( action()->code() );
    StringValue(src);
    VALUE fileName = RubyType<QString>::toVALUE( action()->file() );
    StringValue(fileName);

    const int critical = rb_thread_critical;
    rb_thread_critical = Qtrue;
    ruby_in_eval++;

    VALUE args = rb_ary_new2 (4);
    rb_ary_store(args, 0, d->m_script);
    rb_ary_store(args, 1, rb_intern("module_eval"));
    rb_ary_store(args, 2, src);
    rb_ary_store(args, 3, fileName);
    rb_rescue2((VALUE(*)(...))callWrappedExecute, args, (VALUE(*)(...))handleExecuteExecption, Qnil, rb_eException, 0);

    ruby_in_eval--;
    rb_thread_critical = critical;

    if (ruby_nerrs != 0) {
        #ifdef KROSS_RUBY_SCRIPT_DEBUG
            krossdebug("Compilation has failed");
        #endif
        setError( QString("Failed to compile ruby code: %1").arg(STR2CSTR( rb_obj_as_string(ruby_errinfo) )) ); // TODO: get the error
        d->m_hasBeenSuccessFullyExecuted = false;
    } else {
        d->m_hasBeenSuccessFullyExecuted = true;
    }

#if 0
    if (result != 0) {
        #ifdef KROSS_RUBY_SCRIPT_DEBUG
            krossdebug("RubyScript::execute failed");
        #endif

        /*
        if( TYPE( ruby_errinfo )  == T_DATA && RubyExtension::isOfExceptionType( ruby_errinfo ) )
        {
            setError( RubyExtension::convertToException( ruby_errinfo ) );
        } else
        */
        setError( QString("Failed to execute ruby code: %1").arg(STR2CSTR( rb_obj_as_string(ruby_errinfo) )) ); // TODO: get the error
    }
#endif
}

QStringList RubyScript::functionNames()
{
    #ifdef KROSS_RUBY_SCRIPT_DEBUG
        krossdebug("RubyScript::getFunctionNames()");
    #endif

    if(not d->m_hasBeenSuccessFullyExecuted ) {
        execute();
    }
    return d->m_functions;
}

QVariant RubyScript::callFunction(const QString& name, const QVariantList& args)
{
    QVariant result;
    #ifdef KROSS_RUBY_SCRIPT_DEBUG
        krossdebug(QString("RubyScript::callFunction() name=%1").arg(name));
    #endif

    const int critical = rb_thread_critical;
    rb_thread_critical = Qtrue;
    ruby_in_eval++;
    //ruby_current_node

    if(not d->m_hasBeenSuccessFullyExecuted) {
        execute();
    }
    if(not d->m_hasBeenSuccessFullyExecuted)
    {
        #ifdef KROSS_RUBY_SCRIPT_DEBUG
            krossdebug("RubyScript::callFunction failed");
        #endif
        setError( QString("Failed to call function \"%1\": %2").arg(name).arg(STR2CSTR( rb_obj_as_string(ruby_errinfo) )) ); // TODO: get the error
    }
    else {
        VALUE self = rb_eval_string("self");
        //krossdebug(QString("RubyScript::callFunction() ===> %1").arg(STR2CSTR(rb_inspect(self))));

        const int rnargs = args.size();
        VALUE *rargs = new VALUE[rnargs];
        for(int i = 0; i < rnargs; ++i) {
            rargs[i] = RubyType<QVariant>::toVALUE( args[i] );
        }

        //VALUE r = rb_eval_string("myFunc()");
        VALUE v = rb_funcall2(self, rb_intern(name.toLatin1()), rnargs, rargs);
        result = RubyType<QVariant>::toVariant(v);
        delete[] rargs;
    }

    ruby_in_eval--;
    rb_thread_critical = critical;

    #ifdef KROSS_RUBY_SCRIPT_DEBUG
        krossdebug(QString("RubyScript::callFunction() Result typeName=%1 toString=%2").arg(result.typeName()).arg(result.toString()));
    #endif
    return result;
}
