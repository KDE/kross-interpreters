/***************************************************************************
 * falconqobject.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2006 by Giancarlo Niccolai (jonnymind@falconpl.org)
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

#include "falconqobject.h"
#include "falconkfvm.h"
#include "falconkerror.h"
#include <QtGlobal>
#include <QObject>
#include <falcon/item.h>
#include <falcon/vm.h>
#include <falcon/error.h>


namespace Kross 
{
    /****************************************************
    QObject carrier for falcon QOBject class
    *****************************************************/
    
    FQOCarrier_Sensitive::FQOCarrier_Sensitive( FQOCarrier *owner ):
        m_owner( owner )
    {
        QObject::connect( owner->m_carried, SIGNAL( destroyed() ), 
                          this, SLOT( onCarriedGone() ) );
    }


    void  FQOCarrier_Sensitive::onCarriedGone()
    {
        m_owner->m_carried = 0;
    }


    FQOCarrier::FQOCarrier( QObject *data ):
        m_carried( data ),
        m_sensible( this )
    {
    }


    QObject *FQOCarrier::qobject() const
    {
        return m_carried;
    }


    QObject *FQOCarrier::qobject( Falcon::VMachine *vm ) const
    {
        if ( m_carried == 0 )
        {
            Falcon::String tstr;
            tstr.fromUTF8( KF_OBJECT_GONE_ERROR_DESC.toUtf8().data() );
            
            // raising an error from "runtime" origin because scripts see us as runtime
            vm->raiseModError( new FalconKrossError( 
                    Falcon::ErrorParam( KF_OBJECT_GONE_ERROR, __LINE__ ).
                    desc( tstr ).origin( Falcon::e_orig_runtime ) )
                    );
        }
        
        return m_carried;
    }


    void FQOCarrier::qobject( QObject *qo )
    {
        if ( m_carried != 0 )
        {
            m_sensible.disconnect();
        }
        
        // far from being threadsafe...
        m_carried = qo;
        QObject::connect( qo, SIGNAL( destroyed() ),
                          &m_sensible, SLOT( onCarriedGone() ) );
    }
    
    
    /****************************************************
    Falcon QObject class
    *****************************************************/

    void _falcon_qobject_init( Falcon::VMachine *vm )
    {
        // self is in init
        Falcon::CoreObject *self = vm->self().asObject();
        Falcon::Item *i_parent = vm->param( 0 );
        QObject *parent;
        
        if ( i_parent != 0 )
        {
            if ( i_parent->isObject() && i_parent->asObject()->derivedFrom( "QObject" ) )
            {
                Falcon::CoreObject *o_parent = i_parent->asObject();
                parent = reinterpret_cast< QObject *>(o_parent->getUserData());
            }
            else {
                // raising an error from "runtime" origin because scripts see us as runtime
                vm->raiseModError( new Falcon::ParamError( 
                    Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                    origin( Falcon::e_orig_runtime ) ) );
                parent = 0;
                // don't return; allow the creation of an empty object
            }
        }
        else 
        {
            parent = 0;
        }

        self->setUserData( new FQOCarrier( parent ) );
    }


    void _falcon_qobject_blockSignals( Falcon::VMachine *vm ) 
    {
        Falcon::Item *i_block = vm->param( 0 );
        if( i_block == 0 )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).origin( Falcon::e_orig_runtime ) ) );
            return;
        }
        
        Falcon::CoreObject *self = vm->self().asObject();
        QObject *obj = static_cast< FQOCarrier *>( self->getUserData() )->qobject( vm );
        
        if ( obj != 0 )
        {
            obj->blockSignals( i_block->isTrue() );
        }
    }


    void _falcon_qobject_connect( Falcon::VMachine *vm );
    void _falcon_qobject_disconnect( Falcon::VMachine *vm );
    void _falcon_qobject_dumpObjectInfo( Falcon::VMachine *vm );
    void _falcon_qobject_dumpObjectTree( Falcon::VMachine *vm );
    void _falcon_qobject_findChild( Falcon::VMachine *vm );
    void _falcon_qobject_findChildren ( Falcon::VMachine *vm );
    void _falcon_qobject_inherits ( Falcon::VMachine *vm );
    void _falcon_qobject_installEventFilter ( Falcon::VMachine *vm );
    void _falcon_qobject_isWidgetType ( Falcon::VMachine *vm );
    void _falcon_qobject_killTimer ( Falcon::VMachine *vm );
    void _falcon_qobject_killTimer ( Falcon::VMachine *vm );
    void _falcon_qobject_objectName ( Falcon::VMachine *vm );
    void _falcon_qobject_parent ( Falcon::VMachine *vm );
    void _falcon_qobject_property ( Falcon::VMachine *vm );
    void _falcon_qobject_removeEventFilter ( Falcon::VMachine *vm );
    void _falcon_qobject_setObjectName ( Falcon::VMachine *vm );
    void _falcon_qobject_setParent ( Falcon::VMachine *vm );
    void _falcon_qobject_setProperty( Falcon::VMachine *vm );
    void _falcon_qobject_signalsBlocked ( Falcon::VMachine *vm );
    void _falcon_qobject_startTimer ( Falcon::VMachine *vm );
}
