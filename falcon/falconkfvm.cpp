/***************************************************************************
* falconkfvm.cpp
* This file is part of the KDE project
* copyright (C)2007-2008 by Giancarlo Niccolai (jonnymind@falconpl.org)
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

#include "falconkfvm.h"
#include <falcon/autocstring.h>

#include <QPoint>
#include <QChar>
#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMetaType>
#include <QVariantList>
#include <QVariantMap>
#include <QVarLengthArray>

#include <kross/core/action.h>
#include <kross/core/metatype.h>

#include "falconkrossobject.h"
#include "falconobjman.h"
#include "falconkopaque.h"
#include "metatypeflex.h"

using namespace Kross;

namespace Kross {
    
    class KVFMPrivate {
    public:
        
        /// Our dynamic object import provider.
        Falcon::Module *m_symProvider;
        
        /// Live representation of our module
        Falcon::LiveModule *m_liveProvider;
        
        /// pre-cached KrossOpaque class instance
        Falcon::CoreClass *m_kopaque;
        
        /// Place to store Meta* things so that they can be released at VM termination.
        QList<QMetaProperty> m_metaProps;
        
        /// Place to store Meta* things so that they can be released at VM termination.
        QList<QMetaMethodWithID> m_metaMethods;
        
        /// Our acrtion
        Action *m_action;
        
        KVFMPrivate(Action *a):
            m_symProvider( new Falcon::Module ),
            m_action( a )
        {
            m_symProvider->name( "kfvm.flexyprovider" );
        }
        
        ~KVFMPrivate()
        {
            m_symProvider->decref();
        }
    };
}


// Forward decls
static void init_reflect_object_func( Falcon::VMachine *vm );

// Calls a reflected method
static void reflect_object_method( Falcon::VMachine *vm );

// Read a property from the QObject and send it to our VM.
static void reflect_object_prop_from(
        Falcon::CoreObject *self, 
        void *user_data, 
        Falcon::Item &property, 
        const Falcon::PropEntry& entry );

// Gets an item from our VM and sets it into the reflected QObject.
static void reflect_object_prop_to(
        Falcon::CoreObject *self, 
        void *user_data, 
        Falcon::Item &property, 
        const Falcon::PropEntry& entry );

//===============================================================
// KFVM basic implementation
//===============================================================

KFVM::KFVM( Action *action ):
    d( new KVFMPrivate(action) )
{
    // the VM is already initialzied at this point
    
    // let's add KrossOpaque class our inner provider
    DeclareKrossOpaque( d->m_symProvider );
    
    // Ok, the inner module is ready; let's activate it.
    bool done = link( d->m_symProvider );
    Q_ASSERT( done );
    // we may want to cache the instance for future reference.
    d->m_liveProvider = findModule( "kfvm.flexyprovider" );
    Q_ASSERT( d->m_liveProvider != 0 );
    
    // Precache live instance of the Falcon core class
    Falcon::Item *koi = findWKI( "KrossOpaque" );
    Q_ASSERT( koi != 0  && koi->isClass() );
    
    d->m_kopaque = koi->asClass();
}


KFVM::~KFVM() {
    delete d;
}


// Callback on symbol requested on load.s
bool KFVM::linkSymbolDynamic( const Falcon::String &name, Falcon::SymModule &symdata )
{
    Falcon::AutoCString cstr( name );

    #ifdef KROSS_FALCON_PROVIDER_DEBUG
        krossdebug(QString("KFVM::linkSymbolDynamic( \"%1\" )").arg(cstr.c_str()));
    #endif

    // First of all, let's search for the required name in the objects we've been provided.
    QString qsName( cstr.c_str() );
    
    if( d->m_action->hasObject(qsName) ) 
    {
        #ifdef KROSS_FALCON_VM_DEBUG
            krossdebug( QString("KFVM::linkSymbolDynamic() object=%1 is internally provided").arg(qsName) );
        #endif
        QObject* object = d->m_action->object(qsName);
        Q_ASSERT(object);
        Falcon::Symbol *sym = reflectSingleton( name, object );
        if ( sym != 0 )
        {
            #ifdef KROSS_FALCON_VM_DEBUG
                krossdebug( QString("KFVM::linkSymbolDynamic singleton object \"%1\" reflection succesful").arg(qsName) );
            #endif
            symdata = Falcon::SymModule( d->m_liveProvider, sym );
            return true;
        }
        return false;
    }
    
    // fallback to the base class dynamic providers
    return VMachine::linkSymbolDynamic( name, symdata );
} 

//===============================================================
// KFVM class reflection
//===============================================================

Falcon::Symbol *KFVM::reflectSingleton( const Falcon::String &objName, QObject *obj )
{
    #ifdef KROSS_FALCON_VM_DEBUG
        Falcon::AutoCString cstr_name( objName );
        krossdebug(QString("KFVM::reflectSingleton( \"%1(%2)\" )").arg(cstr_name.c_str()).arg(obj->objectName()));
    #endif
    
    // First, assert that the object name and class name are different.
    const QMetaObject *metaobject = obj->metaObject();
    if( metaobject->className() == objName )
    {
        #ifdef KROSS_FALCON_VM_DEBUG
            krossdebug(QString("KFVM::reflectSingleton( \"%1\" ) - failed to provide the singleton object.")
                .arg(cstr_name.c_str()));
        #endif
        return 0;
    }
    
    // Create a normal anonymous object.
    // We know we don't hold the object.
    Falcon::CoreObject *falobj = reflectObject( obj, false );
    Q_ASSERT( falobj != 0 );
    
    // create a symbol for the variable that will hold our item.
    Falcon::Symbol *singleSym = d->m_symProvider->addGlobal( objName );
    // be sure to allocate enough space in the module global table.
    if ( singleSym->itemId() >= d->m_liveProvider->globals().size() )
    {
        d->m_liveProvider->globals().resize( singleSym->itemId() );
    }
    
    // link the symbol; as we know that it's just a global variable (now), the
    // net effect will just be that of exporting the symbol name to the global
    // name space, and create a reference in the global symbol table.
    if (! linkCompleteSymbol( singleSym, d->m_liveProvider ) )
    {
        // failure means the name is already taken.
        #ifdef KROSS_FALCON_VM_DEBUG
            krossdebug(QString("KFVM::reflectSingleton( \"%1\" ) - failed to link name for the singleton object.")
                    .arg(cstr_name.c_str()));
        #endif
        return 0;
    }
    
    // After link, we know that the correct item is in the live module we have recored
    // at the position indicated by the symbol id.
    d->m_liveProvider->globals().itemAt( singleSym->itemId() ).dereference()
        ->setObject( falobj );
    
    #ifdef KROSS_FALCON_VM_DEBUG
        krossdebug(QString("KFVM::reflectSingleton created singleton \"%1\"")
            .arg(cstr_name.c_str()) );
    #endif
    return singleSym;
}


Falcon::CoreObject *KFVM::reflectObject( QObject *obj, bool bOwn )
{
    #ifdef KROSS_FALCON_VM_DEBUG
        krossdebug(QString("KFVM::reflectObject( %1, %2 )").arg(obj->objectName()).arg( bOwn ) );
    #endif
    
    // get (or create) the class of this object.
    Falcon::CoreClass *cls = reflectClass( obj );
    
    // could the class be reflected?
    if( cls == 0 )
    {
        #ifdef KROSS_FALCON_VM_DEBUG
            krossdebug(QString("KFVM::reflectObject - failed to reflect class for %1").arg(obj->objectName()) );
        #endif
        return 0;
    }
    // create the instance
    Falcon::CoreObject *cobj = cls->createInstance();
    
    #ifdef KROSS_FALCON_VM_DEBUG
        krossdebug(QString("KFVM::reflectObject created instance for %1").arg(obj->objectName()) );
    #endif
    // pass the appropriate user data
    cobj->setUserData( new OwnerPointer( obj, bOwn ) );
    
    return cobj;
}


//===================================================================
// Defining reflectClass for the VM here
//

Falcon::CoreClass* KFVM::reflectClass( QObject *obj )
{
    const QMetaObject *metaobject = obj->metaObject();
    
    #ifdef KROSS_FALCON_VM_DEBUG
        krossdebug(QString("KFVM::reflectClass(%1) -> %2").arg(obj->objectName()).arg(metaobject->className()));
    #endif
    // First, is our class an exported - well known symbol?
    Falcon::Item *wki = findWKI( metaobject->className() );
    
    if ( wki != 0 )
    {
        // yes, but is it a class?
        if ( ! wki->isClass() )
            return 0;
        
        #ifdef KROSS_FALCON_VM_DEBUG
            krossdebug(QString("KFVM::reflectClass - already provided class %1").arg(metaobject->className()));
        #endif
        // ok, we can return the pre-existing class.
        return wki->asClass();
    }
    
    // just candy grammar
    Falcon::Module* mod = d->m_symProvider;
    
    // The constructor here is needed for object created autonomously in the script,
    // which may be derived from QT class.
    // In fact, while object reflected from outside are automatically stored inside the 
    // instances (and we won't call the init method for those instances), the init
    // method of the class must be readied and able to provide a consistent inner data,
    // or the scripts will crash if trying to create new instances of QT objects.
    Falcon::String falName = metaobject->className();
    Falcon::Symbol *ctor = mod->addExtFunc( falName + "._init", init_reflect_object_func, false );
    Q_ASSERT( ctor != 0 );
    
    // our constructor will know how to create a new instance of this class.
    // If we have a MetaType for this class, we'll be able to build it,
    // otherwise the init method will raise an execption (and the user should load the QT module).
    ctor->getExtFuncDef()->extra( (void *) QMetaType::type( metaobject->className() ) );

    // Create the dynamic class that we're going to populate.
    Falcon::Symbol *dyncls = mod->addClass( metaobject->className(), ctor );
    
    // make it available to future instance requests.
    dyncls->setWKS( true );
    
    // and tell the world we'll be managing OwnerPointer(s)
    dyncls->getClassDef()->setObjectManager( &qobject_ptr_manager );
    
    { // initialize methods.
        const int count = metaobject->methodCount();
        for(int i = 0; i < count; ++i) {
            QMetaMethod member = metaobject->method(i);
            
            const QString signature = member.signature();
            const QString name = signature.left(signature.indexOf('('));
            // if we have already cached the name, we don't need this.
            Falcon::String mthName;
            mthName.fromUTF8( name.toUtf8() );
            if ( dyncls->getClassDef()->hasProperty( mthName ) )
            {
                #ifdef KROSS_FALCON_VM_DEBUG
                    krossdebug(QString("FalconProvider::reflectObject - skipping method \"%1\" ").arg(signature));
                #endif
                continue;
            }
            #ifdef KROSS_FALCON_VM_DEBUG
            else
                krossdebug(QString("FalconProvider::reflectObject - adding method \"%1\" ").arg(signature));
            #endif

            
            // Save our method in a place that will be safe for all the execution.
            d->m_metaMethods.push_back( Kross::QMetaMethodWithID( i, member ) );

            // reflect it into our method;
            // Get the method symbol so that we can...
            const Falcon::Symbol *mthd = mod->addClassMethod( dyncls, mthName, reflect_object_method  )
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
            if ( dyncls->getClassDef()->hasProperty( propName ) )
            {
                #ifdef KROSS_FALCON_VM_DEBUG
                    Falcon::AutoCString mc( propName );
                    krossdebug(QString("FalconProvider::reflectClass - skipping property \"%1\" ").arg(mc.c_str()));
                #endif
                continue;
            }
            #ifdef KROSS_FALCON_VM_DEBUG
            else
                krossdebug(QString("FalconProvider::reflectClass - adding property \"%1\" ").arg(prop.name()));
            #endif

            // Store the property in a place that will be safe as long as we exist.
            d->m_metaProps.push_back( prop );
            
            // reflect it into our property
            // If the property is read only, don't set the prop_to reflector.
            mod->addClassProperty( dyncls, propName )
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
                if ( dyncls->getClassDef()->hasProperty( enumName ) )
                {
                    #ifdef KROSS_FALCON_VM_DEBUG
                        Falcon::AutoCString mc( enumName );
                        krossdebug(QString("FalconProvider::reflectClass - skipping enum, \"%1\" ").arg(mc.c_str()));
                    #endif
                    continue;
                }
                #ifdef KROSS_FALCON_VM_DEBUG
                else
                    krossdebug(QString("FalconProvider::reflectClass - adding enum, \"%1\" ").arg(e.key(k)));
                #endif
                
                mod->addClassProperty( dyncls, enumName )
                    .setInteger( e.value(k) )
                    .setReadOnly(true);
            }
        }
    }
    
    // now our class is ready to be linked.
    // We know that the livemodule for our dynamic module is in m_liveProvider
    // be sure to allocate enough space in the module global table.
    if ( dyncls->itemId() >= d->m_liveProvider->globals().size() )
    {
        d->m_liveProvider->globals().resize( dyncls->itemId() );
    }
    
    if ( ! linkCompleteSymbol( dyncls, d->m_liveProvider ) )
    {
        // Well, should not happen
        Q_ASSERT( false );
        return 0;
    }

    // we have the coreclass instance in the correct position in the live module.
    const Falcon::Item &clsItem = d->m_liveProvider->globals().itemAt( dyncls->itemId() );
    Q_ASSERT( clsItem.isClass() );
    return clsItem.asClass();
}



//===============================================================
// KFVM item type conversion.
//===============================================================
    
bool KFVM::itemToVariant( const Falcon::Item &item, QVariant &variant )
{
    switch ( item.type() )
    {
        // Conversion from the nil value
        case FLC_ITEM_NIL:
            variant.clear();
        break;
        
        // Conversion from falcon INT (int64 signed)
        case FLC_ITEM_INT:
            variant.setValue( item.asInteger() );
        break;
        
        // Conversion from falcon numeric (C double)
        case FLC_ITEM_NUM:
            variant.setValue( item.asNumeric() );
        break;
        
        // Conversion from falcon String.
        case FLC_ITEM_STRING:
        {
            Falcon::AutoCString utf8rep( *item.asString() );
            variant.setValue( QString( utf8rep.c_str() ) );
        }
        break;
        
        case FLC_ITEM_ARRAY:
        {
            QList<QVariant> ret;
            Falcon::CoreArray *array = item.asArray();
            
            for ( Falcon::uint32 i = 0; i < array->length(); i++ )
            {
                QVariant temp;
                if ( ! itemToVariant( array->at(i), temp ) )
                    return false;
                
                ret.push_back( temp );
            }
            
            variant = ret;
        }
        break;
        
        case FLC_ITEM_DICT:
        {
            QMap<QString, QVariant> ret;
            Falcon::CoreDict *dict = item.asDict();
            Falcon::DictIterator *iter = dict->first();
            while( iter->isValid() )
            {
                // First, see if we can convert the value.
                QVariant value;
                if ( ! itemToVariant( iter->getCurrent(), value ) )
                    return false;
                    
                // Fine, now we must convert the key item to a string.
                // The conversion may fail; in that case, we just got to return.
                Falcon::String tgt;
                itemToString( tgt, &iter->getCurrentKey() );
                if ( hadError() )
                {
                    delete iter;
                    return false;
                }
                QString skey;
                Falcon::AutoCString ctgt( tgt );
                skey = QString::fromUtf8( ctgt.c_str() );
                
                // Create the entry
                ret[ skey ] = value;
                
                iter->next();
            }
            
            delete iter;
            variant = ret;
        }
        break;
        
        case FLC_ITEM_OBJECT:
        {
            // is that a QPoint intance?
            Falcon::CoreObject *cobj = item.asObject();
            if ( cobj->derivedFrom( "QPoint" ) )
            {
                Falcon::Item i_x, i_y; 
                cobj->getProperty( "x", i_x );
                cobj->getProperty( "y", i_y );
                
                // have we a numeric instance? -- return a QPointF 
                if ( i_x.isNumeric() || i_y.isNumeric() )
                {
                    variant.setValue( QPointF( i_x.forceNumeric(), i_y.forceNumeric() ) );
                }
                else {
                    variant.setValue( QPoint( i_x.forceInteger(), i_y.forceInteger() ) );
                }
            }
        }
        break;
        
        default:
            raiseError( new Falcon::ParamError( Falcon::ErrorParam( Falcon::e_param_type ).
                    origin( Falcon::e_orig_vm ).
                    extra( "Can't convert Falcon::Item into QVariant" ) )
                );
            return false;
    }
    
    return true;
}

bool KFVM::variantToItem( const QVariant &variant, Falcon::Item &item )
{
    switch( variant.type() )
    {
        case QVariant::Invalid:
            item.setNil();
        break;
        
        case QVariant::Bool:
            item.setInteger( (Falcon::int64) ( variant.toBool() ? 1: 0 ) );
        break;
        
        case QVariant::Double:
            item.setNumeric( variant.toDouble() );
        break;
        
        case QVariant::Int:
        case QVariant::UInt:
        case QVariant::LongLong:
            item.setInteger( variant.toLongLong() );
        break;

        case QVariant::ULongLong:
            // Todo: check range break
            item.setInteger( variant.toLongLong() );
        break;

        case QVariant::Char:
        {
            //TODO: maybe an integer representation would be more efficient?
            Falcon::String *tstr = new Falcon::GarbageString( this );
            tstr->append( variant.toChar().unicode() );
            item = tstr;
        }
        break;

        case QVariant::String:
        {
            Falcon::String *tstr = new Falcon::GarbageString( this );
            tstr->fromUTF8( variant.toString().toUtf8().data() );
            item.setString( tstr );
        }
        break;
        
        case QVariant::List:
        {
            const QList<QVariant> &qlist = variant.toList();
            Falcon::CoreArray *array = new Falcon::CoreArray( this, qlist.size() );
            QList<QVariant>::ConstIterator qliter = qlist.constBegin();
            while( qliter != qlist.constEnd() )
            {
                Falcon::Item value;
                if ( ! variantToItem( *qliter, value ) )
                    return false;
                array->append( value );
                ++qliter;
            }
        }
        break;
        
        case QVariant::StringList:
        {
            const QList<QString> &qlist = variant.toStringList();
            Falcon::CoreArray *array = new Falcon::CoreArray( this, qlist.size() );
            QList<QString>::ConstIterator qliter = qlist.constBegin();
            while( qliter != qlist.constEnd() )
            {
                Falcon::String *value = new Falcon::GarbageString( this );
                value->fromUTF8( qliter->toUtf8().data() );
                array->append( value );
                ++qliter;
            }
        }
        break;
        
        case QVariant::Map:
        {
            const QMap<QString, QVariant> &qMap = variant.toMap();
            Falcon::CoreDict *map = new Falcon::LinearDict( this, qMap.size() );
            QMap<QString, QVariant>::ConstIterator qmiter = qMap.constBegin();
            while( qmiter != qMap.constEnd() )
            {
                Falcon::Item value;
                if ( ! variantToItem( qmiter.value(), value ) )
                    return false;
                
                Falcon::String *key = new Falcon::GarbageString( this );
                key->fromUTF8( qmiter.key().toUtf8().data() );
                map->insert( key, value );
                ++qmiter;
            }
        }
        break;

        // That's how a QPoint/Falcon binding can be done.
        case QVariant::Point:
        {
            // We MUST have an istance of the QPoint class in our global table.
            Falcon::Item *i_qp = findGlobalItem( "QPoint" );
            Q_ASSERT( i_qp != 0 );
            // if it's a class, then we can create an instance from that
            QPoint pt = variant.toPoint();
            if ( i_qp->isClass() )
            {
                Falcon::CoreObject *instance = i_qp->asClass()->createInstance();
                instance->setProperty( "x", (Falcon::int64) pt.x() );
                instance->setProperty( "y", (Falcon::int64) pt.y() );
                item = instance; 
            }
            else 
            {
                // someone removed the class. Oh, well...
                item = new Falcon::CoreArray( this, 2 );
                item.asArray()->append( (Falcon::int64) pt.x() );
                item.asArray()->append( (Falcon::int64) pt.y() );
            }
        }
        break;
        
        case QVariant::PointF:
        {
            // We MUST have an istance of the QPoint class in our global table.
            Falcon::Item *i_qp = findGlobalItem( "QPoint" );
            Q_ASSERT( i_qp != 0 );
            // if it's a class, then we can create an instance from that
            QPointF pt = variant.toPointF();
            if ( i_qp->isClass() )
            {
                Falcon::CoreObject *instance = i_qp->asClass()->createInstance();
                instance->setProperty( "x", Falcon::Item((Falcon::numeric) pt.x()) );
                instance->setProperty( "y", Falcon::Item((Falcon::numeric) pt.y()) );
                item = instance; 
            }
            else 
            {
                // someone removed the class. Oh, well...
                item = new Falcon::CoreArray( this, 2 );
                item.asArray()->append( (Falcon::numeric) pt.x() );
                item.asArray()->append( (Falcon::numeric) pt.y() );
            }
        }
        break;

        default:
            reflectFromMetaType( variant.userType(), variant.value<void*>(), item, false );
            
            /*raiseError( new Falcon::ParamError( Falcon::ErrorParam( Falcon::e_param_type ).
                    origin( Falcon::e_orig_vm ).
                    extra( "Can't convert QVariant into Falcon::Item" ) )
                );
                
                return false;*/
    }
    
    return true;
}


