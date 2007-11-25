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

/*
    static void _falcon_qobject_connect( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_disconnect( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_dumpObjectInfo( Falcon::VMachine *vm )
    {
    }
    
    static void _falcon_qobject_dumpObjectTree( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_findChild( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_findChildren ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_inherits ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_installEventFilter ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_isWidgetType ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_killTimer ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_killTimer ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_objectName ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_parent ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_property ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_removeEventFilter ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_setObjectName ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_setParent ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_setProperty( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_signalsBlocked ( Falcon::VMachine *vm )
    {
    }
    
    
    static void _falcon_qobject_startTimer ( Falcon::VMachine *vm )
    {
    }

    static void _falcon_qobject_static_connect ( Falcon::VMachine *vm )
    {
    }
    
    static void _falcon_qobject_static_disconnect ( Falcon::VMachine *vm )
    {
    }
    
    static void _falcon_qobject_static_staticMetaObject( Falcon::VMachine *vm )
    {
    }
    
    static void _falcon_qobject_static_tr( Falcon::VMachine *vm )
    {
    }
    
    static void _falcon_qobject_static_trUtf8( Falcon::VMachine *vm )
    {
    }


    
    void DeclareFalconQObject( Falcon::Module *self )
    {
        Falcon::Symbol *qo_class = self->addClass( "QObject", _falcon_qobject_init );
        // the symbol is exported, so scripts can create qobjects
        
        self->addClassMethod( qo_class, "blockSignals", _falcon_qobject_blockSignals );
        self->addClassMethod( qo_class, "connect", _falcon_qobject_connect );
        self->addClassMethod( qo_class, "disconnect", _falcon_qobject_disconnect );
        self->addClassMethod( qo_class, "dumpObjectInfo", _falcon_qobject_dumpObjectInfo );
        self->addClassMethod( qo_class, "dumpObjectTree", _falcon_qobject_dumpObjectTree );
        self->addClassMethod( qo_class, "findChild", _falcon_qobject_findChild );
        self->addClassMethod( qo_class, "findChildren", _falcon_qobject_findChildren );
        self->addClassMethod( qo_class, "inherits", _falcon_qobject_inherits );
        self->addClassMethod( qo_class, "installEventFilter", _falcon_qobject_installEventFilter );
        self->addClassMethod( qo_class, "isWidgetType", _falcon_qobject_isWidgetType );
        self->addClassMethod( qo_class, "killTimer", _falcon_qobject_killTimer );
        self->addClassMethod( qo_class, "objectName", _falcon_qobject_objectName );
        self->addClassMethod( qo_class, "parent", _falcon_qobject_parent );
        self->addClassMethod( qo_class, "property", _falcon_qobject_property );
        self->addClassMethod( qo_class, "removeEventFilter", _falcon_qobject_removeEventFilter );
        self->addClassMethod( qo_class, "setObjectName", _falcon_qobject_setObjectName );
        self->addClassMethod( qo_class, "setParent", _falcon_qobject_setParent );
        self->addClassMethod( qo_class, "setProperty", _falcon_qobject_setProperty );
        self->addClassMethod( qo_class, "signalsBlocked", _falcon_qobject_signalsBlocked );
        self->addClassMethod( qo_class, "startTimer", _falcon_qobject_startTimer );
        
        
        self->addConstant( "QOBJECT_", (Falcon::int64) QMetaMethod::Method );
        self->addConstant( "QMT_SLOT", (Falcon::int64) QMetaMethod::Slot );
        self->addConstant( "QMT_SIGNAL", (Falcon::int64) QMetaMethod::Signal );
    }
*/

    /*
        code formerly used in falcon kvm
    Falcon::CoreObject *KFVM::MetaClassInfoToObject( const QMetaClassInfo &qmi )
    {
        Falcon::Item *i_qmi = findGlobalItem( "QMetaClassInfo" );
        Q_ASSERT( i_qmi != 0 );
        // if it's a class, then we can create an instance from that
        if ( i_qmi->isClass() )
        {
            Falcon::CoreObject *instance = i_qmi->asClass()->createInstance();
            
            Falcon::String *sName = new Falcon::GarbageString( this, qmi.name() );
            instance->setProperty( "name", sName );
            
            Falcon::String *sValue = new Falcon::GarbageString( this, qmi.value() );
            instance->setProperty( "value", sValue );
            return instance;
        }
        
        return 0;
    }
    
    
    Falcon::CoreObject *KFVM::MetaEnumToObject( const QMetaEnum &qmi )
    {
        Falcon::Item *i_qme = findGlobalItem( "QMetaEnum" );
        Q_ASSERT( i_qme != 0 );
        // if it's a class, then we can create an instance from that
        if ( i_qme->isClass() )
        {
            Falcon::CoreObject *instance = i_qme->asClass()->createInstance();
            instance->setUserData( new FalconMetaEnumData( qmi ) );
            return instance;
        }
        
        return 0;
    }
    
    
    Falcon::CoreObject *KFVM::MetaMethodToObject( const QMetaMethod &qmi )
    {
        Falcon::Item *i_qme = findGlobalItem( "QMetaMethod" );
        Q_ASSERT( i_qme != 0 );
        // if it's a class, then we can create an instance from that
        if ( i_qme->isClass() )
        {
            Falcon::CoreObject *instance = i_qme->asClass()->createInstance();
            instance->setUserData( new FalconMetaMethodData( qmi ) );
            return instance;
        }
        
        return 0;
    }
    
    
    Falcon::CoreObject *KFVM::MetaPropertyToObject( const QMetaProperty &qmp)
    {
        Falcon::Item *i_qmp = findGlobalItem( "QMetaProperty" );
        Q_ASSERT( i_qmp != 0 );
        // if it's a class, then we can create an instance from that
        if ( i_qmp->isClass() )
        {
            Falcon::CoreObject *instance = i_qmp->asClass()->createInstance();
            instance->setUserData( new FalconMetaPropertyData( qmp ) );
            return instance;
        }
        
        return 0;
    }
    
    Falcon::CoreObject *KFVM::MetaObjectToObject( const QMetaObject *qmo )
    {
        Falcon::Item *i_qmo = findGlobalItem( "QMetaObject" );
        Q_ASSERT( i_qmo != 0 );
        // if it's a class, then we can create an instance from that
        if ( i_qmo->isClass() )
        {
            Falcon::CoreObject *instance = i_qmo->asClass()->createInstance();
            instance->setUserData( new FalconMetaObjectData( qmo ) );
            return instance;
        }
        
        return 0;
    }
    */
}
