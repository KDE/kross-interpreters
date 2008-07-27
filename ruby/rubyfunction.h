/***************************************************************************
 * rubyfunction.h
 * This file is part of the KDE project
 * copyright (C)2007 by Sebastian Sauer (mail@dipe.org)
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

#ifndef KROSS_RUBYFUNCTION_H
#define KROSS_RUBYFUNCTION_H

#include "rubyconfig.h"
#include <kross/core/krossconfig.h>
#include <kross/core/manager.h>
#include <kross/core/metafunction.h>

#include <QObject>
#include <QWidget>
#include <QMetaObject>
#include <QMetaMethod>
#include <QByteArray>

namespace Kross {

    /**
     * The RubyFunction class implements a QObject to provide
     * an adaptor between Qt signals+slots and ruby functions.
     */
    class RubyFunction : public MetaFunction
    {
        public:

            /**
            * Constructor.
            *
            * \param sender The QObject instance that sends the signal.
            * \param signal The signature of the signal the QObject emits.
            * \param method The callable ruby methods that should
            * be executed if the QObject emits the signal.
            */
            RubyFunction(QObject* sender, const QByteArray& signal, VALUE method)
                : MetaFunction(sender, signal), m_method(method)
            {
                #ifdef KROSS_RUBY_FUNCTION_CTORDTOR_DEBUG
                    m_debuginfo = QString("sender=%1 signature=%2 method=%3")
                        .arg( sender ? QString("%1 (%2)").arg(sender->objectName()).arg(sender->metaObject()->className()) : "NULL" )
                        .arg( signal.data() )
                        .arg( STR2CSTR(rb_inspect(method)) );
                    krossdebug( QString("RubyFunction Ctor %1").arg(m_debuginfo) );
                #endif
                rb_gc_register_address(&m_method);
            }

            /**
            * Destructor.
            */
            virtual ~RubyFunction()
            {
                #ifdef KROSS_RUBY_FUNCTION_CTORDTOR_DEBUG
                    krossdebug( QString("RubyFunction Dtor %1").arg(m_debuginfo) );
                #endif
                rb_gc_unregister_address(&m_method);
            }

            /**
            * This static function is called by Ruby if the callFunction bellow
            * raises an exception.
            */
            static VALUE callFunctionException(VALUE args, VALUE error)
            {
                //#ifdef KROSS_RUBY_FUNCTION_DEBUG
                    krossdebug( QString("RubyFunction callFunctionException args=%1 error=%2")
                                .arg( STR2CSTR(rb_inspect(args)) ).arg( STR2CSTR(rb_inspect(error)) ) );
                //#else
                //    Q_UNUSED(args);
                //    Q_UNUSED(error);
                //#endif
                VALUE info = rb_gv_get("$!");
                VALUE bt = rb_funcall(info, rb_intern("backtrace"), 0);
                VALUE message = RARRAY(bt)->ptr[0];
                fprintf(stderr,"%s: %s (%s)\n", STR2CSTR(message), STR2CSTR(rb_obj_as_string(info)), rb_class2name(CLASS_OF(info)));
                for(int i = 1; i < RARRAY(bt)->len; ++i)
                    if( TYPE(RARRAY(bt)->ptr[i]) == T_STRING )
                        fprintf(stderr,"\tfrom %s\n", STR2CSTR(RARRAY(bt)->ptr[i]));
                //ruby_nerrs++;
                return Qnil;
            }

            /**
            * This static function is called by Ruby to perform actions within
            * a rescue block to provide with the callFunctionException function
            * above an exception handler.
            */
            static VALUE callFunction(VALUE args)
            {
                #ifdef KROSS_RUBY_FUNCTION_DEBUG
                    krossdebug( QString("RubyFunction callFunction args=%1").arg(STR2CSTR(rb_inspect(args))) );
                #endif
                Q_ASSERT( TYPE(args) == T_ARRAY );
                VALUE self = rb_ary_entry(args, 0);
                int argsize = FIX2INT( rb_ary_entry(args, 1) );
                VALUE arguments = rb_ary_entry(args, 2);
                VALUE* argumentsP = new VALUE[argsize];
                for(int idx = 0; idx < argsize; idx++)
                {
                    argumentsP[idx] = rb_ary_entry(arguments, idx+1);
                }
                //krossdebug(QString("RubyScript::callExecute script=%1").arg(STR2CSTR( rb_inspect(script) )));
                //krossdebug(QString("RubyScript::callExecute fileName=%1").arg(STR2CSTR( rb_inspect(fileName) )));
                //krossdebug(QString("RubyScript::callExecute src=%1").arg(STR2CSTR( rb_inspect(src) )));
                VALUE result = rb_funcall2(self, rb_intern("call"), argsize, argumentsP);
                delete[] argumentsP;
                return result;
            }

            /**
            * This method got called if a method this QObject instance
            * defines should be invoked.
            */
            int qt_metacall(QMetaObject::Call _c, int _id, void **_a)
            {
                _id = QObject::qt_metacall(_c, _id, _a);
                #ifdef KROSS_RUBY_FUNCTION_DEBUG
                    //krossdebug(QString("RubyFunction::qt_metacall id=%1").arg(_id));
                #endif
                if(_id >= 0 && _c == QMetaObject::InvokeMetaMethod) {
                    switch(_id) {
                        case 0: {
                            // convert the arguments
                            QMetaMethod method = metaObject()->method( metaObject()->indexOfMethod(m_signature) );
                            QList<QByteArray> params = method.parameterTypes();
                            const int argsize = params.size();
                            VALUE args = rb_ary_new2(argsize);
                            int idx = 1;
                            foreach(QByteArray param, params) {
                                int tp = QVariant::nameToType( param.constData() );
                                switch(tp) {
                                    case QVariant::Invalid: // fall through
                                    case QVariant::UserType: {
                                        tp = QMetaType::type( param.constData() );
                                        #ifdef KROSS_RUBY_FUNCTION_DEBUG
                                            krossdebug( QString("RubyFunction::qt_metacall: metatypeId=%1").arg(tp) );
                                        #endif
                                        switch( tp ) {
                                            case QMetaType::QObjectStar: {
                                                QObject* obj = (*reinterpret_cast< QObject*(*)>( _a[idx] ));
                                                rb_ary_store(args, idx, RubyExtension::toVALUE( new RubyExtension(obj), true /*owner*/ ) );
                                            } break;
                                            case QMetaType::QWidgetStar: {
                                                QWidget* obj = (*reinterpret_cast< QWidget*(*)>( _a[idx] ));
                                                rb_ary_store(args, idx, RubyExtension::toVALUE( new RubyExtension(obj), true /*owner*/ ) );
                                            } break;
                                            default: {
                                                rb_ary_store(args, idx, Qnil);
                                            } break;
                                        }
                                    } break;
                                    default: {
                                        QVariant v(tp, _a[idx]);

                                        if( ! Kross::Manager::self().strictTypesEnabled() ) {
                                            if( v.type() == QVariant::Invalid && QByteArray(param.constData()).endsWith("*") ) {
                                                QObject* obj = (*reinterpret_cast<QObject*(*)>( _a[idx] ));
                                                v.setValue( (QObject*) obj );
                                            }
                                        }

                                        #ifdef KROSS_RUBY_FUNCTION_DEBUG
                                            krossdebug( QString("RubyFunction::qt_metacall argument param=%1 typeId=%2").arg(param.constData()).arg(tp) );
                                        #endif
                                        rb_ary_store(args, idx, RubyType<QVariant>::toVALUE(v));
                                    } break;
                                }
//rb_gc_register_address(&args[idx-1]);
                                ++idx;
                            }

                            /* makes no sense to init the stack here since we share one stack anyway and it's handled in the interpreter already
                            if (ruby_in_eval == 0) {
                                #ifdef RUBY_INIT_STACK
                                    RUBY_INIT_STACK
                                #endif
                            }
                            */

                            // call the ruby function
                            //VALUE result = rb_funcall2(m_method, rb_intern("call"), argsize, args);

                            //TODO optimize
                            ruby_in_eval++;
                            VALUE argarray = rb_ary_new2(3);
                            rb_ary_store(argarray, 0, m_method); //self
                            rb_ary_store(argarray, 1, INT2FIX(argsize));
                            rb_ary_store(argarray, 2, args);
                            VALUE result = rb_rescue2((VALUE(*)(...))callFunction, argarray, (VALUE(*)(...))callFunctionException, Qnil, rb_eException, 0);
                            ruby_in_eval--;

                            // finally set the returnvalue
                            m_tmpResult = RubyType<QVariant>::toVariant(result);
                            #ifdef KROSS_RUBY_FUNCTION_DEBUG
                                QObject* sender = QObject::sender();
                                krossdebug( QString("RubyFunction::qt_metacall sender.objectName=%1 sender.className=%2 result=%3 variantresult=%4").arg(sender->objectName()).arg(sender->metaObject()->className()).arg(STR2CSTR(rb_inspect(result))).arg(m_tmpResult.toString()) );
                            #endif
                            //_a[0] = Kross::MetaTypeVariant<QVariant>(d->tmpResult).toVoidStar();
                            _a[0] = &(m_tmpResult);

//for(int i = 0; i < argsize; ++i) rb_gc_unregister_address(&args[i]);
                            #ifdef KROSS_RUBY_EXPLICIT_GC
                                rb_gc();
                            #endif
                        } break;
                    }
                    _id -= 1;
                }
                return _id;
            }

        private:
            /// The pointer to the Ruby method.
            VALUE m_method;
            /// Dummy variable used to store the last result of a method call.
            QVariant m_tmpResult;

            #ifdef KROSS_RUBY_FUNCTION_CTORDTOR_DEBUG
                /// \internal string for debugging.
                QString m_debuginfo;
            #endif
    };

}

#endif