Falcon::CoreObject *KFVM::QObjectToKrossObject( QObject *qo ) const
{
    Falcon::Item *i_ko_class = findGlobalItem( "KrossObject" );
    Q_ASSERT( i_ko_class != 0 );
    // if it's a class, then we can create an instance from that
    if ( i_ko_class->isClass() )
    {
        Falcon::CoreObject *instance = i_ko_class->asClass()->createInstance();
        instance->setUserData( new FalconCarrier( qo ) );
        return instance;
    }
    else
        Q_ASSERT( false );
    
    return 0;
}

Falcon::String *KFVM::QStringToString( const QString &qstring )
{
    Falcon::String *ret = new Falcon::GarbageString( this );
    ret->fromUTF8( qstring.toUtf8().data() );
    return ret;
}

//=======================================================================
// Medatadata conversion
//

void KFVM::reflectFromMetaType( int type, void *data, Falcon::Item &item, bool bOwn )
{
    switch( type )
    {
        // for plain types, we do a direct conversion
        
        case QMetaType::Void:
            item.setNil();
        break;
        
        case QMetaType::Bool:
            item.setBoolean( *static_cast<bool *>(data) );
        break;
        
        case QMetaType::Int:
            item.setInteger( (Falcon::int64) *static_cast<int *>(data) );
        break;
        
        case QMetaType::UInt:
            item.setInteger( (Falcon::int64) *static_cast<unsigned int *>(data) );
        break;

        case QMetaType::Double:
            item.setNumeric( (Falcon::numeric) *static_cast<double *>(data) );
        break;
        
        case QMetaType::Long:
            item.setInteger( (Falcon::int64) *static_cast<long *>(data) );
        break;
        
        case QMetaType::LongLong:
            //falcon int64 is portable enough
            item.setInteger( (Falcon::int64) *static_cast<Falcon::int64 *>(data) );
        break;
        
        case QMetaType::Short:
            item.setInteger( (Falcon::int64) *static_cast<short int *>(data) );
        break;

        case QMetaType::Char:
            item.setInteger( (Falcon::int64) *static_cast<char *>(data) );
        break;
        
        case QMetaType::ULong:
            item.setInteger( (Falcon::int64) *static_cast<unsigned long *>(data) );
        break;
        
        case QMetaType::ULongLong:
            //falcon uint64 is portable enough
            item.setInteger( (Falcon::int64) *static_cast<Falcon::uint64 *>(data) );
        break;
        
        case QMetaType::UShort:
            item.setInteger( (Falcon::int64) *static_cast<unsigned short *>(data) );
        break;

        case QMetaType::UChar:
            item.setInteger( (Falcon::int64) *static_cast<unsigned char *>(data) );
        break;
        
        case QMetaType::Float:
            item.setInteger( (Falcon::int64) *static_cast<float *>(data) );
        break;

        // for chars, strings and byte arrays we use Falcon equivalents.
        case QMetaType::QChar:
        {
            //TODO: maybe an integer representation would be more efficient?
            Falcon::String *tstr = new Falcon::GarbageString( this );
            tstr->append( static_cast<QChar *>(data)->unicode() );
            item = tstr;
        }
        break;
        
        case QMetaType::QString:
        {
            Falcon::String *tstr = new Falcon::GarbageString( this );
            tstr->fromUTF8( static_cast<QString *>(data)->toUtf8().data() );
            item.setString( tstr );
        }
        break;
        
        case QMetaType::QByteArray:
        {
            QByteArray *qba = static_cast<QByteArray *>( data ); 
            Falcon::MemBuf *membuf = Falcon::MemBuf::create( this, 1, qba->size() );
            
            // we'll add a better function sooner or later to the membuf interface.
            for ( int i = 0; i < qba->size(); i ++ )
            {
                membuf->set( i, (*qba)[i] );
            }
            
            item = membuf;
        }
        break;

        // For QObjectStar and QWidgetStar we use full reflection
        case QMetaType::QObjectStar:
        case QMetaType::QWidgetStar:
        {
            QObject *obj = *static_cast<QObject **>(data);
            item = reflectObject( obj, bOwn );
        }
        break;
        
        // for everything else, we encapsulate the object in a KrossOpaque instance
        default:
            item = reflectOpaque( type, data, bOwn );
    }
}


