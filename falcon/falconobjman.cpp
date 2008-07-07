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

#include "falconobjman.h"
#include <kross/core/metatype.h>
#include <QObject>
#include <QMetaType>
#include <QVarLengthArray>
#include "metatypeflex.h"
#include "falconkfvm.h"

using namespace Kross;

// ==========================================================
// Static instances of object managers
// Actually, they serve solely as function vectors.
//
namespace Kross {
    QObjectPtrManager qobject_ptr_manager;
    QObjectPtrManager qobject_ptr_obj_manager;
    KOPtrManager ko_ptr_manager;
}

//===========================================================

void *QObjectPtrManager::onInit( Falcon::VMachine * )
{
    // Better let the init method to handle this for now.
    return 0;
}

void QObjectPtrManager::onGarbageMark( Falcon::VMachine *, void * ) 
{
}

void QObjectPtrManager::onDestroy( Falcon::VMachine *, void *user_data )
{
    // destroy the pointer.
    OwnerPointer *ptr = (OwnerPointer*) user_data;
    // the pointer knows if it has the ownership of the inner data.
    delete ptr;
}
    
void *QObjectPtrManager::onClone( Falcon::VMachine *, void *user_data )
{
    OwnerPointer *ptr = (OwnerPointer *) user_data;
    
    // If disabled, let the owner to raise 
    if ( ptr->isNull() )
        return 0;
    
    // Otherwise, return 
    return new OwnerPointer( ptr->data(), false );
}

//===========================================================

void *KOPtrManager::onInit( Falcon::VMachine * )
{
    // Better let the init method to handle this for now.
    return 0;
}

void KOPtrManager::onGarbageMark( Falcon::VMachine *, void * ) 
{
}

void KOPtrManager::onDestroy( Falcon::VMachine *, void *user_data ) 
{
    MetaType *md = (MetaType*) user_data;
    // Delete just our reference to the meta-data.
    // Notice: the virtual destructor of MetaDataVoidStar will dispose
    // the object if owned.
    delete md;
}
    
void *KOPtrManager::onClone( Falcon::VMachine *, void *user_data ) 
{
    MetaType *md = (MetaType*) user_data;
    
    // Perform a complete cloning
    return QMetaType::construct( md->typeId(), md->toVoidStar() );
}


