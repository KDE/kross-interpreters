/***************************************************************************
 * falconprovider.h
 * This file is part of the KDE project
 * copyright (C)2004-2007 by jonnymind@falconpl.org
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

#include "falconprovider.h"
#include "falconkfvm.h"

#include <falcon/engine.h>
#include <QList>
#include <QObject>
#include <QMetaMethod>
#include <QMetaProperty>
#include <QMetaEnum>

using namespace Kross;

// A bit of forward declarations.
// Reflected class initializer
static void init_reflect_object_func( Falcon::VMachine *vm );
// Sole method given to reflector objects.
static void reflect_object_method( Falcon::VMachine *vm );

// Property reflectors.
static Falcon::reflectionFuncDecl reflect_object_prop_from;
static Falcon::reflectionFuncDecl reflect_object_prop_to;

// Local class used to manage our inner data made of QPointers to QObjects
class QObjectPtrManager: public Falcon::ObjectManager
{
    void *onInit( Falcon::VMachine * ){
        // Better let the init method to handle this for now.
        return 0;
    }
    
    void onGarbageMark( Falcon::VMachine *, void * ) {}
    
    virtual void onDestroy( Falcon::VMachine *, void *user_data ) {
        // destroy the pointer.
        QPointer<QObject> *ptr = (QPointer<QObject>*) user_data;
        delete ptr;
    }
    
    virtual void *onClone( Falcon::VMachine *, void *user_data ) {
        QPointer<QObject> *ptr = (QPointer<QObject>*) user_data;
        
        // If disabled, let the owner to raise 
        if ( ptr->isNull() )
            return 0;
        
        // Otherwise, return 
        return new QPointer<QObject>( ptr->data() );
    }
};

static QObjectPtrManager qobject_ptr_manager;


namespace Kross {

    /// \internal
    class FalconProviderPrivate {
        friend class FalconProvider;
        
        /** We want to have the action that generated the script handy. */
        Action* m_action;
        
        /** Place to store Meta* things so that they can be released at module termination. */
        QList<QMetaProperty> m_metaProps;
        QList<QMetaMethod> m_metaMethods;
    };
}

FalconProvider::FalconProvider(Action* action):
    d( new FalconProviderPrivate )
{
    d->m_action = action;
}

FalconProvider::~FalconProvider()
{
    delete d;
}

Falcon::Symbol* FalconProvider::onSymbolRequest( const Falcon::String &name )
{
    Falcon::AutoCString cstr( name );

    #ifdef KROSS_FALCON_PROVIDER_DEBUG
        krossdebug(QString("FalconProvider::onSymbolRequest( \"%1\" )").arg(cstr.c_str()));
    #endif
    
    QString qsName( cstr.c_str() );
    
    if( d->m_action->hasObject(qsName) ) {
        #ifdef KROSS_FALCON_PROVIDER_DEBUG
            krossdebug( QString("FalconProvider::onSymbolRequest() module=%1 is internally provided").arg(qsName) );
        #endif
        QObject* object = d->m_action->object(qsName);
        Q_ASSERT(object);
        Falcon::Symbol* sym = reflectObject( name, object );
        Q_ASSERT(sym);
        return sym;
    }
    
    return 0;
}

