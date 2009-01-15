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
#include "rubymodule.h"

#include <kross/core/manager.h>
#include <kross/core/action.h>

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>

extern NODE *ruby_eval_tree;

using namespace Kross;

static VALUE callExecuteException(VALUE self, VALUE error)
{
    //#ifdef KROSS_RUBY_SCRIPT_DEBUG
        krossdebug( QString("RubyScript::callExecuteException script=%1 error=%2")
                    .arg( STR2CSTR(rb_inspect(self)) ).arg( STR2CSTR(rb_inspect(error)) ) );
    //#else
        //Q_UNUSED(error);
    //#endif

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

    VALUE rubyscriptvalue = rb_funcall(self, rb_intern("const_get"), 1, ID2SYM(rb_intern("RUBYSCRIPTOBJ")));
    RubyScript* rubyscript;
    Data_Get_Struct(rubyscriptvalue, RubyScript, rubyscript);
    Q_ASSERT(rubyscript);
    rubyscript->setError(errormessage, tracemessage);

    return Qnil;
}

static VALUE callExecute(VALUE args)
{
    #ifdef KROSS_RUBY_SCRIPT_DEBUG
        krossdebug( QString("RubyScript::callExecute args=%1").arg( STR2CSTR(rb_inspect(args)) ) );
    #endif
    Q_ASSERT( TYPE(args) == T_ARRAY );
    VALUE self = rb_ary_entry(args, 0);
    Q_ASSERT( ! NIL_P(self) );
    VALUE src = rb_ary_entry(args, 1);
    Q_ASSERT( ! NIL_P(src) );
    VALUE fileName = rb_ary_entry(args, 2);
    Q_ASSERT( ! NIL_P(fileName) && TYPE(fileName) == T_STRING );
    //krossdebug(QString("RubyScript::callExecute script=%1").arg(STR2CSTR( rb_inspect(script) )));
    //krossdebug(QString("RubyScript::callExecute fileName=%1").arg(STR2CSTR( rb_inspect(fileName) )));
    //krossdebug(QString("RubyScript::callExecute src=%1").arg(STR2CSTR( rb_inspect(src) )));
    return rb_funcall(self, rb_intern("module_eval"), 2, src, fileName);
}

namespace Kross {

    /// \internal
    class RubyScriptPrivate {
        friend class RubyScript;

        /**
        * Return the \a RubyExtension instance that wraps our \a Kross::Action instance.
        */
        static VALUE action_instance(VALUE self, VALUE)
        {
            //krossdebug(QString("RubyScriptPrivate::action_instance"));
            VALUE rubyscriptvalue = rb_funcall(self, rb_intern("const_get"), 1, ID2SYM(rb_intern("RUBYSCRIPTOBJ")));
            RubyScript* rubyscript;
            Data_Get_Struct(rubyscriptvalue, RubyScript, rubyscript);
            Q_ASSERT(rubyscript);
            return RubyExtension::toVALUE(rubyscript->d->m_extension, /*owner*/ false);
        }

        /**
        * This is called if a method got added. We handle this here to be able
        * to e.g. attach our QObject signals to it if requested.
        */
        static VALUE method_added(VALUE self, VALUE unit)
        {
            VALUE module = rb_funcall(self, rb_intern("module_function"), 1, unit);
            Q_ASSERT( TYPE(module) == TYPE(self) );

            const char *methodname = rb_id2name(SYM2ID(unit));
            #ifdef KROSS_RUBY_SCRIPT_METHODADDED_DEBUG
                krossdebug(QString("RubyScriptPrivate::method_added methodname=%1 self=%2 module=%3").arg(methodname).arg(STR2CSTR( rb_inspect(self) )).arg(STR2CSTR( rb_inspect(module) )));
            #endif

            VALUE rubyscriptvalue = rb_funcall(self, rb_intern("const_get"), 1, ID2SYM(rb_intern("RUBYSCRIPTOBJ")));
            //krossdebug(QString("RubyScriptPrivate::method_added rubyscriptvalue=%1").arg(STR2CSTR( rb_inspect(rubyscriptvalue) )));

            RubyScript* rubyscript;
            Data_Get_Struct(rubyscriptvalue, RubyScript, rubyscript);
            Q_ASSERT(rubyscript);
            //krossdebug(QString("RubyScriptPrivate::method_added rubyscript=%1").arg(rubyscript->action()->objectName()));

            rubyscript->d->m_functionnames.append(methodname);

            if( rubyscript->d->m_functions.contains(methodname) ) {
                QPair< QObject* , QString > f = rubyscript->d->m_functions[methodname];

                VALUE method = rb_funcall(self, rb_intern("method"), 1, rb_str_new2(methodname));
                #ifdef KROSS_RUBY_SCRIPT_METHODADDED_DEBUG
                    krossdebug(QString("RubyScriptPrivate::method_added method=%1").arg(STR2CSTR( rb_inspect(method) )));
                #endif

                RubyFunction* function = rubyscript->connectFunction(f.first, f.second.toLatin1(), method);
                Q_UNUSED(function);
            }

            return module;
        }

