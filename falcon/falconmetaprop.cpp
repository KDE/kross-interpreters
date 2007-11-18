/***************************************************************************
 * falconmetaprop.cpp
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

#include "falconmetaprop.h"
#include "falconkfvm.h"
#include "falconqobject.h"
#include <falcon/autocstring.h>

#include <QVariant>

namespace Kross
{
    FalconMetaPropertyData::FalconMetaPropertyData( const QMetaProperty &base ):
        m_metaProp( base )
    {
    }
    
    Falcon::UserData *FalconMetaPropertyData::clone()
    {
        return new FalconMetaPropertyData( m_metaProp );
    }


    static bool _internal_get_object_param( Falcon::VMachine *vm, QObject **param )
    {
        *param = 0;
        
        Falcon::Item *i_obj = vm->param(0);
        if ( i_obj != 0 )
        {
            if ( !i_obj->isObject() )
            {
                vm->raiseModError( new Falcon::ParamError( 
                    Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                    origin( Falcon::e_orig_runtime ).
                    extra( "[O]" ) ) );
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
            if ( *param == 0 )
            {
               return false;
            }
        }
        
        return true;
    }

    static void _static_enumerator( Falcon::VMachine *vm )
    {
        KFVM *kfvm = static_cast< KFVM *>( vm );
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        Falcon::CoreObject *eobj = kfvm->MetaEnumToObject( fdata->metaProp().enumerator() );
        vm->retval( eobj );
    }
    
    
    static void _static_isDesignable( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        // Decode optional parameter of type QObject and ensure it is alive
        QObject *param;
        if( _internal_get_object_param( vm, &param ) )
        {
            vm->retval( (Falcon::int64) (fdata->metaProp().isDesignable( param ) ? 1: 0 ) );
        }
    }
    
    
    static void _static_isEnumType( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        vm->retval( (Falcon::int64) (fdata->metaProp().isEnumType() ? 1: 0 ) );
    }
    
    
    static void _static_isFlagType( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        vm->retval( (Falcon::int64) (fdata->metaProp().isFlagType() ? 1: 0 ) );
    }
    
    
    static void _static_isScriptable( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        // Decode optional parameter of type QObject and ensure it is alive
        QObject *param;
        if( _internal_get_object_param( vm, &param ) )
        {
            vm->retval( (Falcon::int64) (fdata->metaProp().isScriptable( param ) ? 1: 0 ) );
        }
    }
    
    
    static void _static_isStored( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        // Decode optional parameter of type QObject and ensure it is alive
        QObject *param;
        if( _internal_get_object_param( vm, &param ) )
        {
            vm->retval( (Falcon::int64) (fdata->metaProp().isStored( param ) ? 1: 0 ) );
        }
    }
    
    
    static void _static_isUser( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        // Decode optional parameter of type QObject and ensure it is alive
        QObject *param;
        if( _internal_get_object_param( vm, &param ) )
        {
            vm->retval( (Falcon::int64) (fdata->metaProp().isUser( param ) ? 1: 0 ) );
        }
    }
    
    static void _static_isValid( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        vm->retval( (Falcon::int64) (fdata->metaProp().isValid() ? 1: 0 ) );
    }
    
    static void _static_isWritable( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        vm->retval( (Falcon::int64) (fdata->metaProp().isValid() ? 1: 0 ) );
    }
    
    
    static void _static_name( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        vm->retval( fdata->metaProp().name() );
    }
    
    
    static void _static_read( Falcon::VMachine *vm )
    {
        KFVM *kfvm = static_cast< KFVM *>( vm );
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        QObject *param;
        if( _internal_get_object_param( vm, &param ) )
        {
            // the parameter is not optional for this member
            if ( param == 0 )
            {
                vm->raiseModError( new Falcon::ParamError( 
                    Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                    origin( Falcon::e_orig_runtime ).
                    extra( "O" ) ) );
                return;
            }
            
            Falcon::Item ret;
            if ( kfvm->variantToItem( fdata->metaProp().read( param ), ret ) )
            {
                vm->retval( ret );
            }
            else 
            {
                vm->retnil();
            }
        }
        
        // already raised something, just go through
    }
    
    
    static void _static_reset( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        QObject *param;
        if( _internal_get_object_param( vm, &param ) )
        {
            // the parameter is not optional for this member
            if ( param == 0 )
            {
                vm->raiseModError( new Falcon::ParamError( 
                    Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                    origin( Falcon::e_orig_runtime ).
                    extra( "O" ) ) );
                return;
            }
            
            vm->retval( (Falcon::int64) (fdata->metaProp().reset( param ) ? 1: 0) );
        }
        
        // already raised something, just go through
    }
    
    
    static void _static_type( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        vm->retval( (Falcon::int64) fdata->metaProp().type() );
    }
    
    
    static void _static_typeName( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        vm->retval( fdata->metaProp().typeName() );
    }
    
    
    static void _static_userType( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        vm->retval( (Falcon::int64) fdata->metaProp().userType() );
    }
    
    
    static void _static_write( Falcon::VMachine *vm )
    {
        KFVM *kfvm = static_cast< KFVM *>( vm );
        Falcon::CoreObject *self = vm->self().asObject();
        FalconMetaPropertyData *fdata = static_cast<FalconMetaPropertyData *>( self->getUserData() );
        
        QObject *param;
        if( _internal_get_object_param( vm, &param ) )
        {
            // the parameters are not optional for this member
            Falcon::Item *itm = vm->param( 1 );
            if ( param == 0 || itm == 0 )
            {
                vm->raiseModError( new Falcon::ParamError( 
                    Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                    origin( Falcon::e_orig_runtime ).
                    extra( "O,X" ) ) );
                return;
            }
            
            QVariant qv;
            if ( kfvm->itemToVariant( *itm, qv ) )
            {
                vm->retval( (Falcon::int64) (fdata->metaProp().write( param, qv ) ? 1: 0) );
            }
            else  
            {
                vm->retval( (Falcon::int64) 0 );
            }
        }
    }


    void DeclareFalconMetaProperty( Falcon::Module *self )
    {
        Falcon::Symbol *qme_class = self->addClass( "QMetaProperty" );
        // make this symbol unexported, that is, unavailable for instantation to scripts
        qme_class->exported( false );
        
        self->addClassMethod( qme_class, "enumerator", _static_enumerator );
        self->addClassMethod( qme_class, "isDesignable", _static_isDesignable );
        self->addClassMethod( qme_class, "isEnumType", _static_isEnumType );
        self->addClassMethod( qme_class, "isFlagType", _static_isFlagType );
        self->addClassMethod( qme_class, "isScriptable", _static_isScriptable );
        self->addClassMethod( qme_class, "isStored", _static_isStored );
        self->addClassMethod( qme_class, "isUser", _static_isUser );
        self->addClassMethod( qme_class, "isValid", _static_isValid );
        self->addClassMethod( qme_class, "isWritable", _static_isWritable );
        self->addClassMethod( qme_class, "name", _static_name );
        self->addClassMethod( qme_class, "read", _static_read );
        self->addClassMethod( qme_class, "reset", _static_reset );
        self->addClassMethod( qme_class, "type", _static_type );
        self->addClassMethod( qme_class, "typeName", _static_typeName );
        self->addClassMethod( qme_class, "userType", _static_userType );
        self->addClassMethod( qme_class, "write", _static_write );
        
        self->addConstant( "QVTYPE_Invalid", (Falcon::int64) QVariant::Invalid );
        self->addConstant( "QVTYPE_BitArray", (Falcon::int64) QVariant::BitArray );
        self->addConstant( "QVTYPE_Bitmap", (Falcon::int64) QVariant::Bitmap );
        self->addConstant( "QVTYPE_Bool", (Falcon::int64) QVariant::Bool );
        self->addConstant( "QVTYPE_Brush", (Falcon::int64) QVariant::Brush );
        self->addConstant( "QVTYPE_ByteArray", (Falcon::int64) QVariant::ByteArray );
        self->addConstant( "QVTYPE_Char", (Falcon::int64) QVariant::Char );
        self->addConstant( "QVTYPE_Color", (Falcon::int64) QVariant::Color );
        self->addConstant( "QVTYPE_Cursor", (Falcon::int64) QVariant::Cursor );
        self->addConstant( "QVTYPE_Date", (Falcon::int64) QVariant::Date );
        self->addConstant( "QVTYPE_DateTime", (Falcon::int64) QVariant::DateTime );
        self->addConstant( "QVTYPE_Double", (Falcon::int64) QVariant::Double );
        self->addConstant( "QVTYPE_Font", (Falcon::int64) QVariant::Font );
        self->addConstant( "QVTYPE_Icon", (Falcon::int64) QVariant::Icon );
        self->addConstant( "QVTYPE_Image", (Falcon::int64) QVariant::Image );
        self->addConstant( "QVTYPE_Int", (Falcon::int64) QVariant::Int );
        self->addConstant( "QVTYPE_KeySequence", (Falcon::int64) QVariant::KeySequence );
        self->addConstant( "QVTYPE_Line", (Falcon::int64) QVariant::Line );
        self->addConstant( "QVTYPE_LineF", (Falcon::int64) QVariant::LineF );
        self->addConstant( "QVTYPE_List", (Falcon::int64) QVariant::List );
        self->addConstant( "QVTYPE_Locale", (Falcon::int64) QVariant::Locale );
        self->addConstant( "QVTYPE_LongLong", (Falcon::int64) QVariant::LongLong );
        self->addConstant( "QVTYPE_Map", (Falcon::int64) QVariant::Map );
        self->addConstant( "QVTYPE_Matrix", (Falcon::int64) QVariant::Matrix );
        self->addConstant( "QVTYPE_Transform", (Falcon::int64) QVariant::Transform );
        self->addConstant( "QVTYPE_Palette", (Falcon::int64) QVariant::Palette );
        self->addConstant( "QVTYPE_Pen", (Falcon::int64) QVariant::Pen );
        self->addConstant( "QVTYPE_Pixmap", (Falcon::int64) QVariant::Pixmap );
        self->addConstant( "QVTYPE_Point", (Falcon::int64) QVariant::Point );
        //self->addConstant( "QVTYPE_PointArray", (Falcon::int64) QVariant::PointArray );
        self->addConstant( "QVTYPE_PointF", (Falcon::int64) QVariant::PointF );
        self->addConstant( "QVTYPE_Polygon", (Falcon::int64) QVariant::Polygon );
        self->addConstant( "QVTYPE_Rect", (Falcon::int64) QVariant::Rect );
        self->addConstant( "QVTYPE_RectF", (Falcon::int64) QVariant::RectF );
        self->addConstant( "QVTYPE_RegExp", (Falcon::int64) QVariant::RegExp );
        self->addConstant( "QVTYPE_Region", (Falcon::int64) QVariant::Region );
        self->addConstant( "QVTYPE_Size", (Falcon::int64) QVariant::Size );
        self->addConstant( "QVTYPE_SizeF", (Falcon::int64) QVariant::SizeF );
        self->addConstant( "QVTYPE_SizePolicy", (Falcon::int64) QVariant::SizePolicy );
        self->addConstant( "QVTYPE_String", (Falcon::int64) QVariant::String );
        self->addConstant( "QVTYPE_StringList", (Falcon::int64) QVariant::StringList );
        self->addConstant( "QVTYPE_TextFormat", (Falcon::int64) QVariant::TextFormat );
        self->addConstant( "QVTYPE_TextLengthTextLength", (Falcon::int64) QVariant::TextLength );
        self->addConstant( "QVTYPE_Time", (Falcon::int64) QVariant::Time );
        self->addConstant( "QVTYPE_UInt", (Falcon::int64) QVariant::UInt );
        self->addConstant( "QVTYPE_ULongLong", (Falcon::int64) QVariant::ULongLong );
        self->addConstant( "QVTYPE_Url", (Falcon::int64) QVariant::Url );
        self->addConstant( "QVTYPE_UserType", (Falcon::int64) QVariant::UserType );
    }
}
