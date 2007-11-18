/***************************************************************************
 * falconmetaobject.h
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

#ifndef FALCONMETAOBJECT_H

#include <falcon/module.h>
#include <falcon/cobject.h>
#include <QMetaObject>

namespace Kross
{
    /**
    * Creates the reflected "QMetaObject"
    */
    void DeclareFalconMetaObject( Falcon::Module *self );

    /**
    * Reflects QMetaObject in Falcon objects
    */
    class FalconMetaObjectData: public Falcon::UserData
    {
        const QMetaObject *m_metaObject;
        
    public:
        FalconMetaObjectData( const QMetaObject *base );
        
        virtual Falcon::UserData *clone();
        const QMetaObject &metaObject() const { return *m_metaObject; }
    };
}

#endif
