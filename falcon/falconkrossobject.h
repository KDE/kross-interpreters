/***************************************************************************
 * falconkrossobject.h
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

#ifndef KROSS_FALCONKROSSOBJECT_H
#define KROSS_FALCONKROSSOBJECT_H

#include <falcon/userdata.h>
#include <falcon/module.h>
#include <QObject>


namespace Kross {
    
    /**
    * Carrier class for QObjects.
    * This class is needed to provide storage and reflectivity for
    * QObjects stored in Falcon objects for script manipulation.
    *
    * This class provides safe access to the stored QObject. In case
    * it gets destroyed while in the carrier, the object is zeroed
    * and access will generate an exception in the calling VM.
    *
    * This class hasn't a d-> pointer as it IS the d pointer for the
    * CoreObject class...
    */
    class FalconCarrier: public Falcon::FalconData
    {
        QObject *m_carried;
        
    public:
        /**
        * Creates the carrier
        */
        FalconCarrier( QObject *data ):
            m_carried( data )
        {}
        
        /** Returns the carried object */
        QObject *qobject() const {
            return m_carried;
        }
        
        virtual Falcon::FalconData *clone() const;
        virtual void gcMark( Falcon::VMachine* );
    };
    
    /**
    * Declares the KrossObject class which interfaces QObjects to the scripts.
    */
    void DeclareFalconKrossObject( ::Falcon::Module *self );
}



#endif
