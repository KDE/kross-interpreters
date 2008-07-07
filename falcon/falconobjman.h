/***************************************************************************
* falconobjman.h
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

#ifndef FALCON_KROSS_OBJECT_MANAGER
#define FALCON_KROSS_OBJECT_MANAGER

#include <falcon/objectmanager.h>
#include <QPointer>
#include <QObject>

namespace Kross 
{

    /** 
    * Object manager for QT objects.
    *
    * This is the object manager for reflected QObjects.
    *
    * Data managed by this object manager is of type OwnerPointer, which
    * derives from the QPoinber smart pointers.
    */
    class QObjectPtrManager: public Falcon::ObjectManager
    {
        virtual void *onInit( Falcon::VMachine * );
        virtual void onGarbageMark( Falcon::VMachine *, void * );
        virtual void onDestroy( Falcon::VMachine *, void *user_data );
        virtual void *onClone( Falcon::VMachine *, void *user_data );
    };
    
    
    /** 
    * Object manager for Kross opaque objects.
    *
    * This object manager is used for reflected items that cannot
    * be manipulated by the script in the KrossOpaque class.
    *
    * This mangaer handles Kross:MetaType as inner user_data objects;
    * this allows for a finer control of what meta data is inside
    * this opaque object.
    *
    * This manager won't destroy the object when the Falcon object
    * is garbaged.
    */
    
    class KOPtrManager: public Falcon::ObjectManager
    {
        virtual void *onInit( Falcon::VMachine * );
        virtual void onGarbageMark( Falcon::VMachine *, void * );
        virtual void onDestroy( Falcon::VMachine *, void *user_data );
        virtual void *onClone( Falcon::VMachine *, void *user_data );
    };
    

    /**
    * Smart pointers having ownership for QObjects.
    *
    * This class stores a smart pointer to QObject class, and
    * it also records ownership of the pointed data.
    * 
    * If they are created with given ownership, they also delete
    * the inner data on destruction.
    *
    * Notice that the destructor is not virtual, and for the
    * behavior to be correct, it is necessary to cast instances
    * of this class correctly.
    */
    class OwnerPointer: public QPointer<QObject> {
    public:
        explicit OwnerPointer( QObject *obj, bool bOwn ):
            QPointer<QObject>( obj ),
            m_bOwn( bOwn )
        {}
        
        ~OwnerPointer() {
            if ( m_bOwn )
                delete data();
        }

    private:
        bool m_bOwn;
    };
    
    /// Unique ptr manager instance.
    extern QObjectPtrManager qobject_ptr_manager;
    
    /// Unique opaque metatype ptr manager instance.
    extern KOPtrManager ko_ptr_manager;
    
}

#endif