        /**
        * The constructor which will initialize the Ruby "Script" class
        * if not already done before.
        */
        RubyScriptPrivate(RubyScript* q) : q(q), m_script(0), m_hasBeenSuccessFullyExecuted(false)
        {
            if(RubyScriptPrivate::s_krossScript == 0) {
                RubyScriptPrivate::s_krossScript = rb_define_class_under(RubyInterpreter::krossModule(), "Script", rb_cModule);
                rb_define_method(RubyScriptPrivate::s_krossScript, "action", (VALUE (*)(...))RubyScriptPrivate::action_instance, 0);
                rb_define_method(RubyScriptPrivate::s_krossScript, "method_added", (VALUE (*)(...))RubyScriptPrivate::method_added, 1);
            }
        }

        VALUE execute(VALUE code)
        {
            VALUE fileName = RubyType<QString>::toVALUE( q->action()->file() );
            StringValue(fileName);

            // needed to prevent infinitive loops ifour scripting call uses e.g. callFunction
            m_hasBeenSuccessFullyExecuted = true;

            const int critical = rb_thread_critical;
            rb_thread_critical = Qtrue;

            ruby_nerrs = 0;
            ruby_errinfo = Qnil;

            VALUE args = rb_ary_new2(3);
            rb_ary_store(args, 0, m_script); //self
            rb_ary_store(args, 1, code);
            rb_ary_store(args, 2, fileName);

            /* makes no sense to init the stack here since we share one stack anyway and it's handled in the interpreter already
            if (ruby_in_eval == 0) {
                #ifdef RUBY_INIT_STACK
                        RUBY_INIT_STACK
                #endif
            }
            */

            ruby_in_eval++;
            VALUE result = rb_rescue2((VALUE(*)(...))callExecute, args, (VALUE(*)(...))callExecuteException, m_script, rb_eException, 0);
            ruby_in_eval--;

            if (ruby_nerrs != 0) {
                //#ifdef KROSS_RUBY_SCRIPT_EXECUTE_DEBUG
                    krossdebug( QString("Compilation has failed. errorMessage=%1 errorTrace=\n%2\n").arg(q->errorMessage()).arg(q->errorTrace()) );
                //#endif
                m_hasBeenSuccessFullyExecuted = false;
            } else {
                m_hasBeenSuccessFullyExecuted = true;
            }

            #ifdef KROSS_RUBY_EXPLICIT_GC
                rb_gc();
            #endif

            rb_thread_critical = critical;
            return result;
        }

        /**
        * This method does walk through the list of children the \a ChildrenInterface
        * has - \a Action and \a Manager are implementing this interface - and
        * fills the m_functions hash which got used on the static method_added
        * method to e.g. connect signals with Ruby scripting functions.
        */
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

