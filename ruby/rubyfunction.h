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
            RubyFunction(QObject* sender, const QByteArray& signal, const VALUE& method)
                : MetaFunction(sender, signal), m_method(method) {}

            /**
            * Destructor.
            */
            virtual ~RubyFunction() {}

            /**
            * This method got called if a method this QObject instance
            * defines should be invoked.
            */
            int qt_metacall(QMetaObject::Call _c, int _id, void **_a)
            {
                _id = QObject::qt_metacall(_c, _id, _a);
                //krossdebug(QString("RubyFunction::qt_metacall id=%1").arg(_id));
                if(_id >= 0 && _c == QMetaObject::InvokeMetaMethod) {
                    switch(_id) {
                        case 0: {
                            // convert the arguments
                            QMetaMethod method = metaObject()->method( metaObject()->indexOfMethod(m_signature) );
                            QList<QByteArray> params = method.parameterTypes();
                            const int argsize = params.size();
                            VALUE* args = new VALUE[argsize];
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
                                                args[idx-1] = RubyExtension::toVALUE( new RubyExtension(obj) );
                                            } break;
                                            case QMetaType::QWidgetStar: {
                                                QWidget* obj = (*reinterpret_cast< QWidget*(*)>( _a[idx] ));
                                                args[idx-1] = RubyExtension::toVALUE( new RubyExtension(obj) );
                                            } break;
                                            default: {
                                                args[idx-1] = Qnil;
                                            } break;
                                        }
                                    } break;
                                    default: {
                                        QVariant v(tp, _a[idx]);
                                        #ifdef KROSS_RUBY_FUNCTION_DEBUG
                                            krossdebug( QString("RubyFunction::qt_metacall argument param=%1 typeId=%2").arg(param.constData()).arg(tp) );
                                        #endif
                                        args[idx-1] = RubyType<QVariant>::toVALUE(v);
                                    } break;
                                }
                                ++idx;
                            }

                            // call the ruby function
                            //TODO use rb_rescue2 to catch exceptions?
                            VALUE result = rb_funcall2(m_method, rb_intern("call"), argsize, args);

                            // finally set the returnvalue
                            m_tmpResult = RubyType<QVariant>::toVariant(result);
                            #ifdef KROSS_RUBY_FUNCTION_DEBUG
                                QObject* sender = QObject::sender();
                                krossdebug( QString("RubyFunction::qt_metacall sender.objectName=%1 sender.className=%2 result=%3 variantresult=%4").arg(sender->objectName()).arg(sender->metaObject()->className()).arg(STR2CSTR(rb_inspect(result))).arg(m_tmpResult.toString()) );
                            #endif
                            //_a[0] = Kross::MetaTypeVariant<QVariant>(d->tmpResult).toVoidStar();
                            _a[0] = &(m_tmpResult);

                            delete[] args;
                        } break;
                    }
                    _id -= 1;
                }
                return _id;
            }

        private:
            VALUE m_method;
            QVariant m_tmpResult;
    };

}

#endif