Falcon::Symbol *FalconProvider::reflectObject( const Falcon::String &objName, QObject *obj )
{
    #ifdef KROSS_FALCON_PROVIDER_DEBUG
        krossdebug(QString("FalconProvider::reflectObject( \"%1\" )").arg(obj->objectName()));
    #endif
    
    const QMetaObject* metaobject = obj->metaObject();
    
    // First, we must create a constructor for our reflected object that 
    // will take care to set the object instance in the reflected class
    Falcon::Symbol *ctor = addExtFunc( objName + "._init", init_reflect_object_func, false );
    
    // our constructor will know what data to assign.
    QPointer<QObject> *pobj = new QPointer<QObject>( obj );
    ctor->getExtFuncDef()->extra( pobj );
    
    // let's create a singleton instance that will receive the object we just created.
    Falcon::Symbol *falinst = addSingleton( objName, ctor );
    Falcon::Symbol *falcls = falinst->getInstance();
    falcls->getClassDef()->setObjectManager( &qobject_ptr_manager );
    
    { // initialize methods.
        const int count = metaobject->methodCount();
        for(int i = 0; i < count; ++i) {
            QMetaMethod member = metaobject->method(i);
            
            const QString signature = member.signature();
            const QString name = signature.left(signature.indexOf('('));
            // if we have already cached the name, we don't need this.
            Falcon::String mthName;
            mthName.fromUTF8( name.toUtf8() );
            if ( falcls->getClassDef()->hasProperty( mthName ) )
            {
                #ifdef KROSS_FALCON_PROVIDER_DEBUG
                    krossdebug(QString("FalconProvider::reflectObject - skipping method \"%1\" ").arg(signature));
                #endif
                continue;
            }
            #ifdef KROSS_FALCON_PROVIDER_DEBUG
            else
                krossdebug(QString("FalconProvider::reflectObject - adding method \"%1\" ").arg(signature));
            #endif

            
            // Save our method in a place that will be safe for all the execution.
            d->m_metaMethods.push_back( member );

            // reflect it into our method;
            // Get the method symbol so that we can...
            const Falcon::Symbol *mthd = addClassMethod( falcls, mthName, reflect_object_method  )
                .asSymbol();
            
            // ... put in the meta method reference.
            mthd->getExtFuncDef()->extra( &d->m_metaMethods.back() );
        }
    }

    { // initialize properties
        const int count = metaobject->propertyCount();
        for(int i = 0; i < count; ++i) {
            QMetaProperty prop = metaobject->property(i);
            // if we have already cached the name, we don't need this.
            Falcon::String propName;
            propName.bufferize( prop.name() );
            if ( falcls->getClassDef()->hasProperty( propName ) )
            {
                #ifdef KROSS_FALCON_PROVIDER_DEBUG
                    Falcon::AutoCString mc( propName );
                    krossdebug(QString("FalconProvider::reflectObject - skipping property \"%1\" ").arg(mc.c_str()));
                #endif
                continue;
            }
            #ifdef KROSS_FALCON_PROVIDER_DEBUG
            else
                krossdebug(QString("FalconProvider::reflectObject - adding property \"%1\" ").arg(prop.name()));
            #endif

            // Store the property in a place that will be safe as long as we exist.
            d->m_metaProps.push_back( prop );
            
            // reflect it into our property
            // If the property is read only, don't set the prop_to reflector.
            addClassProperty( falcls, propName )
                .setReflectFunc( reflect_object_prop_from,
                        prop.isWritable() ? reflect_object_prop_to: 0,
                        &d->m_metaProps.back() );
        }
    }

    { // initialize enumerations
      // In falcon, enumerations can be implemented as normal non-reflective read-only properties.
        const int count = metaobject->enumeratorCount();
        for(int i = 0; i < count; ++i) {
            QMetaEnum e = metaobject->enumerator(i);
            const int kc = e.keyCount();
            for(int k = 0; k < kc; ++k) {
                
                Falcon::String enumName;
                enumName.bufferize( e.key(k) );
                if ( falcls->getClassDef()->hasProperty( enumName ) )
                {
                    #ifdef KROSS_FALCON_PROVIDER_DEBUG
                        Falcon::AutoCString mc( enumName );
                        krossdebug(QString("FalconProvider::reflectObject - skipping enum, \"%1\" ").arg(mc.c_str()));
                    #endif
                    continue;
                }
                #ifdef KROSS_FALCON_PROVIDER_DEBUG
                else
                    krossdebug(QString("FalconProvider::reflectObject - adding enum, \"%1\" ").arg(e.key(k)));
                #endif
                
                addClassProperty( falcls, enumName )
                    .setInteger( e.value(k) )
                    .setReadOnly(true);
            }
        }
    }
    
    // Return our instance.
    return falinst;
}

//===========================================================0
// The reflectors.
//

// Initialize the new instance.
void init_reflect_object_func( Falcon::VMachine *vm )
{
    // get the data from the symbol of the constructor and set it as our user data.
    Falcon::CoreObject *self = vm->self().asObject();
    Falcon::ExtFuncDef *def = vm->currentSymbol()->getExtFuncDef();
    // we are a singleton, we can get the only instance of the user data that has been
    // prepared for us.
    Q_ASSERT( def->extra() != 0 );
    
    #ifdef KROSS_FALCON_PROVIDER_DEBUG
        if ( def->extra() == 0 )
        {
            krossdebug(QString("init_reflect_object_func - extra already consumed!") );
            abort();
        }
    #endif
    
    // Swap the pointer set in the constructor in this instance.
    QPointer<QObject> *ptr = (QPointer<QObject>*) def->extra();
    def->extra( 0 );
    self->setUserData( ptr );
}

