/***************************************************************************
 * metatypeflex.cpp
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

#include "metatypeflex.h"

namespace Kross {

MetaTypeFlex::~MetaTypeFlex()
{
    #ifdef KROSS_METATYPE_DEBUG
        krossdebug( QString("MetaTypeFlex Dtor typeid=%1 typename=%2 owner=%3").arg(m_type).arg(typeid(m_ptr).name()).arg(m_owner) );
    #endif
    
    // destroy won't touch PODs, so we can even pass rubbish to it.
    if( m_bOwn && m_type != QMetaType::Void )
        QMetaType::destroy(m_type, m_data.dVoidStar);
}


void MetaTypeFlex::clear()
{
    #ifdef KROSS_METATYPE_DEBUG
        krossdebug( QString("MetaTypeFlex clear typeid=%1 typename=%2 owner=%3").arg(m_type).arg(typeid(m_ptr).name()).arg(m_owner) );
    #endif
    
    // destroy won't touch PODs, so we can even pass rubbish to it.
    if( m_bOwn && m_type != QMetaType::Void )
        QMetaType::destroy(m_type, m_data.dVoidStar);
    
    m_type = QMetaType::Void;
}

void* MetaTypeFlex::toVoidStar()
{
    switch( m_type )
    {
        case QMetaType::Void: return 0;
        
        case QMetaType::Bool: return (void *) &m_data.dBool;
        case QMetaType::Char: return (void *) &m_data.dChar;
        case QMetaType::UChar: return (void *) &m_data.dUChar;
        case QMetaType::Short: return (void *) &m_data.dShort;
        case QMetaType::UShort: return (void *) &m_data.dUShort;
        case QMetaType::Int: return (void *) &m_data.dInt;
        case QMetaType::UInt: return (void *) &m_data.dUInt;
        case QMetaType::Long: return (void *) &m_data.dLong;
        case QMetaType::ULong: return (void *) &m_data.dULong;
        case QMetaType::LongLong: return (void *) &m_data.dLongLong;
        case QMetaType::ULongLong: return (void *) &m_data.dULongLong;
        case QMetaType::Float: return (void *) &m_data.dFloat;
        case QMetaType::Double: return (void *) &m_data.dDouble;
    }

    return m_data.dVoidStar;
}

void MetaTypeFlex::setQString( const QString &qs )
{ 
    m_type = QMetaType::QString; 
    m_data.dVoidStar = new QString(qs);
    m_bOwn = true;
}


}