#if 0
MetaType *KFVM::reflectIntoMetaType( const Item &source, int type )
{
    switch( source.type() )
    {
        // for plain types, we do a direct conversion
        case FLC_ITEM_NIL:
            if ( type == QMetaType::Void )
                return new MetaTypeVoidStar( QMetaType::Void, 0, false );
            return 0;
        
        case FLC_ITEM_INT:
            // thre are MANY type of int we may return
            switch( type )
            {
                case QMetaType::Int:
                    return new MetaTypeImpl< int >( (int) source.asInteger() );
                    
                case QMetaType::UInt:
                    return new MetaTypeImpl< unsigned int >( (unsigned int) source.asInteger() );
                    
                case QMetaType::Double:
                    return new MetaTypeImpl< double >( (double) source.asInteger() );
                    
                case QMetaType::Long:
                    return new MetaTypeImpl< long >( (long) source.asInteger() );
                   
                case QMetaType::Void:
                case QMetaType::LongLong:
                    return new MetaTypeImpl< Falcon::int64 >( source.asInteger() );
                   
                case QMetaType::Short:
                    return new MetaTypeImpl< short >( (short) source.asInteger() );
                   
                case QMetaType::Char: 
                    return new MetaTypeImpl< char >( (char) source.asInteger() );
                
                case QMetaType::ULong: 
                    return new MetaTypeImpl< unsigned long >( (unsigned long) source.asInteger() );
                    
                case QMetaType::ULongLong: 
                    return new MetaTypeImpl< Falcon::uint64 >( (Falcon::uint64) source.asInteger() );

                case QMetaType::UShort: 
                    return new MetaTypeImpl< unsigned short >( (unsigned short) source.asInteger() );

                case QMetaType::UChar: 
                    return new MetaTypeImpl< unsigned char >( (unsigned char) source.asInteger() );
                
                case QMetaType::Float: 
                    return new MetaTypeImpl< float >( (float) source.asInteger() );
            }
            return 0;
            
        
        case FLC_ITEM_NUM:
            switch( type )
            {
                case QMetaType::Int:
                    return new MetaTypeImpl< int >( (int) source.asNumeric() );
                    
                case QMetaType::UInt:
                    return new MetaTypeImpl< unsigned int >( (unsigned int) source.asNumeric() );
                    
                case QMetaType::Void:
                case QMetaType::Double:
                    return new MetaTypeImpl< double >( (double) source.asNumeric() );
                    
                case QMetaType::Long:
                    return new MetaTypeImpl< long >( (long) source.asNumeric() );
                   
                case QMetaType::LongLong:
                    return new MetaTypeImpl< Falcon::int64 >( source.asNumeric() );
                   
                case QMetaType::Short:
                    return new MetaTypeImpl< short >( (short) source.asNumeric() );
                   
                case QMetaType::Char: 
                    return new MetaTypeImpl< char >( (char) source.asNumeric() );
                
                case QMetaType::ULong: 
                    return new MetaTypeImpl< unsigned long >( (unsigned long) source.asNumeric() );
                    
                case QMetaType::ULongLong: 
                    return new MetaTypeImpl< Falcon::uint64 >( (Falcon::uint64) source.asNumeric() );

                case QMetaType::UShort: 
                    return new MetaTypeImpl< unsigned short >( (unsigned short) source.asNumeric() );

                case QMetaType::UChar: 
                    return new MetaTypeImpl< unsigned char >( (unsigned char) source.asNumeric() );
                
                case QMetaType::Float: 
                    return new MetaTypeImpl< float >( (float) source.asNumeric() );
            }
            return 0;
        
        case FLC_ITEM_STRING:
            if ( type == QMetaType::Void || type == QMetaType::QString )
            {
                Falcon::AutoCString fstring( *source.asString() );
                MetaTypeImpl<QString> *qs = new MetaTypeImpl< QString >;
                QString *qstring = (QString *) qs->toVoidStar();
                qstring->fromUtf8( fstring.c_str() );
                return qs;
            }
            else if( type == QMetaType::QChar )
            {
                MetaTypeImpl<QChar> *qc = new MetaTypeImpl< QChar >;
                QChar &qchar = (QChar *) qc->toVoidStar();
                if( source->asString()->length() > 0 )
                    qchar = source.asString()->getCharAt(0);
                else
                    qchar = 0;
                
                return qc;
            }
            return 0;
            
        case FLC_ITEM_MEMBUF:
            if ( type == QMetaType::Void || type == QMetaType::QByteArray )
            {
                Falcon::MemBuf *membuf = source.asMemBuf();
                //TODO: consider multibyte membufs.
                MetaTypeImpl<QByteArray> *qba = new MetaTypeImpl< QByteArray >;
                QByteArray &qbuffer = *(QByteArray *) qs->toVoidStar();
                unsigned size = membuf->length();
                qbuffer->resize( size );
                for ( unsigned i = 0; < size; ++i )
                {
                    qbuffer[i] = (char) membuf->get(i);
                } 
                return qs;
            }
            return 0;
        
        case FLC_ITEM_OBJECT:
            {
                CoreObject *obj = source.asCoreObject();
                if ( obj->derivedFrom( "KrossOpaque" ) )
                {
                    // KrossOpaque objects have a MetaType core.
                    MetaType *mt = (MetaType *) obj->getUserData();
                    if ( mt->typeId() == type || type == QMetaType::Void )
                    {
                        // we return a shell for our inner data.
                        return new MetaTypeVoidStar( mt->typeId(), mt->toVoidStar(), false );
                    }
                    // else we have incompatible types
                    return 0;
                }
                else if ( obj->generator()->getObjectManager() == &qobject_ptr_manager 
                          || obj->generator()->getObjectManager() == &qobject_ptr_own_manager)
                {
                    if ( type == QMetaType::QObjectStar || type == QMetaType::Void )
                    {
                        // ok, anything is fine
                        return new MetaTypeVoidStar( mt->typeId(), mt->toVoidStar(), false );
                    }
                    else if ( type == QMetaType::QWidgetStar )
                    {
                        // We must check the thing is at least coming from a qwidget
                        // We have a QPointer<QObject> in the user-data
                        QPointer<QObject> *qp = (QPointer<QObject> *) obj->getUserData();
                        if(  qp->data()->inherits( "QWidget" ) )
                        {
                            return new MetaTypeVoidStar( mt->typeId(), mt->toVoidStar(), false );
                        }
                        return 0;
                    }
                }
                 // support for (future) QT binding
                else if ( obj->derivedFrom( "QObject" ) )
                {
                    if( type == QMetaType::QObjectStar || type == QMetaType::Void )
                        return new MetaTypeVoidStar( mt->typeId(), mt->toVoidStar(), false );
                }
                else  if ( obj->derivedFrom( "QWidget" ) ) 
                {
                    QPointer<QObject> *qp = (QPointer<QObject> *) obj->getUserData();
                    if( qp->data()->inherits( "QWidget" ) )
                    {
                        return new MetaTypeVoidStar( mt->typeId(), mt->toVoidStar(), false );
                    }
                }
                
            }
            return 0;
    }
    
    return 0;
}
#endif


