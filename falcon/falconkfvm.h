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

#include <QMetaClassInfo>
#include <QMetaEnum>
#include <QMetaMethod>

namespace Kross {

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
        * Creates an instance of the Falcon representation of a metaclass info.
        * \param qmi metaclass info to be converted
        * \return a CoreObject holding a Falcon "QMetaClassInfo" instance.
        */
        Falcon::CoreObject *MetaClassInfoToObject( const QMetaClassInfo &qmi );
        
        /**
        * Creates an instance of the Falcon representation of a QMetaEnum info.
        * \param qmi metaclass info to be converted
        * \return a CoreObject holding a Falcon "QMetaEnum" instance.
        */
        Falcon::CoreObject *MetaEnumToObject( const QMetaEnum &qmi );
        
        /**
        * Creates an instance of the Falcon representation of a QMetaMethod info.
        * \param qmi metaclass info to be converted
        * \return a CoreObject holding a Falcon "QMetaMethod" instance.
        */
        Falcon::CoreObject *MetaMethodToObject( const QMetaMethod &qmi );
        
        /**
        * Creates an instance of the Falcon representation of a QMetaProperty info.
        * \param qmp metaclass property to be converted
        * \return a CoreObject holding a Falcon "QMetaProperty" instance.
        */
        Falcon::CoreObject *MetaPropertyToObject( const QMetaProperty &qmp);
        
        /**
        * Creates an instance of the Falcon representation of a QMetaObject info.
        * Notice that we want here a QMetaObject pointer, which is shared across
        * all the program. We don't own it, and we suppose it's always available.
        * \param qmo metaclass object to be converted
        * \return a CoreObject holding a Falcon "QMetaObject" instance.
        */
        Falcon::CoreObject *MetaObjectToObject( const QMetaObject *qmo );
    };
}

#endif
