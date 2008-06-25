/***************************************************************************
 * falconkrossobject.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2006 by Sebastian Sauer (mail@dipe.org)
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
 ************************************************************************/
 
#include "falconkrossobject.h"
#include "falconkfvm.h"
#include "falconfunction.h"
#include <falcon/autocstring.h>

#include <QMetaMethod>

namespace Kross
{
    static void _falcon_ko_init( ::Falcon::VMachine *vm )
    {
        // creates a new instance with an empty QObject.
        Falcon::CoreObject *self = vm->self().asObject();
        self->setUserData( new FalconCarrier( new QObject ) );
    }
    
    static void _falcon_ko_className( ::Falcon::VMachine *vm )
    {
        KFVM *kvm = static_cast< KFVM *>(vm);
        Falcon::CoreObject *self = kvm->self().asObject();
        FalconCarrier *fdata = static_cast<FalconCarrier *>( self->getUserData() );
        vm->retval( kvm->QStringToString( fdata->qobject()->metaObject()->className() ) );
    }
    
    
    static void _falcon_ko_signalNames( ::Falcon::VMachine *vm )
    {
        KFVM *kvm = static_cast< KFVM *>(vm);
        Falcon::CoreObject *self = kvm->self().asObject();
        FalconCarrier *d = static_cast<FalconCarrier *>( self->getUserData() );

        const QMetaObject* metaobject = d->qobject()->metaObject();
        const int count = metaobject->methodCount();
        Falcon::CoreArray *list = new Falcon::CoreArray( vm, count );
        
        for(int i = 0; i < count; ++i) 
        {
            const QMetaMethod &m = metaobject->method(i);
            if( m.methodType() == QMetaMethod::Signal )
                list->append( kvm->QStringToString( m.signature() ) );
        }
        
        vm->retval( list );
    }
    
    
    static void _falcon_ko_slotNames( ::Falcon::VMachine *vm )
    {
        KFVM *kvm = static_cast< KFVM *>(vm);
        Falcon::CoreObject *self = kvm->self().asObject();
        FalconCarrier *d = static_cast<FalconCarrier *>( self->getUserData() );
        
        const QMetaObject* metaobject = d->qobject()->metaObject();
        const int count = metaobject->methodCount();
        Falcon::CoreArray *list = new Falcon::CoreArray( vm, count );
        
        for(int i = 0; i < count; ++i) 
        {
            const QMetaMethod &m = metaobject->method(i);
            if( m.methodType() == QMetaMethod::Slot )
                list->append( kvm->QStringToString( m.signature() ) );
        }
        
        vm->retval( list );
    }
    
    
    static void _falcon_ko_propertyNames( ::Falcon::VMachine *vm )
    {
        KFVM *kvm = static_cast< KFVM *>(vm);
        Falcon::CoreObject *self = kvm->self().asObject();
        FalconCarrier *d = static_cast<FalconCarrier *>( self->getUserData() );

        const QMetaObject* metaobject = d->qobject()->metaObject();
        const int count = metaobject->propertyCount();
        Falcon::CoreArray *list = new Falcon::CoreArray( vm, count );
        
        for(int i = 0; i < count; ++i)
            list->append( kvm->QStringToString( metaobject->property(i).name() ) );
        
        vm->retval( list );
    }
    
    
    static void _falcon_ko_property( ::Falcon::VMachine *vm )
    {
        KFVM *kvm = static_cast< KFVM *>(vm);
        Falcon::CoreObject *self = kvm->self().asObject();
        FalconCarrier *d = static_cast<FalconCarrier *>( self->getUserData() );
        
        Falcon::Item *i_propName = vm->param(0);
        if ( i_propName == 0 || !i_propName->isString() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "S" ) ) );
            return;
        }
        
        Falcon::AutoCString propName( *i_propName->asString() );
        kvm->variantToItem( d->qobject()->property( propName.c_str() ), kvm->regA() );
    }
    
    
    static void _falcon_ko_setProperty( ::Falcon::VMachine *vm )
    {
        KFVM *kvm = static_cast< KFVM *>(vm);
        Falcon::CoreObject *self = kvm->self().asObject();
        FalconCarrier *d = static_cast<FalconCarrier *>( self->getUserData() );
        
        Falcon::Item *i_propName = vm->param(0);
        Falcon::Item *i_value = vm->param(1);
        
        if ( i_propName == 0 || !i_propName->isString() || i_value == 0 )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "S,X" ) ) );
            return;
        }
        
        Falcon::AutoCString propName( *i_propName->asString() );
        QVariant value;
        kvm->itemToVariant( *i_value, value );
        kvm->retval( (Falcon::int64)
            ( d->qobject()->setProperty( propName.c_str(), value ) ? 1: 0 ) );
    }
    
    
    static void _falcon_ko_connect( ::Falcon::VMachine *vm )
    {
        // Parameters:
        // 1: sender
        // 1: the signal (from the sender object) to connect
        // 2: method name in this object
        // (3): optional connection type
        
        Falcon::Item *i_sender = vm->param(0);
        Falcon::Item *i_signal = vm->param(1);
        Falcon::Item *i_slot = vm->param(2);
        Falcon::Item *i_type = vm->param(3);
        
        // check parameters
        if ( i_sender == 0 || ! i_sender->isObject() || 
             i_signal == 0 || ! i_signal->isString() ||
             i_slot == 0 || ! i_slot->isString() ||
             ( i_type != 0 && ! i_type->isOrdinal() )
             )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "S,S,S,[N]" ) ) );
            return;
        }
        
        // we know we're a KrossObject, but also the sender must be one.
        Falcon::CoreObject *sender = i_sender->asObject();
        if ( ! sender->derivedFrom( "KrossObject" ) )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "First parameter must be a KrossObject instance." ) ) );
            return;
        }
        
        QObject *qsender =  static_cast<FalconCarrier *>( sender->getUserData() )->qobject();
        
        // In ourselves.
        Falcon::CoreObject *self = vm->self().asObject();
        
        // the sender can take care of itself; we must wrap the receiver callable item.
        // provided it's callable...
        Falcon::Item callable;
        if ( ! self->getProperty( *i_slot->asString(), callable ) || ! callable.isCallable() )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_param_type, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "Non-callable property for this item." ) ) );
            return;
        }
        // methodize the callable (it's a function which is a method for self.
        callable.methodize( self );
        
        // everything should be fine, except for final link success.
        // we can use a bit of CPU and memory now.
        Falcon::AutoCString cSenderSignal( *i_signal->asString() );
        // normalize sender signal signature
        QByteArray sendersignal = QMetaObject::normalizedSignature( cSenderSignal.c_str() );
        FalconFunction* function = new FalconFunction(qsender, sendersignal, callable, 
            static_cast<KFVM *>(vm) );
        
        // the receiver slot is our function callback, which we say equal to the sender.
        // it will be managed anyhow by our meta-callback function
        QByteArray receiverslot = sendersignal;
        
        // perform final linking and cross fingers.
        // Dirty hack to replace SIGNAL() and SLOT() macros.
        if( ! sendersignal.startsWith('1') && ! sendersignal.startsWith('2') )
            sendersignal.prepend('2'); // prepending 2 means SIGNAL(...)
        if( ! receiverslot.startsWith('1') && ! receiverslot.startsWith('2') )
            receiverslot.prepend('1'); // prepending 1 means SLOT(...)

        #ifdef KROSS_FALCON_EXTENSION_CONNECT_DEBUG
            krossdebug( QString("Falcon KrossObject.connect sender=%1 signal=%2 receiver=%3 slot=%4").
                arg(sender->objectName()).arg(sendersignal.constData()).
                arg(function->objectName()).arg(receiverslot.constData()).toLatin1().constData() );
        #endif

        if(! QObject::connect(qsender, sendersignal, function, receiverslot) ) 
        {
            krosswarning( QString("KrossObject.connect Failed to connect").toLatin1().constData() );
            vm->retval( false );
        }
        else
        {
            vm->retval( true );
        }
    }
    
    
    static void _falcon_ko_disconnect( ::Falcon::VMachine *vm )
    {
        //TODO
        // (to do this, we must record our functions generated in the above step)
        vm->retval( false );
    }
    
    
    void DeclareFalconKrossObject( Falcon::Module *self )
    {
        Falcon::Symbol *ko_class = self->addClass( "KrossObject", _falcon_ko_init );
        ko_class->carryFalconData();
        
        self->addClassMethod( ko_class, "className", _falcon_ko_className );
        self->addClassMethod( ko_class, "signalNames", _falcon_ko_signalNames );
        self->addClassMethod( ko_class, "slotNames", _falcon_ko_slotNames );
        self->addClassMethod( ko_class, "propertyNames", _falcon_ko_propertyNames );
        self->addClassMethod( ko_class, "property", _falcon_ko_property );
        self->addClassMethod( ko_class, "setProperty", _falcon_ko_setProperty );
        self->addClassMethod( ko_class, "connect", _falcon_ko_connect );
        self->addClassMethod( ko_class, "disconnect", _falcon_ko_disconnect );
    }
    
    
    Falcon::FalconData *FalconCarrier::clone() const
    {
        return 0;
    }
    
    void FalconCarrier::gcMark( Falcon::VMachine* ) 
    {
    }
}