bool KFVM::reflectIntoMetaType( const Falcon::Item &source, MetaTypeFlex &flex, int type )
{
    switch( source.type() )
    {
        // for plain types, we do a direct conversion
        case FLC_ITEM_NIL:
            if ( type == QMetaType::Void )
            {
                flex.clear();
                return true;
            }
            return false;
            
        case FLC_ITEM_BOOL:
            if ( type == QMetaType::Void || type == QMetaType::Bool)
            {
                flex.setBool( (bool) source.isTrue() ); 
                return true;
            }
            return true;
        
        case FLC_ITEM_INT:
            // thre are MANY type of int we may return
            switch( type )
            {
                case QMetaType::Bool:
                    flex.setBool( (bool) source.isTrue() );
                    return true;
                    
                case QMetaType::Int:
                    flex.setInt( (int) source.asInteger() );
                    return true;
                    
                case QMetaType::UInt:
                    flex.setUInt( (unsigned) source.asInteger() );
                    return true;
                    
                case QMetaType::Double:
                    flex.setDouble( (double) source.asInteger() );
                    return true;
                    
                case QMetaType::Long:
                    return new MetaTypeImpl< long >( (long) source.asInteger() );
                   
                case QMetaType::Void:
                case QMetaType::LongLong:
                    flex.setLongLong( (qint64) source.asInteger() );
                    return true;
                   
                case QMetaType::Short:
                    flex.setShort( (short) source.asInteger() );
                    return true;
                   
                case QMetaType::Char: 
                    flex.setChar( (char) source.asInteger() );
                    return true;
                
                case QMetaType::ULong: 
                    flex.setULong( (unsigned long) source.asInteger() );
                    return true;
                    
                case QMetaType::ULongLong: 
                    flex.setULongLong( (quint64) source.asInteger() );
                    return true;

                case QMetaType::UShort: 
                    flex.setUShort( (unsigned short) source.asInteger() );
                    return true;

                case QMetaType::UChar: 
                    flex.setUChar( (unsigned char) source.asInteger() );
                    return true;
                
                case QMetaType::Float: 
                    flex.setFloat( (float) source.asInteger() );
                    return true;
            }
            return false;
            
        
        case FLC_ITEM_NUM:
            switch( type )
            {
                case QMetaType::Bool:
                    flex.setBool( (bool) source.isTrue() );
                    return true;
                    
                case QMetaType::Int:
                    flex.setInt( (int) source.asNumeric() );
                    return true;
                    
                case QMetaType::UInt:
                    flex.setUInt( (unsigned) source.asNumeric() );
                    return true;
                    
                case QMetaType::Void:
                case QMetaType::Double:
                    flex.setDouble( (double) source.asNumeric() );
                    return true;
                    
                case QMetaType::Long:
                    return new MetaTypeImpl< long >( (long) source.asNumeric() );
                   
                case QMetaType::LongLong:
                    flex.setLongLong( (qint64) source.asNumeric() );
                    return true;
                   
                case QMetaType::Short:
                    flex.setShort( (short) source.asNumeric() );
                    return true;
                   
                case QMetaType::Char: 
                    flex.setChar( (char) source.asNumeric() );
                    return true;
                
                case QMetaType::ULong: 
                    flex.setULong( (unsigned long) source.asNumeric() );
                    return true;
                    
                case QMetaType::ULongLong: 
                    flex.setULongLong( (quint64) source.asNumeric() );
                    return true;

                case QMetaType::UShort: 
                    flex.setUShort( (unsigned short) source.asNumeric() );
                    return true;

                case QMetaType::UChar: 
                    flex.setUChar( (unsigned char) source.asNumeric() );
                    return true;
                
                case QMetaType::Float: 
                    flex.setFloat( (float) source.asNumeric() );
                    return true;
            }
            return 0;
        
        case FLC_ITEM_STRING:
            if ( type == QMetaType::Bool )
            {
                flex.setBool( (bool) source.isTrue() );
                return true;
            }
            if ( type == QMetaType::Void || type == QMetaType::QString )
            {
                Falcon::AutoCString fstring( *source.asString() );
                QString* qstring = new QString;
                *qstring = QString::fromUtf8( fstring.c_str());
                flex.setUserType( QMetaType::QString, qstring, true );
                return true;
            }
            else if( type == QMetaType::QChar )
            {
                QChar *qchar = new QChar;
                if( source.asString()->length() > 0 )
                    *qchar = source.asString()->getCharAt(0);
                else
                    *qchar = 0;
                
                flex.setUserType( QMetaType::QChar, qchar, true );
                return true;
            }
            return false;
            
        case FLC_ITEM_MEMBUF:
            if ( type == QMetaType::Void || type == QMetaType::QByteArray )
            {
                Falcon::MemBuf *membuf = source.asMemBuf();
                //TODO: consider multibyte membufs.
                QByteArray &qbuffer = *(new QByteArray);
                unsigned size = membuf->length();
                qbuffer.resize( size );
                for ( unsigned i = 0; i < size; ++i )
                {
                    qbuffer[i] = (char) membuf->get(i);
                } 
                
                flex.setUserType( QMetaType::QByteArray, &qbuffer, true );
                return true;
            }
            return false;
        
        case FLC_ITEM_OBJECT:
            {
                if( type == QMetaType::Bool ) {
                    flex.setBool( true );
                    return true;
                }
                
                Falcon::CoreObject *obj = source.asObject();
                if ( obj->derivedFrom( "KrossOpaque" ) )
                {
                    // KrossOpaque objects have a MetaType core.
                    MetaType *mt = (MetaType *) obj->getUserData();
                    if ( mt->typeId() == type || type == QMetaType::Void )
                    {
                        // we return a shell for our inner data.
                        flex.setUserType( mt->typeId(), mt->toVoidStar(), false );
                        return true;
                    }
                    // else we have incompatible types
                    return false;
                }
                else if ( obj->generator()->getObjectManager() == &qobject_ptr_manager )
                {
                    // We must check the thing is at least coming from a qwidget
                    // We have a OwnerPointer (extended QPointer) in the user-data
                    OwnerPointer *qp = (OwnerPointer *) obj->getUserData();
                    
                    if ( type == QMetaType::QObjectStar || type == QMetaType::Void )
                    {
                        // ok, anything is fine
                        flex.setUserType( QMetaType::QObjectStar, qp->data(), false );
                        return true;
                    }
                    else if ( type == QMetaType::QWidgetStar )
                    {
                        
                        if(  qp->data()->inherits( "QWidget" ) )
                        {   
                            flex.setUserType( QMetaType::QWidgetStar, qp->data(), false );
                            return true;
                        }
                        return 0;
                    }
                }
                /*
                // support for (future) QT binding
                else if ( obj->derivedFrom( "QObject" ) )
                {
                    //TODO
                }
                else  if ( obj->derivedFrom( "QWidget" ) ) 
                {
                    //TODO
                }
                */
                
            }
            return false;
    }
    
    return false;
}


