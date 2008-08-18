/***************************************************************************
 * pythonfunction.h
 * This file is part of the KDE project
 * copyright (C)2006 by Sebastian Sauer (mail@dipe.org)
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

#ifndef KROSS_PYTHONFUNCTION_H
#define KROSS_PYTHONFUNCTION_H

#include "pythonconfig.h"
#include <kross/core/krossconfig.h>
#include <kross/core/metafunction.h>

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QByteArray>

namespace Kross {

    /**
     * The PythonFunction class implements a QObject to provide
     * an adaptor between Qt signals+slots and python functions.
     */
    class PythonFunction : public MetaFunction
    {
        public:

            /**
            * Constructor.
            *
            * \param sender The QObject instance that sends the signal.
            * \param signal The signature of the signal the QObject emits.
            * \param callable The callable python function that should
            * be executed if the QObject emits the signal.
            */
            PythonFunction(QObject* sender, const QByteArray& signal, const Py::Callable& callable)
                : MetaFunction(sender, signal), m_callable(callable)
            {
                #ifdef KROSS_PYTHON_FUNCTION_DEBUG
                    m_debuginfo = QString("sender=%1 [%2] signal=%3").arg(sender->objectName()).arg(sender->metaObject()->className()).arg(signal.constData());
                    krossdebug( QString("PythonFunction::Constructor: %1").arg(m_debuginfo) );
                #endif
            }

            /**
            * Destructor.
            */
            virtual ~PythonFunction()
            {
                #ifdef KROSS_PYTHON_FUNCTION_DEBUG
                    krossdebug( QString("PythonFunction::Destructor: %1").arg(m_debuginfo) );
                #endif
            }

            /**
            * This method got called if a method this QObject instance
            * defines should be invoked.
            */
            int qt_metacall(QMetaObject::Call _c, int _id, void **_a)
            {
                _id = QObject::qt_metacall(_c, _id, _a);
                //krossdebug(QString("PythonFunction::qt_metacall id=%1").arg(_id));
                if(_id >= 0 && _c == QMetaObject::InvokeMetaMethod) {
                    switch(_id) {
                        case 0: {
                            // convert the arguments
                            QMetaMethod method = metaObject()->method( metaObject()->indexOfMethod(m_signature) );
                            QList<QByteArray> params = method.parameterTypes();
                            Py::Tuple args( params.size() );
                            int idx = 0;
                            foreach(QByteArray param, params) {
                                ++idx;
                                int tp = QVariant::nameToType( param.constData() );
                                switch(tp) {
                                    case QVariant::Invalid: // fall through
                                    case QVariant::UserType: {
                                        tp = QMetaType::type( param.constData() );
                                        #ifdef KROSS_PYTHON_FUNCTION_DEBUG
                                            krossdebug( QString("PythonFunction::qt_metacall: param=%1 metatypeId=%2").arg(param.constData()).arg(tp) );
                                        #endif
                                        switch( tp ) {
                                            case QMetaType::QObjectStar: {
                                                QObject* obj = (*reinterpret_cast< QObject*(*)>( _a[idx] ));
                                                //args[idx-1] = Py::asObject(new PythonExtension(obj, false));
                                                args[idx-1] = Py::asObject(new PythonExtension(obj));
                                                continue;
                                            } break;
                                            case QMetaType::QWidgetStar: {
                                                QWidget* obj = (*reinterpret_cast< QWidget*(*)>( _a[idx] ));
                                                //args[idx-1] = Py::asObject(new PythonExtension(obj, false));
                                                args[idx-1] = Py::asObject(new PythonExtension(obj));
                                                continue;
                                            } break;
                                            default:
                                                break;
                                        }
                                    } // fall through
                                    default: {
                                        QVariant v(tp, _a[idx]);

                                        if( ! Kross::Manager::self().strictTypesEnabled() ) {
                                            if( v.type() == QVariant::Invalid && QByteArray(param.constData()).endsWith("*") ) {
                                                QObject* obj = (*reinterpret_cast< QObject*(*)>( _a[idx] ));
                                                v.setValue( (QObject*) obj );
                                            }
                                        }

                                        #ifdef KROSS_PYTHON_FUNCTION_DEBUG
                                            krossdebug( QString("PythonFunction::qt_metacall argument param=%1 typeId=%2").arg(param.constData()).arg(tp) );
                                        #endif
                                        args[idx-1] = PythonType<QVariant>::toPyObject(v);
                                    } break;
                                }
                            }

                            Py::Object result;
                            try {
                                // call the python function
                                result = m_callable.apply(args);
                                // set the returnvalue
                                m_tmpResult = PythonType<QVariant>::toVariant(result);
                            }
                            catch(Py::Exception& e) {
                                QStringList trace;
                                int lineno;
                                PythonInterpreter::extractException(trace, lineno);
                                #ifdef KROSS_PYTHON_FUNCTION_DEBUG
                                    krosswarning( QString("PythonFunction::qt_metacall exception on line %1:\n%2 \n%3").arg(lineno).arg(Py::value(e).as_string().c_str()).arg(trace.join("\n")) );
                                #endif
                                PyErr_Print(); //e.clear();
                                return -1;
                            }

                            //_a[0] = Kross::MetaTypeVariant<QVariant>(d->tmpResult).toVoidStar();
                            _a[0] = &(m_tmpResult);
                        } break;
                    }
                    _id -= 1;
                }
                return _id;
            }

        private:
            Py::Callable m_callable;
            QVariant m_tmpResult;

            #ifdef KROSS_PYTHON_FUNCTION_DEBUG
                QString m_debuginfo;
            #endif
    };

}

#endif