// Calls a reflected method
void reflect_object_method( Falcon::VMachine *vm )
{
    Falcon::CoreObject *self = vm->self().asObject();
    QPointer<QObject> *ptr = (QPointer<QObject>*) self->getUserData();
    KFVM *kvm = (KFVM*) vm;
    
    // raise if we're burned.
    if( ptr->isNull() )
    {
        vm->raiseModError( new Falcon::CodeError( 
            Falcon::ErrorParam( 5000, __LINE__ ).
            desc( "Invalid object (already destroyed)" ).
            origin( Falcon::e_orig_runtime ).
            extra( self->instanceOf()->name() ) ) );
        return;
    }
}


// Read a property from the QObject and send it to our VM.
void reflect_object_prop_from(
        Falcon::CoreObject *self, 
        void *user_data, 
        Falcon::Item &property, 
        const Falcon::PropEntry& entry )
{
    QPointer<QObject> *ptr = (QPointer<QObject>*) user_data;
    KFVM *kvm = (KFVM*) self->origin();
    
    // raise if we're burned.
    if( ptr->isNull() )
    {
        kvm->raiseModError( new Falcon::CodeError( 
            Falcon::ErrorParam( 5000, __LINE__ ).
            desc( "Invalid object (already destroyed)" ).
            origin( Falcon::e_orig_runtime ).
            extra( self->instanceOf()->name() ) ) );
        return;
    }

    // The property is in the user-available field of our entry
    QMetaProperty *prop = (QMetaProperty *) entry.reflect_data;
    
    // first, turn the variant into an item.
    QVariant qvariant = prop->read ( ptr->data() );
    if ( qvariant.isValid() )
    {
        // try to convert the variant to a falcon item (directly in the property)
        if ( ! kvm->variantToItem( qvariant, property ) )
        {
            kvm->raiseModError( new Falcon::CodeError( 
                Falcon::ErrorParam( 5004, __LINE__ ).
                desc( "Property being read not convertible to Falcon Item." ).
                origin( Falcon::e_orig_runtime ).
                extra( *entry.m_name ) ) );
        }
    }
    else 
    {
        kvm->raiseModError( new Falcon::CodeError( 
                Falcon::ErrorParam( 5003, __LINE__ ).
                desc( "Reading property failed" ).
                origin( Falcon::e_orig_runtime ).
                extra( *entry.m_name ) ) );
    }
}


// Gets an item from our VM and sets it into the reflected QObject.
void reflect_object_prop_to(
        Falcon::CoreObject *self, 
        void *user_data, 
        Falcon::Item &property, 
        const Falcon::PropEntry& entry )
{
    QPointer<QObject> *ptr = (QPointer<QObject>*) user_data;
    KFVM *kvm = (KFVM*) self->origin();
    
    // raise if we're burned.
    if( ptr->isNull() )
    {
        kvm->raiseModError( new Falcon::CodeError( 
            Falcon::ErrorParam( 5000, __LINE__ ).
            desc( "Invalid object (already destroyed)" ).
            origin( Falcon::e_orig_runtime ).
            extra( self->instanceOf()->name() ) ) );
    }
    
    // obtain the qvariant to set
    QVariant qvariant;
    if ( kvm->itemToVariant( property, qvariant ) )
    {
        // the property is in the extra value of our entry
        QMetaProperty *prop = (QMetaProperty *) entry.reflect_data;
        if ( ! prop->write( ptr->data(), qvariant ) )
        {
            kvm->raiseModError( new Falcon::CodeError( 
                Falcon::ErrorParam( 5001, __LINE__ ).
                desc( "Setting property failed" ).
                origin( Falcon::e_orig_runtime ).
                extra( *entry.m_name ) ) );
        }
    }
    else {
        kvm->raiseModError( new Falcon::CodeError( 
            Falcon::ErrorParam( 5002, __LINE__ ).
            desc( "Item being set not convertible to variant." ).
            origin( Falcon::e_orig_runtime ).
            extra( *entry.m_name ) ) );
    }
}