bool KFVM::isReflectionPossible( const Falcon::Item &source, int type )
{

    switch( source.type() )
    {
        case FLC_ITEM_NIL:
            return type == QMetaType::Void;
        
        case FLC_ITEM_BOOL:
            return type == QMetaType::Bool;
        
        case FLC_ITEM_NUM:
        case FLC_ITEM_INT:
            return type == QMetaType::Bool ||
                   type == QMetaType::Int ||
                   type == QMetaType::UInt ||
                   type == QMetaType::Double ||
                   type == QMetaType::Long ||
                   type == QMetaType::LongLong ||
                   type == QMetaType::Short ||
                   type == QMetaType::Char ||
                   type == QMetaType::ULong ||
                   type == QMetaType::ULongLong ||
                   type == QMetaType::UShort ||
                   type == QMetaType::UChar ||
                   type == QMetaType::Float;
        
        case FLC_ITEM_STRING:
            return  type == QMetaType::Bool ||
                    type == QMetaType::QString ||
                    type == QMetaType::QByteArray ||
                    type == QMetaType::QChar;
            
        case FLC_ITEM_OBJECT:
            // In Falcon, objects are always true
            if ( type == QMetaType::Bool )
                return true;
                
            // we know only objects derived from KrossOpaque or from QObject.
            // However, we don't provide the reflection of the inheritance (if
            // the reflection is dynamic).
            // So, we must determine the handler of the object, and see if it
            // mathes one of our object handlers
            {
                Falcon::CoreObject *obj = source.asObject();
                if ( obj->derivedFrom( "KrossOpaque" ) )
                {
                    // KrossOpaque objects have a MetaType core.
                    MetaType *mt = (MetaType *) obj->getUserData();
                    return mt->typeId() == type;
                }
                else if ( obj->generator()->getObjectManager() == &qobject_ptr_manager )
                {
                    if ( type == QMetaType::QObjectStar )
                    {
                        // ok, anything is fine
                        return true;
                    }
                    else if ( type == QMetaType::QWidgetStar )
                    {
                        // We must check the thing is at least coming from a qwidget
                        // We have a QPointer<QObject> in the user-data
                        QPointer<QObject> *qp = (QPointer<QObject> *) obj->getUserData();
                        return qp->data()->inherits( "QWidget" );
                    }
                }
                // support for (future) QT binding
                else if ( obj->derivedFrom( "QObject" ) )
                    return type == QMetaType::QObjectStar;
                else  if ( obj->derivedFrom( "QWidget" ) )
                    return type == QMetaType::QObjectStar||type == QMetaType::QWidgetStar;
                
                return false;
            }
            break;
            
        case FLC_ITEM_MEMBUF:
            return type == QMetaType::QByteArray;
    }
    
    return false;
}


