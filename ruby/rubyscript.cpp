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
#include "rubyinterpreter.h"
#include "rubyextension.h"
#include "rubyfunction.h"

#include <kross/core/manager.h>
#include <kross/core/action.h>

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>

extern NODE *ruby_eval_tree;

using namespace Kross;

static VALUE callExecuteException(VALUE args, VALUE error)
{
    #ifdef KROSS_RUBY_SCRIPT_DEBUG
        krossdebug("RubyScript::callExecuteException");
    #endif
    Q_UNUSED(args);
    Q_UNUSED(error);
    VALUE info = rb_gv_get("$!");
    VALUE bt = rb_funcall(info, rb_intern("backtrace"), 0);
    VALUE message = RARRAY(bt)->ptr[0];
    fprintf(stderr,"%s: %s (%s)\n", STR2CSTR(message), STR2CSTR(rb_obj_as_string(info)), rb_class2name(CLASS_OF(info)));
    for(int i = 1; i < RARRAY(bt)->len; ++i)
        if( TYPE(RARRAY(bt)->ptr[i]) == T_STRING )
            fprintf(stderr,"\tfrom %s\n", STR2CSTR(RARRAY(bt)->ptr[i]));
    ruby_nerrs++;
    return Qnil;
}

static VALUE callExecute(VALUE args)
{
    #ifdef KROSS_RUBY_SCRIPT_DEBUG
        krossdebug("RubyScript::callExecute");
    #endif
    Check_Type(args, T_ARRAY);
    VALUE self = rb_ary_entry(args, 0);
    VALUE src = rb_ary_entry(args, 1);
    VALUE fileName = rb_ary_entry(args, 2);
    //krossdebug(QString("RubyScript::callExecute script=%1").arg(STR2CSTR( rb_inspect(script) )));
    //krossdebug(QString("RubyScript::callExecute fileName=%1").arg(STR2CSTR( rb_inspect(fileName) )));
    //krossdebug(QString("RubyScript::callExecute src=%1").arg(STR2CSTR( rb_inspect(src) )));
    return rb_funcall(self, rb_intern("module_eval"), 2, src, fileName);
}

namespace Kross {

    /// \internal
    class RubyScriptPrivate {
        friend class RubyScript;

        static VALUE method_added(VALUE self, VALUE unit)
        {
            VALUE module = rb_funcall(self, rb_intern("module_function"), 1, unit);
            Q_ASSERT( TYPE(module) == TYPE(self) );

            char *methodname = rb_id2name(SYM2ID(unit));
            krossdebug( QString("RubyScriptPrivate::method_added methodname=%1").arg(methodname) );
            krossdebug(QString("RubyScriptPrivate::method_added self=%1").arg(STR2CSTR( rb_inspect(self) )));
            krossdebug(QString("RubyScriptPrivate::method_added module=%1").arg(STR2CSTR( rb_inspect(module) )));

            VALUE rubyscriptvalue = rb_funcall(self, rb_intern("const_get"), 1, ID2SYM(rb_intern("RUBYSCRIPTOBJ")));
            krossdebug(QString("RubyScriptPrivate::method_added rubyscriptvalue=%1").arg(STR2CSTR( rb_inspect(rubyscriptvalue) )));

            RubyScript* rubyscript;
            Data_Get_Struct(rubyscriptvalue, RubyScript, rubyscript);
            Q_ASSERT(rubyscript);
            krossdebug(QString("RubyScriptPrivate::method_added rubyscript=%1").arg(rubyscript->action()->objectName()));

            if( rubyscript->d->m_functions.contains(methodname) ) {
                QPair< QObject* , QString > f = rubyscript->d->m_functions[methodname];

                //TODO destroy if not needed any longer
                RubyFunction* function = new RubyFunction(f.first, f.second.toLatin1(), unit);

                QByteArray sendersignal = QString("2%1").arg(f.second).toLatin1();
                QByteArray receiverslot = QString("1%1").arg(f.second).toLatin1();
                if( QObject::connect(f.first, sendersignal, function, receiverslot) ) {
                    krossdebug( QString("=> RubyScript::method_added connected object='%1' signal='%2' method='%3'").arg(f.first->objectName()).arg(f.second).arg(methodname) );
                }
                else {
                    krossdebug( QString("=> RubyScript::method_added failed to connect object='%1' signal='%2' method='%3'").arg(f.first->objectName()).arg(f.second).arg(methodname) );
                }
                //rubyscript->addFunction(methodname)

                //script->action()->
                //QObject* object = extension->object();
                //const QMetaObject* metaobject = object->metaObject();
                //int idx = metaobject->indexOfSignal(methodname);
            }

            return module;
        }

