/***************************************************************************
 * falconkfvm.h
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

#ifndef KROSS_FALCONKFVM_H
#define KROSS_FALCONKFVM_H

#include "falconconfig.h"
#include <kross/core/krossconfig.h>
#include <falcon/vm.h>

#include <QObject>
#include <QMetaMethod>
#include <kross/core/script.h>
#include <kross/core/metatype.h>

namespace Kross {
    
    class MetaTypeFlex;
    class KVFMPrivate;
    
    /** 
    * Kross - Falcon Virtual Machine.
    *
    * This specialization of the Falcon Virtual Machine is used by Kross binding.
    *
    * It automathises stream redirection, error redirection, and mainly item-to-QTVariant
    * conversion. In fact, Falcon requires deep item initialization to be performed under
    * a VM hood, so that memory can be allocated in the correct garbage collector and 
    * conversion errors can be raised and notified to the correct script.
    *
    * This class is never published by the plugin; it's just used in d pointers of other
    * classes.
    */
    
    class KFVM: public Falcon::VMachine
    {
    public:
        /** 
        * Constructor.
        *
        * The Action pointer can be zero; it's used just for reference to search for
        * objects during dynamic link requests.
        *
        */
        explicit KFVM( Action *action );
        
        virtual ~KFVM();
        
        /// Reimplements dynamic symbol requests from the Falcon VM.
        virtual bool linkSymbolDynamic( const Falcon::String &name, Falcon::SymModule &symdata );
        
        /**
        * Converts a Falcon item into a coresponding variant.
        * If the conversion is possible, the variant is filled with a copy of the data
        * in the item, else the VM raises an error.
        * \param item the Falcon::Item to be converted
        * \param variant the target QVariant
        * \return true if operation was possible, false in case of error.
        */
        bool itemToVariant( const Falcon::Item &item, QVariant &variant );
        
        /**
        * Converts a Falcon item into a coresponding variant.
        * If the conversion is possible, the item is filled with a VM relevant value
        * copied (or deeply copied) from the variant, else the VM raises an error.
        * \param item the target Falcon::Item
        * \param variant QVariant that must be converted.
        * \return true if operation was possible, false in case of error.
        */
        bool variantToItem( const QVariant &variant, Falcon::Item &item ); 
        
        /**
        * Creates an instance of the Falcon representation of a QObject.
        * \param qo qobject to be converted
        * \return a CoreObject holding a Falcon "KrossObject" instance.
        */
        Falcon::CoreObject *QObjectToKrossObject( QObject *qo ) const;
        
        /**
        * Creates a dinamically allocated Falcon garbage string from a QString.
        * This class respect international transcoding between QT and Falcon strings
        * by using the respective to and from UTF8 encoding.
        * \param qstring metaclass info to be converted
        * \return a newly allocated garbage string.
        */
        Falcon::String *QStringToString( const QString &qstring );
        
        /**
        * Reflect a generic QT registered type into Falcon VM.
        *
        * This method determines if the given data can be fully dynamically reflected
        * or must be encapsulated in an opaque type. 
        * 
        * If the meta type is qobject or qwidget, then full reflection is performed, and
        * an existing matching class is searched (or created if not found); then, the
        * inner QObjectPtrManager or the QObjectPtrOwnManager are delegated to manage
        * the lifetime of the object, and reflected method can be used.
        *
        * If the item is not a QObject type, two things may happen. If the meta-type is
        * refers to a C POD or in other known items, as QVariant lists, it is directly
        * translated into Falcon types. If it's an unknwon class, it is searched in the
        * VM (an external module may have provided it), and if not found an instance
        * of KrossOpaque is created. That class is meant to provide the scripts with
        * minimal dynamic information about what they contain, and to be provided
        * as parameters for other methods: the data they opaquely incapsulate will be
        * anyhow unpacked and checked for dynamic cast availablility against the called
        * method parameters.
        *
        * The conversion cannot fail.
        *
        * \param type The of the data to be converted.
        * \param data The data to be converted.
        * \param target The item where to store the converted value.
        * \param bOwn Wether ownership belongs to the VM (data created by dynamic calls) or
        *             to someone else (data actually stored in other places).
        */
        void reflectFromMetaType( int type, void *data, Falcon::Item &target, bool bOwn );
        
        /**
        * Reflect a generic QT registered type into Falcon VM.
        *
        * Version using a Kross MetaType pointer for handlier operations.
        */
        
        void reflectFromMetaType( MetaType *data, Falcon::Item &target, bool bOwn )
        {
            reflectFromMetaType( data->typeId(), data->toVoidStar(), target, bOwn );
        }
        
        /**
        * Reflect a Falcon item into a Kross::MetaType.
        *
        * This method reverses \a reflectFromMetaType. The Falcon item is turned into
        * data known by QT system, encapsulated into a Kross::MetaType for easier manipulation.
        *
        * The conversion can fail if the item isn't a KrossFalcon or QT item matching the
        * item that is provided in input (in example, if a method is called with a Falcon
        * object that has nothing to do with QT or Kross).
        *
        * In that case, the function returns false.
        * 
        * It is possible to check if a conversion is possible in advance 
        * by calling isReflectionPossible().
        *
        * \param source the item to be converted.
        * \param flex a Flexy MetaType that will be configured (without ownership, unless converting a string).
        * \param type the metatype into wich the item should be converted; pass 
        *        QMetaType::Void for a default conversion.
        * \return True on success, false on failure.
        */
        bool reflectIntoMetaType( const Falcon::Item &source, MetaTypeFlex &flex, int type = QMetaType::Void );
        // Old form:
        //MetaType *reflectIntoMetaType( const Falcon::Item &source, int type );
    
        /**
        * Returns true if the reflection from a Falcon Item into a MetaType 
        * is possible.
        * \param source The item that should be converted.
        * \param type The metatype in which the item should be converted.
        * \return true if the conversion is possible.
        */
        bool isReflectionPossible( const Falcon::Item &source, int type );
        
        /**
        * Reflect a class into Falcon VM.
        *
        * This method creates a Falcon Class using a given object instance, through its
        * QMeta* interface. The class is given a q_object_manager, and is meant to store
        * obj AND any other object alike in future. For this reason the class is given
        * the "well known symbol" attribute.
        *
        * If a well known symbol with that name already exists, that is returned instead.
        *
        * If obj is not given (zero), the method uses QMetaType subsystem to obtain an
        * handle to the desired type; if that type can be cast to a QObject, then a
        * dummy instance is created to extract its meta data and then destroyed. If the
        * name is not known as a registered QT type, or if that registered type can't be
        * cast to a QObject, the method returns zero.
        *
        * \param clsName The name of the class to be created.
        * \param obj An optional QObject that will be used to create the class template.
        * \return 0 on failure, the new class (already turned into a WKS) on return.
        */
        Falcon::CoreClass *reflectClass( QObject *obj=0 );
        
        /**
        * Reflects a signle QObject instance into a Falcon Symbol.
        *
        * Retreives or creates the class in QObject via reflectClass, and then 
        * creates a singleton instance that implements the class.
        *
        * The singleton instance is initialized with an appropriate core object,
        * and the returned symbol can be used to access it by name.
        *
        * The ownership of the symbol stays to Kross (outside the script), as
        * singleton objects are created from direct requests to access named objects
        * provided to the script action from otuside.
        *
        * \param objName The name under which the object should be known to the VM.
        * \param obj The QObject to be reflected.
        * \return The symbol hosting the singleton object.
        */
        Falcon::Symbol *reflectSingleton( const Falcon::String &objName, QObject *obj );
        
        /**
        * Reflects an anonymous QObject instance into a Falcon item.
        *
        * Retreives or creates the class in QObject via reflectClass, and then
        * returns an anonymous instance of the reflected QObject item.
        * 
        * The Reflected object ownership may be given to the CoreObject, that will
        * dispose it when it's destroyed by the GC. The QObject may also be autonomously
        * destroyed while the reflector returned by this method is still in use; the
        * reflection is performed through a smart QT pointer to the object that gets
        * informed of inner object destruction; if trying to access the reflection
        * methods when the inner object is gone, the VM will raise an access error
        * and the situation will be handled cleanly.
        *
        * If the ownership is not given to the CoreObject wrapper, the QT object is
        * simply dropped when the CoreObject is collected.
        *
        * \param obj The QObject to be reflected.
        * \param bOwn True to have the QObject disposed when the CoreObject is collected.
        * \return The anonymous instance of the Falcon object reflecting the QT object.
        */
        Falcon::CoreObject *reflectObject( QObject *obj, bool bOwn = true );
        
        /**
        * Reflects an opaque type into a KrossOpaque object.
        *
        * KrossOpaque objects are objects storing a MetaData type and minimal
        * script-level information to help the scripters in using them.
        * 
        * They wrap user defined QT meta types, or all the QT classes not derived
        * from QObject if the QT binding is not loaded.
        *
        * The Kross::MetaType instance held inside is owned by the object; if 
        * own is true
        */ 
        Falcon::CoreObject *reflectOpaque( int type, void *data, bool own );
        
    private:
        /// Private d-pointer class.
        KVFMPrivate* const d;
    };
    
    /// We need this class to record meta methods (we need their parameter types) and their IDs.
    class QMetaMethodWithID: public QMetaMethod {
    public:
        int m_id;
        
        QMetaMethodWithID( int id, const QMetaMethod &other ):
            QMetaMethod( other ),
            m_id( id )
        {}
    };
    
}

#endif