Falcon::CoreObject *KFVM::reflectOpaque( int type, void *data, bool own )
{
    MetaType* mi = new MetaTypeVoidStar( type, data, own );
    
    // Create an instance of the KrossOpaque class
    Falcon::CoreObject *ko_obj = d->m_kopaque->createInstance();
    
    // feed the correct data into it
    ko_obj->setUserData( mi );
    
    return ko_obj;
}


//===========================================================0
// The reflectors.
//

// Initialize the new instance.
static void init_reflect_object_func( Falcon::VMachine *vm )
{
    // get the data from the symbol of the constructor and set it as our user data.
    Falcon::CoreObject *self = vm->self().asObject();
    Falcon::ExtFuncDef *def = vm->currentSymbol()->getExtFuncDef();
    
    // The dynamic type manager has recorded the meta type here.
    long metaType = (long) def->extra();
    if ( metaType == 0 )
    {
        vm->raiseModError( new Falcon::CodeError( 
            Falcon::ErrorParam( 5000, __LINE__ ).
            desc( "Not enough dynamic information to create this object." ).
            origin( Falcon::e_orig_runtime ).
            extra( self->instanceOf()->name() ) ) );
        return;
    }
    
    // we know we can be called only after QObjects, so our handler is a QObjectPtrManager
    self->setUserData( new OwnerPointer( (QObject *)QMetaType::construct( metaType ), true ) );
}