        RubyScriptPrivate() : m_script(0), m_hasBeenSuccessFullyExecuted(false)
        {
            if(RubyScriptPrivate::s_krossScript == 0) {
                RubyScriptPrivate::s_krossScript = rb_define_class_under(RubyInterpreter::krossModule(), "Script", rb_cModule);
                rb_define_method(RubyScriptPrivate::s_krossScript, "method_added", (VALUE (*)(...))RubyScriptPrivate::method_added, 1);
            }
        }

        void addFunctions(ChildrenInterface* children)
        {
            QHashIterator< QString, ChildrenInterface::Options > it( children->objectOptions() );
            while(it.hasNext()) {
                it.next();
                if( it.value() & ChildrenInterface::AutoConnectSignals ) {
                    QObject* sender = children->object( it.key() );
                    if( sender ) {
                        const QMetaObject* metaobject = sender->metaObject();
                        const int count = metaobject->methodCount();
                        for(int i = 0; i < count; ++i) {
                            QMetaMethod metamethod = metaobject->method(i);
                            if( metamethod.methodType() == QMetaMethod::Signal ) {
                                const QString signature = metamethod.signature();
                                const QByteArray name = signature.left(signature.indexOf('(')).toLatin1();
                                m_functions.insert( name, QPair< QObject* , QString >(sender, signature) );
                            }
                        }
                    }
                }
            }
        }

        VALUE m_script;
        QStringList m_functionnames;
        static VALUE s_krossScript;
        bool m_hasBeenSuccessFullyExecuted;

        QHash< QByteArray, // the signalname, e.g. "mySignal"
                QPair< QObject* , // the QObject the signal belongs to
                       QString > // the signature, e.g. "mySignal(QString,int)"
             > m_functions;
    };

}

VALUE RubyScriptPrivate::s_krossScript = 0;

RubyScript::RubyScript(Kross::Interpreter* interpreter, Kross::Action* action)
    : Kross::Script(interpreter, action), d(new RubyScriptPrivate())
{
    d->m_script = rb_funcall(RubyScriptPrivate::s_krossScript, rb_intern("new"), 0);

    VALUE rubyscriptvalue = Data_Wrap_Struct(RubyScriptPrivate::s_krossScript, 0, 0, this);
    rb_define_const(d->m_script, "RUBYSCRIPTOBJ", rubyscriptvalue);

    rb_global_variable(&d->m_script);

    d->addFunctions( &Manager::self() );
    d->addFunctions( action );
}

RubyScript::~RubyScript()
{
    delete d;
}

void RubyScript::execute()
{
    #ifdef KROSS_RUBY_SCRIPT_DEBUG
        krossdebug("RubyScript::execute()");
    #endif

    const int critical = rb_thread_critical;
    rb_thread_critical = Qtrue;

    ruby_nerrs = 0;
    ruby_errinfo = Qnil;

    VALUE src = RubyType<QString>::toVALUE( action()->code() );
    StringValue(src);
    VALUE fileName = RubyType<QString>::toVALUE( action()->file() );
    StringValue(fileName);

    VALUE args = rb_ary_new2(3);
    rb_ary_store(args, 0, d->m_script); //self
    rb_ary_store(args, 1, src);
    rb_ary_store(args, 2, fileName);

    ruby_in_eval++;
    rb_rescue2((VALUE(*)(...))callExecute, args, (VALUE(*)(...))callExecuteException, Qnil, rb_eException, 0);
    ruby_in_eval--;

    if (ruby_nerrs != 0) {
        #ifdef KROSS_RUBY_SCRIPT_DEBUG
            krossdebug("Compilation has failed");
        #endif
        setError( QString("Failed to compile ruby code: %1").arg(STR2CSTR( rb_obj_as_string(ruby_errinfo) )) ); // TODO: get the error
        d->m_hasBeenSuccessFullyExecuted = false;
    } else {
        d->m_hasBeenSuccessFullyExecuted = true;
    }

    rb_thread_critical = critical;
}

QStringList RubyScript::functionNames()
{
    #ifdef KROSS_RUBY_SCRIPT_DEBUG
        krossdebug("RubyScript::getFunctionNames()");
    #endif

    if(not d->m_hasBeenSuccessFullyExecuted ) {
        execute();
        //d->m_functionnames = ; //TODO
    }
    return d->m_functionnames;
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
