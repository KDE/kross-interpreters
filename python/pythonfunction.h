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
                : MetaFunction(sender, signal), m_callable(callable) {}

            /**
            * Destructor.
            */
            virtual ~PythonFunction() {}

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
                            int idx = 1;
                            foreach(QByteArray param, params) {
                                int tp = QVariant::nameToType( param.constData() );
                                switch(tp) {
                                    case QVariant::Invalid: // fall through
                                    case QVariant::UserType: {
                                        tp = QMetaType::type( param.constData() );
                                        #ifdef KROSS_PYTHON_FUNCTION_DEBUG
                                            krossdebug( QString("PythonFunction::qt_metacall: metatypeId=%1").arg(tp) );
                                        #endif
                                        switch( tp ) {
                                            case QMetaType::QObjectStar: {
                                                QObject* obj = (*reinterpret_cast< QObject*(*)>( _a[idx] ));
                                                //args[idx-1] = Py::asObject(new PythonExtension(obj, false));
                                                args[idx-1] = Py::asObject(new PythonExtension(obj));
                                            } break;
                                            case QMetaType::QWidgetStar: {
                                                QWidget* obj = (*reinterpret_cast< QWidget*(*)>( _a[idx] ));
                                                //args[idx-1] = Py::asObject(new PythonExtension(obj, false));
                                                args[idx-1] = Py::asObject(new PythonExtension(obj));
                                            } break;
                                            default: {
                                                args[idx-1] = Py::None();
                                            } break;
                                        }
                                    } break;
                                    default: {
                                        QVariant v(tp, _a[idx]);
                                        #ifdef KROSS_PYTHON_FUNCTION_DEBUG
                                            krossdebug( QString("PythonFunction::qt_metacall argument param=%1 typeId=%2").arg(param.constData()).arg(tp) );
                                        #endif
                                        args[idx-1] = PythonType<QVariant>::toPyObject(v);
                                    } break;
                                }
                                ++idx;
                            }

                            Py::Object result;
                            try {
                                // call the python function
                                result = m_callable.apply(args);
                            }
                            catch(Py::Exception& e) {
                                QStringList trace;
                                int lineno;
                                PythonInterpreter::extractException(trace, lineno);
                                krosswarning( QString("PythonFunction::qt_metacall exception on line %1:\n%2 \n%3").arg(lineno).arg(Py::value(e).as_string().c_str()).arg(trace.join("\n")) );
                                PyErr_Print(); //e.clear();
                                return -1;
                            }

                            // finally set the returnvalue
                            m_tmpResult = PythonType<QVariant>::toVariant(result);
                            #ifdef KROSS_PYTHON_FUNCTION_DEBUG
                                QObject* sender = QObject::sender();
                                krossdebug( QString("PythonFunction::qt_metacall sender.objectName=%1 sender.className=%2 pyresult=%3 variantresult=%4").arg(sender->objectName()).arg(sender->metaObject()->className()).arg(result.as_string().c_str()).arg(d->tmpResult.toString()) );
                            #endif
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
    };

}

#endif
