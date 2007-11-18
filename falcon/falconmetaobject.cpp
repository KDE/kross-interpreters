/***************************************************************************
 * falconmetaobject.cpp
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

#include <falcon/autocstring.h>
#include <QVariant>

#include "falconkfvm.h"
#include "falconqobject.h"
#include "falconmetaobject.h"

namespace Kross
{
    FalconMetaObjectData::FalconMetaObjectData( const QMetaObject *base ):
        m_metaObject( base )
    {
    }
    
    Falcon::UserData *FalconMetaObjectData::clone()
    {
        
        return new FalconMetaObjectData( m_metaObject );
    }


    static bool _internal_get_object_param( Falcon::VMachine *vm, QObject **param )
    {
        *param = 0;
        
        Falcon::Item *i_obj = vm->param(0);
        if ( i_obj == 0 || !i_obj->isObject() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "O" ) ) );
            return false;
        }
        
        Falcon::CoreObject *obj = i_obj->asObject();
        if ( ! obj->derivedFrom( "QObject" ) )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_param_type, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "not a QObject" ) ) );
            return false;
        }
        
        // ok, get the qobject we're carrying
        *param = static_cast< FQOCarrier *>( obj->getUserData() )->qobject( vm );
        return param != 0;
    }

    static void _static_classInfo( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        Falcon::Item *i_keyId = vm->param(0);
        if ( i_keyId == 0 || !i_keyId->isOrdinal() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "N" ) ) );
            return;
        }
        
        KFVM *kfvm = static_cast<KFVM *>( vm );
        Falcon::CoreObject *cinfo = kfvm->MetaClassInfoToObject( 
            fdata->metaObject().classInfo( i_keyId->forceInteger() ) );
            
        vm->retval( cinfo );
    }
    
    static void _static_classInfoCount( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
       
        vm->retval( (Falcon::int64) fdata->metaObject().classInfoCount() );
    }
    
    
    static void _static_classInfoOffset( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
       
        vm->retval( (Falcon::int64) fdata->metaObject().classInfoOffset() );
    }
    
    
    static void _static_className( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
       
        vm->retval( fdata->metaObject().className() );
    }
    
    
    static void _static_enumerator( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        Falcon::Item *i_keyId = vm->param(0);
        if ( i_keyId == 0 || !i_keyId->isOrdinal() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "N" ) ) );
            return;
        }
        
        KFVM *kfvm = static_cast<KFVM *>( vm );
        Falcon::CoreObject *cinfo = kfvm->MetaEnumToObject( 
            fdata->metaObject().enumerator( i_keyId->forceInteger() ) );
            
        vm->retval( cinfo );
    }
    
    
    static void _static_enumeratorCount( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
       
        vm->retval( (Falcon::int64) fdata->metaObject().enumeratorCount() );
    }
    
    
    static void _static_enumeratorOffset( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
       
        vm->retval( (Falcon::int64) fdata->metaObject().enumeratorOffset() );
    }
    
    
    static void _static_indexOfClassInfo( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        Falcon::Item *i_keyName = vm->param(0);
        if ( i_keyName == 0 || !i_keyName->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "S" ) ) );
            return;
        }
        
        Falcon::AutoCString keyName( *i_keyName->asString() );
        vm->retval( (Falcon::int64) fdata->metaObject().indexOfClassInfo( keyName.c_str() ) );
    }
    
    
    static void _static_indexOfEnumerator( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        Falcon::Item *i_keyName = vm->param(0);
        if ( i_keyName == 0 || !i_keyName->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "S" ) ) );
            return;
        }
        
        Falcon::AutoCString keyName( *i_keyName->asString() );
        vm->retval( (Falcon::int64) fdata->metaObject().indexOfEnumerator( keyName.c_str() ) );
    }
    
    
    static void _static_indexOfMethod( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        Falcon::Item *i_keyName = vm->param(0);
        if ( i_keyName == 0 || !i_keyName->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "S" ) ) );
            return;
        }
        
        Falcon::AutoCString keyName( *i_keyName->asString() );
        vm->retval( (Falcon::int64) fdata->metaObject().indexOfMethod( keyName.c_str() ) );
    }
    
    
    static void _static_indexOfProperty( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        Falcon::Item *i_keyName = vm->param(0);
        if ( i_keyName == 0 || !i_keyName->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "S" ) ) );
            return;
        }
        
        Falcon::AutoCString keyName( *i_keyName->asString() );
        vm->retval( (Falcon::int64) fdata->metaObject().indexOfMethod( keyName.c_str() ) );
    }
    
    
    static void _static_indexOfSignal( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        Falcon::Item *i_keyName = vm->param(0);
        if ( i_keyName == 0 || !i_keyName->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "S" ) ) );
            return;
        }
        
        Falcon::AutoCString keyName( *i_keyName->asString() );
        vm->retval( (Falcon::int64) fdata->metaObject().indexOfSignal( keyName.c_str() ) );
    }
    
    
    static void _static_indexOfSlot( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        Falcon::Item *i_keyName = vm->param(0);
        if ( i_keyName == 0 || !i_keyName->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "S" ) ) );
            return;
        }
        
        Falcon::AutoCString keyName( *i_keyName->asString() );
        vm->retval( (Falcon::int64) fdata->metaObject().indexOfSlot( keyName.c_str() ) );
    }
    
    
    static void _static_method( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        Falcon::Item *i_keyId = vm->param(0);
        if ( i_keyId == 0 || !i_keyId->isOrdinal() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "N" ) ) );
            return;
        }
        
        KFVM *kfvm = static_cast<KFVM *>( vm );
        Falcon::CoreObject *cinfo = kfvm->MetaMethodToObject( 
            fdata->metaObject().method( i_keyId->forceInteger() ) );
            
        vm->retval( cinfo );
    }
    
    
    static void _static_methodCount( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
       
        vm->retval( (Falcon::int64) fdata->metaObject().methodCount() );
    }
    
    
    static void _static_methodOffset( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
       
        vm->retval( (Falcon::int64) fdata->metaObject().methodOffset() );
    }
    
    
    static void _static_property( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        Falcon::Item *i_keyId = vm->param(0);
        if ( i_keyId == 0 || !i_keyId->isOrdinal() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "N" ) ) );
            return;
        }
        
        KFVM *kfvm = static_cast<KFVM *>( vm );
        Falcon::CoreObject *cinfo = kfvm->MetaPropertyToObject( 
            fdata->metaObject().property( i_keyId->forceInteger() ) );
            
        vm->retval( cinfo );
    }
    
    
    static void _static_propertyCount( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
       
        vm->retval( (Falcon::int64) fdata->metaObject().propertyCount() );
    }
    
    
    static void _static_propertyOffset( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
       
        vm->retval( (Falcon::int64) fdata->metaObject().propertyOffset() );
    }
    
    
    static void _static_superClass( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        
        const QMetaObject *superclass = fdata->metaObject().superClass();
        if ( superclass == 0 )
        {
            vm->retnil();
        }
        else
        {
            KFVM *kfvm = static_cast<KFVM *>( vm );
            Falcon::CoreObject *cinfo = kfvm->MetaObjectToObject( superclass );
            vm->retval( cinfo );
        }
    }
    
    
    static void _static_userProperty( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaObjectData *fdata = static_cast<FalconMetaObjectData *>( self->getUserData() );
        KFVM *kfvm = static_cast<KFVM *>( vm );
        Falcon::CoreObject *cinfo = kfvm->MetaPropertyToObject( fdata->metaObject().userProperty() );
        vm->retval( cinfo );
    }
    

    // Static interface
    
    static void _static_mo_checkConnectArgs( Falcon::VMachine *vm )
    {
        Falcon::Item *i_signal = vm->param( 0 );
        Falcon::Item *i_method = vm->param( 1 );
        
        if( i_signal == 0 || ! i_signal->isString() ||
            i_method == 0 || ! i_method->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "S,S" ) ) );
                
            return;
        }
        
        Falcon::AutoCString sSignal( i_signal->asString() );
        Falcon::AutoCString sMethod( i_method->asString() );
        
        vm->retval( (Falcon::int64) ( QMetaObject::checkConnectArgs( sSignal.c_str(), sMethod.c_str() ) ? 1: 0) );
    }
    
    static void _static_mo_connectSlotsByName( Falcon::VMachine *vm )
    {
        // Decode mandatory parameter of type QObject
        QObject *param;
        if ( ! _internal_get_object_param( vm, &param ) )
        {
            return;
        }
        
        QMetaObject::connectSlotsByName( param );
    }
    
    
    static void _static_mo_invokeMethod( Falcon::VMachine *vm )
    {
    
    }
    
    
    static void _static_mo_normalizedSignature( Falcon::VMachine *vm )
    {
        Falcon::Item *i_method = vm->param( 0 );
        
        if( i_method == 0 || ! i_method->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "S" ) ) );
            return;
        }
        
        Falcon::AutoCString sMethod( i_method->asString() );
        QByteArray bName = QMetaObject::normalizedSignature( sMethod.c_str() );
        Falcon::String *sName = new Falcon::GarbageString( vm, (const char *) bName );
        sName->bufferize();
        vm->retval( sName );
    }
    
    
    static void _static_mo_normalizedType( Falcon::VMachine *vm )
    {
        Falcon::Item *i_type = vm->param( 0 );
        
        if( i_type == 0 || ! i_type->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).extra( "S" ) ) );
            return;
        }
        
        Falcon::AutoCString sType( i_type->asString() );
        QByteArray bName = QMetaObject::normalizedType( sType.c_str() );
        Falcon::String *sName = new Falcon::GarbageString( vm, (const char *) bName );
        sName->bufferize();
        vm->retval( sName );
    }
    

    void DeclareFalconMetaObject( Falcon::Module *self )
    {
        Falcon::Symbol *qmo_class = self->addClass( "QMetaObject" );
        // make this symbol unexported, that is, unavailable for instantation to scripts
        qmo_class->exported( false );
        
        self->addClassMethod( qmo_class, "classInfo", _static_classInfo );
        self->addClassMethod( qmo_class, "classInfoCount", _static_classInfoCount );
        self->addClassMethod( qmo_class, "classInfoOffset", _static_classInfoOffset );
        self->addClassMethod( qmo_class, "className", _static_className );
        self->addClassMethod( qmo_class, "enumerator", _static_enumerator );
        self->addClassMethod( qmo_class, "enumeratorCount", _static_enumeratorCount );
        self->addClassMethod( qmo_class, "enumeratorOffset", _static_enumeratorOffset );
        self->addClassMethod( qmo_class, "indexOfClassInfo", _static_indexOfClassInfo );
        self->addClassMethod( qmo_class, "indexOfEnumerator", _static_indexOfEnumerator );
        self->addClassMethod( qmo_class, "indexOfMethod", _static_indexOfMethod );
        self->addClassMethod( qmo_class, "indexOfProperty", _static_indexOfProperty );
        self->addClassMethod( qmo_class, "indexOfSignal", _static_indexOfSignal );
        self->addClassMethod( qmo_class, "indexOfSlot", _static_indexOfSlot );
        self->addClassMethod( qmo_class, "method", _static_method );
        self->addClassMethod( qmo_class, "methodCount", _static_methodCount );
        self->addClassMethod( qmo_class, "methodOffset", _static_methodOffset );
        self->addClassMethod( qmo_class, "property", _static_property );
        self->addClassMethod( qmo_class, "propertyCount", _static_propertyCount );
        self->addClassMethod( qmo_class, "propertyOffset", _static_propertyOffset );
        self->addClassMethod( qmo_class, "superClass", _static_superClass );
        self->addClassMethod( qmo_class, "userProperty", _static_userProperty );
        
        // Static interface
        self->addExtFunc( "MO_checkConnectArgs", _static_mo_checkConnectArgs );
        self->addExtFunc( "MO_connectSlotsByName", _static_mo_connectSlotsByName );
        self->addExtFunc( "MO_invokeMethod", _static_mo_invokeMethod );
        self->addExtFunc( "MO_normalizedSignature", _static_mo_normalizedSignature );
        self->addExtFunc( "MO_normalizedType", _static_mo_normalizedType );
    }

}
