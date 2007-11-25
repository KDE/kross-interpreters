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
    };
}

#endif
