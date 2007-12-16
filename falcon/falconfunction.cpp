/***************************************************************************
 * falconfunction.cpp
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

#include "falconfunction.h"

namespace Kross {

    FalconFunction::FalconFunction(
                QObject* sender, 
                const QByteArray& signal, 
                const Falcon::Item& callable, 
                KFVM *vm ): 
        MetaFunction(sender, signal), 
        m_vm( vm )
    {
        // inhibits garbage collecting on the protected item.
        m_callLock = m_vm->gcLock( callable );
    }
    
    FalconFunction::~FalconFunction()
    {
        // tells the VM it can re-collect the item
        m_vm->gcUnlock( m_callLock );
    }

    int FalconFunction::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
    {
        _id = QObject::qt_metacall(_c, _id, _a);
        krossdebug(QString("FalconFunction::qt_metacall id=%1").arg(_id));
        
        if(_id >= 0 && _c == QMetaObject::InvokeMetaMethod) {
            switch(_id) {
                case 0: {
                    // convert the arguments
                    QMetaMethod method = metaObject()->method( metaObject()->indexOfMethod(m_signature) );
                    QList<QByteArray> params = method.parameterTypes();
                    
                    int idx = 1;
                    foreach(QByteArray param, params) {
                        int tp = QVariant::nameToType( param.constData() );
                        switch(tp) {
                            case QVariant::Invalid: // fall through
                            case QVariant::UserType: {
                                tp = QMetaType::type( param.constData() );
                                #ifdef KROSS_FALCON_FUNCTION_DEBUG
                                    krossdebug( QString("FalconFunction::qt_metacall: metatypeId=%1").arg(tp) );
                                #endif
                                switch( tp ) {
                                    case QMetaType::QWidgetStar:
                                    case QMetaType::QObjectStar: {
                                        QObject* obj = (*reinterpret_cast< QObject*(*)>( _a[idx] ));
                                        m_vm->pushParameter( m_vm->QObjectToKrossObject( obj ) );
                                    } break;
                                    default: {
                                        m_vm->pushParameter( Falcon::Item() );
                                    } break;
                                }
                            } break;
                            default: {
                                QVariant v(tp, _a[idx]);
                                #ifdef KROSS_FALCON_FUNCTION_DEBUG
                                    krossdebug( QString("FalconFunction::qt_metacall argument param=%1 typeId=%2").arg(param.constData()).arg(tp) );
                                #endif
                                Falcon::Item temp;
                                m_vm->variantToItem( v, temp );
                                m_vm->pushParameter( temp );
                            } break;
                        }
                        ++idx;
                    }
    
                    // At the moment, this callback can be performed only from outside the VM
                    // as linkable signals are only in QObjects coming from the applications.
                    // so, it's correct to use callItem.
                    m_vm->callItem( m_callLock->item(), idx - 1 ); 
                    // set the return value
                    if ( m_vm->hadError() ) {
                        // Kross has alredy been notified about the error.
                        // so, let's just clear the state for the next time and return nothing.
                        m_tmpResult = QVariant();
                        m_vm->resetEvent();
                    }
                    else {
                        m_vm->itemToVariant( m_vm->regA(), m_tmpResult );
                    }
                    _a[0] = &(m_tmpResult);
                } break;
            }
            _id -= 1;
        }
        return _id;
    }
}
