/***************************************************************************
 * falconmetamethod.cpp
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

#include "falconmetamethod.h"
#include <falcon/vm.h>
#include <falcon/autocstring.h>



namespace Kross
{
    FalconMetaMethodData::FalconMetaMethodData( const QMetaMethod &base ):
        m_metaMethod( base )
    {
    }
        
    Falcon::UserData *FalconMetaMethodData::clone()
    {
        return new FalconMetaMethodData( m_metaMethod );
    }
    

    static void _static_access( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaMethodData *fdata = static_cast<FalconMetaMethodData *>( self->getUserData() );
        
        vm->retval( (Falcon::int64) fdata->metaMethod().access() );
    }
    
    
    static void _static_methodType( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaMethodData *fdata = static_cast<FalconMetaMethodData *>( self->getUserData() );
        
        vm->retval( (Falcon::int64) fdata->metaMethod().methodType() );
    }
    
    
    static void _static_parameterNames( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaMethodData *fdata = static_cast<FalconMetaMethodData *>( self->getUserData() );
        
        QList<QByteArray> parList =  fdata->metaMethod().parameterNames();
        
        Falcon::CoreArray *array = new Falcon::CoreArray( vm, parList.size() );
        
        QList<QByteArray>::const_iterator pli = parList.begin();
        while( pli != parList.end() )
        {
            Falcon::String *param = new Falcon::GarbageString( vm, (const char *) *pli );
            param->bufferize();
            array->append( param );
            ++pli;
        }
        
        vm->retval( array );
    }
    
    
    static void _static_parameterTypes( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaMethodData *fdata = static_cast<FalconMetaMethodData *>( self->getUserData() );
        
        QList<QByteArray> parList =  fdata->metaMethod().parameterTypes();
        
        Falcon::CoreArray *array = new Falcon::CoreArray( vm, parList.size() );
        
        QList<QByteArray>::const_iterator pli = parList.begin();
        while( pli != parList.end() )
        {
            Falcon::String *param = new Falcon::GarbageString( vm, (const char *) *pli );
            param->bufferize();
            array->append( param );
            ++pli;
        }
        
        vm->retval( array );
    }
    
    
    static void _static_signature( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaMethodData *fdata = static_cast<FalconMetaMethodData *>( self->getUserData() );
        
        vm->retval( fdata->metaMethod().signature() );
    }
    
    
    static void _static_tag( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaMethodData *fdata = static_cast<FalconMetaMethodData *>( self->getUserData() );
        
        vm->retval( fdata->metaMethod().tag() );
    }
    
    
    static void _static_typeName( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaMethodData *fdata = static_cast<FalconMetaMethodData *>( self->getUserData() );
        
        vm->retval( fdata->metaMethod().typeName() );
    }
    

    void DeclareFalconMetaMethod( Falcon::Module *self )
    {
        Falcon::Symbol *qme_class = self->addClass( "QMetaMethod" );
        // make this symbol unexported, that is, unavailable for instantation to scripts
        qme_class->exported( false );
        
        self->addClassMethod( qme_class, "access", _static_access );
        self->addClassMethod( qme_class, "methodType", _static_methodType );
        self->addClassMethod( qme_class, "parameterNames", _static_parameterNames );
        self->addClassMethod( qme_class, "parameterTypes", _static_parameterTypes );
        self->addClassMethod( qme_class, "signature", _static_signature );
        self->addClassMethod( qme_class, "tag", _static_tag );
        self->addClassMethod( qme_class, "typeName", _static_typeName );
        
        self->addConstant( "QMT_METHOD", (Falcon::int64) QMetaMethod::Method );
        self->addConstant( "QMT_SLOT", (Falcon::int64) QMetaMethod::Slot );
        self->addConstant( "QMT_SIGNAL", (Falcon::int64) QMetaMethod::Signal );
    }
}
