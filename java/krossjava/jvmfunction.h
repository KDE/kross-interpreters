/***************************************************************************
 * jvmfunction.h
 * This file is part of the KDE project
 * copyright (C)2007 by Sebastian Sauer <mail@dipe.org>
 * copyright (C)2007 by Vincent Verhoeven <verhoevenv@gmail.com>
 *
 * Parts of the code are from kjsembed4 SlotProxy
 * Copyright (C) 2005, 2006 KJSEmbed Authors.
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

#ifndef KROSS_JVMFUNCTION_H
#define KROSS_JVMFUNCTION_H

#include "jvmconfig.h"
#include "jvmvariant.h"
#include "jvminterpreter.h"
#include <kross/core/krossconfig.h>
#include <kross/core/metafunction.h>

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QByteArray>

namespace Kross {

    /**
     * The JVMFunction class implements a QObject to provide
     * an adaptor between Qt signals+slots and Java functions (jmethods).
     */
    class JVMFunction : public MetaFunction
    {
        public:

            /**
            * Constructor.
            *
            * \param sender The QObject instance that sends the signal.
            * \param signal The signature of the signal the QObject emits.
            * \param instance The Java Object on which to invoke the method.
            * \param callable The callable Java function (a java.lang.reflect.Method) that should
            * be executed if the QObject emits the signal.
            * \param env The Java environment where the Java objects live.
            */
            JVMFunction(QObject* sender, const QByteArray& signal, jobject instance, jobject callable, JNIEnv* env)
                : MetaFunction(sender, signal), m_env(env) {
                m_callable = m_env->NewGlobalRef(callable);
                m_instance = m_env->NewGlobalRef(instance);
            }

            /**
            * Destructor.
            */
            virtual ~JVMFunction() {
                m_env->DeleteGlobalRef(m_instance);
                m_env->DeleteGlobalRef(m_callable);
            }

            /**
            * This method got called if a method this QObject instance
            * defines should be invoked.
            */
            int qt_metacall(QMetaObject::Call _c, int _id, void **_a)
            {
                //The method might be called from a different thread, so we get the right JNIEnv.
                JNIEnv* m_env = JVMInterpreter::getEnv();

                _id = QObject::qt_metacall(_c, _id, _a);
                #ifdef KROSS_JVM_FUNCTION_DEBUG
                    krossdebug(QString("JVMFunction::qt_metacall id=%1").arg(_id));
                #endif
                if(_id >= 0 && _c == QMetaObject::InvokeMetaMethod) {
                    switch(_id) {
                        case 0: {
                            // convert the arguments
                            QMetaMethod method = metaObject()->method( metaObject()->indexOfMethod(m_signature) );
                            QList<QByteArray> params = method.parameterTypes();
                            //Py::Tuple args( params.size() );
                            jclass objclass = m_env->FindClass("java/lang/Object");
                            jobjectArray args = m_env->NewObjectArray(params.size(), objclass, NULL);
                            int idx = 1;
                            foreach(QByteArray param, params) {
                                int tp = QVariant::nameToType( param.constData() );
                                switch(tp) {

                                    case QVariant::Invalid: // fall through
                                    case QVariant::UserType: {
                                        tp = QMetaType::type( param.constData() );
                                        #ifdef KROSS_JVM_FUNCTION_DEBUG
                                            krossdebug( QString("JVMFunction::qt_metacall: metatypeId=%1").arg(tp) );
                                        #endif
                                        switch( tp ) {
                                            case QMetaType::QObjectStar: {
                                                QObject* obj = (*reinterpret_cast< QObject*(*)>( _a[idx] ));
                                                //args[idx-1] = Py::asObject(new JVMExtension(obj, false));
                                                const JVMExtension* ext = JVMInterpreter::extension(obj);
                                                if(ext == 0)
                                                    ext = new JVMExtension(obj);
                                                m_env->SetObjectArrayElement(args, idx-1, ext->javaobject());
                                            } break;
#if 0
                                            case QMetaType::QWidgetStar: {
                                                QWidget* obj = (*reinterpret_cast< QWidget*(*)>( _a[idx] ));
                                                //args[idx-1] = Py::asObject(new JVMExtension(obj, false));
                                                args[idx-1] = Py::asObject(new JVMExtension(obj));
                                            } break;
#endif
                                            default: {
                                                m_env->SetObjectArrayElement(args, idx-1, NULL);
                                            } break;
                                        }
                                    } break;
                                    default: {
                                        QVariant v(tp, _a[idx]);
                                        #ifdef KROSS_JVM_FUNCTION_DEBUG
                                            krossdebug( QString("JVMFunction::qt_metacall argument param=%1 typeId=%2").arg(param.constData()).arg(tp) );
                                        #endif
                                        jobject javaArg = JavaType<QVariant>::toJObject(v, m_env);
                                        m_env->SetObjectArrayElement(args, idx-1, javaArg);
                                    } break;
                                }
                                ++idx;
                            }

                            jclass methodclass = m_env->FindClass("java/lang/reflect/Method");
                            jmethodID invoke = m_env->GetMethodID(methodclass,"invoke","(Ljava/lang/Object;[Ljava/lang/Object;)Ljava/lang/Object;");
                            jobject result = m_env->CallObjectMethod(m_callable, invoke, m_instance, args);
#if 0
                            try {
                                // call the JVM function
                                result = m_callable.apply(args);
                            }
                            catch(Py::Exception& e) {
                                QStringList trace;
                                int lineno;
                                JVMInterpreter::extractException(trace, lineno);
                                krosswarning( QString("JVMFunction::qt_metacall exception on line %1:\n%2 \n%3").arg(lineno).arg(Py::value(e).as_string().c_str()).arg(trace.join("\n")) );
                                PyErr_Print(); //e.clear();
                                return -1;
                            }

                            // finally set the returnvalue
                            m_tmpResult = JavaType<QVariant>::toVariant(result, m_env);
                            #ifdef KROSS_JVM_FUNCTION_DEBUG
                                QObject* sender = QObject::sender();
                                krossdebug( QString("JVMFunction::qt_metacall sender.objectName=%1 sender.className=%2 variantresult=%3").arg(sender->objectName()).arg(sender->metaObject()->className()).arg(m_tmpResult.toString()) );
                            #endif
                            //_a[0] = Kross::MetaTypeVariant<QVariant>(d->tmpResult).toVoidStar();
                            _a[0] = &(m_tmpResult);
#endif
                        } break;
                    }
                    _id -= 1;
                }
                return _id;
            }

        private:
            jobject m_instance;
            jobject m_callable;
            JNIEnv* m_env;
            QVariant m_tmpResult;
    };

}

#endif