// Calls a reflected method
static void reflect_object_method( Falcon::VMachine *vm )
{
    #ifdef KROSS_FALCON_VM_DEBUG
        krossdebug( QString("reflect_object_method - begin") );
    #endif

    // maximum size allowed by QT dynamic calls.
    #define KROSS_DYNCALLS_MAXARGS 11
    
    Falcon::CoreObject *self = vm->self().asObject();
    OwnerPointer *ptr = (OwnerPointer*) self->getUserData();
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
    
    // get the extra data of this call (our metamethod with id).
    Kross::QMetaMethodWithID &method = *static_cast<Kross::QMetaMethodWithID*>( 
        vm->currentSymbol()->getExtFuncDef()->extra() );
    
    const QList<QByteArray>& paramTypes = method.parameterTypes();
    int paramCount = paramTypes.size();
    Q_ASSERT( paramCount < KROSS_DYNCALLS_MAXARGS );
    
    // Now we got to check our types and be sure they can be reflected from falcon.
    bool matches = true;
    
    QVarLengthArray<MetaTypeFlex> flexyArgs;
    QVarLengthArray<void*> voidArgs;
    
    // First: do our method call matches the declaration?
    Falcon::uint32 pcount = vm->paramCount();
    if ( pcount != (Falcon::uint32) paramCount )
    {
        // no, wrong number of parameters
        matches = false;
    }
    else {
        // try to flexyfy all the parameters.
        flexyArgs.resize( paramCount + 1 );
        voidArgs.resize( paramCount + 1 );
        
        QList<QByteArray>::const_iterator iter;
        int pi = 1;
        for ( iter = paramTypes.constBegin(); iter != paramTypes.constEnd(); ++iter)
        {
            // this is the flexyfication that may fail
            // take the QMetaType id.
            int metaId = QMetaType::type( *iter );
            if ( metaId == 0 )
            {
                // invalid type name? -- should not happen, but...
                matches = false;
                break;
            }
            
            if (! kvm->reflectIntoMetaType( *kvm->param(pi-1), flexyArgs[pi], metaId ) )
            {
                matches = false;
                break;
            }
            
            // otherwise, since we're here, put in also the void*
            voidArgs[pi] = flexyArgs[pi].toVoidStar();
        }
    }
    
    // the return metatype will be needed now and later.
    int returnMetaType = 0;
    
    // if we made it with the parameters, try also with the return value
    if ( matches )
    {
        // now create the return value, if needed.
        if ( method.typeName() == 0 || method.typeName()[0] == 0 )
        {
            // no return value.
            voidArgs[0] = 0;
            // flexyArgs is already ok
        }
        else 
        {
            // Try to create the space needed for the return value
            returnMetaType = QMetaType::type( method.typeName() );
            Q_ASSERT( returnMetaType != 0 );
            if ( returnMetaType == 0 )
            {
                // again, shouldn't happen, but...
                matches = false;
            }
            else {
                // create a dynamic instance of this type.
                //TODO: IMHO it would be better to use a wide enough stack space.
                voidArgs[0] = QMetaType::construct( returnMetaType );
            }
        }
    }

    // if we can't make it, report error.
    if ( ! matches )
    {
        // Show also the signature, so the user knows what to do to fix the program.
        vm->raiseModError( new Falcon::ParamError( 
            Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
            origin( Falcon::e_orig_runtime ).
            extra( method.signature()) ) );
        return;
        
        // we have nothing to destroy; everything is handled by the unrolling
    }
    
    
    // and perform the call on the object.
    // The id of the method is in our "method" var, and the object is in the pointer.
    QObject *obj = ptr->data();
    int r = obj->qt_metacall(QMetaObject::InvokeMetaMethod, method.m_id, &voidArgs[0]);
    #ifdef KROSS_FALCON_VM_DEBUG
        krossdebug( QString("Call result nr=%1").arg(r) );
    #else
        Q_UNUSED(r);
    #endif
    
    // if there is a return value, we must convert it and return to the vm.
    if( returnMetaType != 0 )
    {
        #ifdef KROSS_FALCON_VM_DEBUG
            krossdebug( QString("Return metatype: %1").arg(returnMetaType) );
        #else
            Q_UNUSED(r);
        #endif
        // TODO: Is there a way to know if we have ownership?
        kvm->reflectFromMetaType( returnMetaType, voidArgs[0], kvm->regA(), false );
        // reflectFrom is always succesful
    }
    else {
        // not necessary, just for reference.
        kvm->regA().setNil();
    }
}


// Read a property from the QObject and send it to our VM.
static void reflect_object_prop_from(
        Falcon::CoreObject *self, 
        void *user_data, 
        Falcon::Item &property, 
        const Falcon::PropEntry& entry )
{
    #ifdef KROSS_FALCON_VM_DEBUG
        Falcon::AutoCString cstr_key( *entry.m_name );
        krossdebug( QString("reflect_object_prop_from: reflecting property \"%1\"").arg(cstr_key.c_str()) );
    #endif


    OwnerPointer *ptr = (OwnerPointer*) user_data;
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
static void reflect_object_prop_to(
        Falcon::CoreObject *self, 
        void *user_data, 
        Falcon::Item &property, 
        const Falcon::PropEntry& entry )
{
    #ifdef KROSS_FALCON_VM_DEBUG
        Falcon::AutoCString cstr_key( *entry.m_name );
        krossdebug( QString("reflect_object_prop_to: reflecting property \"%1\"").arg(cstr_key.c_str()) );
    #endif
            
    OwnerPointer *ptr = (OwnerPointer*) user_data;
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
