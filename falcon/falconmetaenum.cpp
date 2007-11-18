/***************************************************************************
 * falconmetaenum.cpp
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

#include "falconmetaenum.h"
#include <falcon/vm.h>
#include <falcon/autocstring.h>

namespace Kross
{

    FalconMetaEnumData::FalconMetaEnumData( const QMetaEnum &base ):
        m_metaEnum( base )
    {
    }
        
    Falcon::UserData *FalconMetaEnumData::clone()
    {
        return new FalconMetaEnumData( m_metaEnum );
    }


    static void _static_isFlag( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaEnumData *fdata = static_cast<FalconMetaEnumData *>( self->getUserData() );
        
        vm->retval( (Falcon::int64) (fdata->metaEnum().isFlag() ? 1 : 0 ) );
    }
    
    
    static void _static_isValid( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaEnumData *fdata = static_cast<FalconMetaEnumData *>( self->getUserData() );
        
        vm->retval( (Falcon::int64) (fdata->metaEnum().isValid() ? 1 : 0 ) );
    }
    
    
    static void _static_key( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaEnumData *fdata = static_cast<FalconMetaEnumData *>( self->getUserData() );
        Falcon::Item *i_keyId = vm->param(0);
        if ( i_keyId == 0 || !i_keyId->isOrdinal() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "N" ) ) );
            return;
        }

        const char *key = fdata->metaEnum().key( i_keyId->forceInteger() );
        if ( key == 0 )
            vm->retnil();
        else
            vm->retval( new Falcon::GarbageString( vm, key ) );
    }
    
    
    static void _static_keyCount( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaEnumData *fdata = static_cast<FalconMetaEnumData *>( self->getUserData() );
        vm->retval( fdata->metaEnum().keyCount() );
    }
    
    
    static void _static_keyToValue( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaEnumData *fdata = static_cast<FalconMetaEnumData *>( self->getUserData() );
        Falcon::Item *i_key = vm->param(0);
        if ( i_key == 0 || !i_key->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "S" ) ) );
            return;
        }

        Falcon::AutoCString skey( *i_key->asString() );
        vm->retval( (Falcon::int64) fdata->metaEnum().keyToValue( skey.c_str() ));
    }
    
    
    static void _static_keysToValue( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaEnumData *fdata = static_cast<FalconMetaEnumData *>( self->getUserData() );
        Falcon::Item *i_key = vm->param(0);
        if ( i_key == 0 || !i_key->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "S" ) ) );
            return;
        }

        Falcon::AutoCString skey( *i_key->asString() );
        vm->retval( (Falcon::int64) fdata->metaEnum().keysToValue( skey.c_str()  ));
    }
    
    
    static void _static_name( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaEnumData *fdata = static_cast<FalconMetaEnumData *>( self->getUserData() );
        vm->retval( new Falcon::GarbageString( vm, fdata->metaEnum().name() ) );
    }
    
    
    static void _static_scope( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaEnumData *fdata = static_cast<FalconMetaEnumData *>( self->getUserData() );
        vm->retval( new Falcon::GarbageString( vm, fdata->metaEnum().scope() ) );
    }
    
    
    static void _static_value( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaEnumData *fdata = static_cast<FalconMetaEnumData *>( self->getUserData() );
        Falcon::Item *i_keyId = vm->param(0);
        if ( i_keyId == 0 || !i_keyId->isOrdinal() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "N" ) ) );
            return;
        }

        vm->retval( (Falcon::int64) fdata->metaEnum().value( i_keyId->forceInteger() ));
    }
    
    
    static void _static_valueToKey( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaEnumData *fdata = static_cast<FalconMetaEnumData *>( self->getUserData() );
        Falcon::Item *i_keyId = vm->param(0);
        if ( i_keyId == 0 || !i_keyId->isOrdinal() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "N" ) ) );
            return;
        }
        
        const char *key = fdata->metaEnum().valueToKey( i_keyId->forceInteger() );
        if ( key == 0 )
            vm->retnil();
        else
            vm->retval( new Falcon::GarbageString( vm, key ) );
    }
    
    
    static void _static_valueToKeys( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaEnumData *fdata = static_cast<FalconMetaEnumData *>( self->getUserData() );
        Falcon::Item *i_keyId = vm->param(0);
        if ( i_keyId == 0 || !i_keyId->isOrdinal() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "N" ) ) );
            return;
        }
        
        QByteArray keys = fdata->metaEnum().valueToKeys( i_keyId->forceInteger() );
        // we need a localstorage, as keys will be destroyed
        Falcon::String *strRet = new Falcon::GarbageString( vm, (const char *) keys );
        strRet->bufferize();
        vm->retval( strRet );
    }


    void DeclareFalconMetaEnum( Falcon::Module *self )
    {
        Falcon::Symbol *qme_class = self->addClass( "QMetaEnum" );
        // make this symbol unexported, that is, unavailable for instantation to scripts
        qme_class->exported( false );
        
        self->addClassMethod( qme_class, "isFlag", _static_isFlag );
        self->addClassMethod( qme_class, "isValid", _static_isValid );
        self->addClassMethod( qme_class, "key", _static_key );
        self->addClassMethod( qme_class, "keyCount", _static_keyCount );
        self->addClassMethod( qme_class, "keyToValue", _static_keyToValue );
        self->addClassMethod( qme_class, "keysToValue", _static_keysToValue );
        self->addClassMethod( qme_class, "name", _static_name );
        self->addClassMethod( qme_class, "scope", _static_scope );
        self->addClassMethod( qme_class, "value", _static_value );
        self->addClassMethod( qme_class, "valueToKey", _static_valueToKey );
        self->addClassMethod( qme_class, "valueToKeys", _static_valueToKeys );
    }
    
}