        RubyScript* q;
        /// The Ruby class instance that wraps a \a RubyScript instances.
        VALUE m_script;
        /// The extension that wraps our \a Kross::Action instance.
        RubyExtension* m_extension;
        /// A list of functionnames.
        QStringList m_functionnames;
        /// The Ruby class definition for instances of m_script.
        static VALUE s_krossScript;
        /// This is true if the last execution was successfully.
        bool m_hasBeenSuccessFullyExecuted;
        /// A hash for faster signal=>function access.
        QHash< QByteArray, // the signalname, e.g. "mySignal"
                QPair< QObject* , // the QObject the signal belongs to
                       QString > // the signature, e.g. "mySignal(QString,int)"
             > m_functions;
        /// List of \a RubyFunction instances the \a RubyScript is owner of.
        //QList< RubyFunction* > m_rubyfunctions;
        QList< QPointer<RubyFunction> > m_rubyfunctions;

        /// The map of local modules.
        QHash<QString, QPointer<RubyModule> > modules;

        #ifdef KROSS_RUBY_SCRIPT_CTORDTOR_DEBUG
            /// \internal string for debugging.
            QString debuginfo;
        #endif
    };

}

VALUE RubyScriptPrivate::s_krossScript = 0;

RubyScript::RubyScript(Kross::Interpreter* interpreter, Kross::Action* action)
    : Kross::Script(interpreter, action), d(new RubyScriptPrivate(this))
{
    #ifdef KROSS_RUBY_SCRIPT_CTORDTOR_DEBUG
        d->debuginfo = QString("name=%1 text=%2").arg(action->objectName()).arg(action->text());
        krossdebug( QString("RubyScript Ctor %1").arg(d->debuginfo) );
    #endif

    d->m_script = rb_funcall(RubyScriptPrivate::s_krossScript, rb_intern("new"), 0);
    Q_ASSERT(TYPE(d->m_script) == T_MODULE);
    rb_gc_register_address(&d->m_script);
    //rb_global_variable(&d->m_script);

    VALUE rubyscriptvalue = Data_Wrap_Struct(RubyScriptPrivate::s_krossScript, 0, 0, this);
    rb_define_const(d->m_script, "RUBYSCRIPTOBJ", rubyscriptvalue);

    d->m_extension = new RubyExtension(action);

    d->addFunctions( &Manager::self() );
    d->addFunctions( action );
}

RubyScript::~RubyScript()
{
    #ifdef KROSS_RUBY_SCRIPT_CTORDTOR_DEBUG
        krossdebug( QString("RubyScript Dtor %1").arg(d->debuginfo) );
    #endif

    qDeleteAll( d->m_rubyfunctions );
    //qDeleteAll( d->modules.values() );
    delete d->m_extension;

    rb_gc_unregister_address(&d->m_script);
    delete d;
}

RubyModule* RubyScript::module(QObject* object, const QString& modname)
{
    RubyModule* module = d->modules.contains(modname) ? d->modules[modname] : 0;
    if( ! module ) {
        module = new RubyModule(this, object, modname);
        d->modules.insert(modname, module);
    }
    return module;
}

bool RubyScript::isRubyScript(VALUE value)
{
    VALUE result = rb_funcall(value, rb_intern("kind_of?"), 1, RubyScriptPrivate::s_krossScript);
    return (TYPE(result) == T_TRUE);
}

void RubyScript::execute()
{
    #ifdef KROSS_RUBY_SCRIPT_EXECUTE_DEBUG
        krossdebug( "RubyScript::execute()" );
    #endif

    VALUE code = RubyType<QString>::toVALUE( action()->code() );
    StringValue(code);
    d->execute(code);
}

QStringList RubyScript::functionNames()
{
    #ifdef KROSS_RUBY_SCRIPT_FUNCTIONNAMES_DEBUG
        krossdebug( "RubyScript::getFunctionNames()" );
    #endif

    if( ! d->m_hasBeenSuccessFullyExecuted ) {
        execute(); // will fill the d->m_functionnames
    }
    return d->m_functionnames;
}

static VALUE callFunction2(VALUE args)
{
    #ifdef KROSS_RUBY_SCRIPT_DEBUG
        krossdebug( QString("RubyScript::callFunction2 args=%1").arg( STR2CSTR(rb_inspect(args)) ) );
    #endif
    Q_ASSERT( TYPE(args) == T_ARRAY );
    VALUE self = rb_ary_entry(args, 0);
    Q_ASSERT( ! NIL_P(self) );
    ID functionId = rb_ary_entry(args, 1);
    VALUE arguments = rb_ary_entry(args, 2);
    Q_ASSERT( TYPE(arguments) == T_ARRAY );
    return rb_funcall2(self, functionId, RARRAY(arguments)->len, RARRAY(arguments)->ptr);
}

QVariant RubyScript::callFunction(const QString& name, const QVariantList& args)
{
    QVariant result;
    #ifdef KROSS_RUBY_SCRIPT_CALLFUNCTION_DEBUG
        krossdebug( QString("RubyScript::callFunction() name=%1").arg(name) );
    #endif

    const int critical = rb_thread_critical;
    rb_thread_critical = Qtrue;
    ruby_in_eval++;
    //ruby_current_node

    if( ! d->m_hasBeenSuccessFullyExecuted ) {
        execute();
    }
    if( ! d->m_hasBeenSuccessFullyExecuted ) {
        #ifdef KROSS_RUBY_SCRIPT_CALLFUNCTION_DEBUG
            krossdebug("RubyScript::callFunction failed");
        #endif
        setError( QString("Failed to call function \"%1\": %2").arg(name).arg(STR2CSTR( rb_obj_as_string(ruby_errinfo) )) ); // TODO: get the error
    }
    else {
        //VALUE self = rb_eval_string("self");
        //krossdebug(QString("RubyScript::callFunction() ===> %1").arg(STR2CSTR(rb_inspect(self))));

        const int rnargs = args.size();
        VALUE *rargs = new VALUE[rnargs];
        for(int i = 0; i < rnargs; ++i) {
            rargs[i] = RubyType<QVariant>::toVALUE( args[i] );
        }

        VALUE args = rb_ary_new2(3);
        rb_ary_store(args, 0, d->m_script); //self
        rb_ary_store(args, 1, rb_intern(name.toLatin1()));
        rb_ary_store(args, 2, rb_ary_new4(rnargs, rargs));
        VALUE v = rb_rescue2((VALUE(*)(...))callFunction2, args, (VALUE(*)(...))callExecuteException, d->m_script, rb_eException, 0);
        result = RubyType<QVariant>::toVariant(v);

        delete[] rargs;
    }

    #ifdef KROSS_RUBY_SCRIPT_CALLFUNCTION_DEBUG
        krossdebug( QString("RubyScript::callFunction() result typeName=%1 toString=%2").arg(result.typeName()).arg(result.toString()) );
    #endif

    #ifdef KROSS_RUBY_EXPLICIT_GC
//         rb_gc(); // This one is plainly wrong, since there is a good deal of chance that it will delete the content of result before it is used
    #endif

    ruby_in_eval--;
    rb_thread_critical = critical;

    return result;
}

QVariant RubyScript::evaluate(const QByteArray& code)
{
    VALUE v = RubyType<QString>::toVALUE( code );
    StringValue(v);
    VALUE result = d->execute(v);
    return RubyType<QVariant>::toVariant(result);
}

RubyFunction* RubyScript::connectFunction(QObject* sender, const QByteArray& signature, VALUE method)
{
    RubyFunction* function = new RubyFunction(sender, signature, method);
    QByteArray sendersignal = QString("2%1").arg(signature.data()).toLatin1(); //prepanding 2 means SIGNAL()
    QByteArray receiverslot = QString("1%1").arg(signature.data()).toLatin1(); //prepanding 1 means SLOT()
    if( ! QObject::connect(sender, sendersignal, function, receiverslot) ) {
        krosswarning( QString("RubyScript::method_added failed to connect object='%1' signal='%2' method='%3'").arg(sender->objectName()).arg(signature.data()).arg(STR2CSTR(rb_inspect(method))) );
        delete function;
        return 0;
    }
    #ifdef KROSS_RUBY_SCRIPT_CONNECTFUNCTION_DEBUG
        krossdebug( QString("RubyScript::method_added connected object='%1' signal='%2' method='%3'").arg(sender->objectName()).arg(signature.data()).arg(STR2CSTR(rb_inspect(method))) );
    #endif
    d->m_rubyfunctions.append( function );
    return function;
}
