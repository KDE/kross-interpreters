/***************************************************************************
 * metatypeflex.h
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

#ifndef KROSS_METATYPEFLEX_H
#define KROSS_METATYPEFLEX_H

#include <kross/core/metatype.h>
#include <QString>
#include <QMetaType>

namespace Kross {
    
    /**
    * Flexible metatype.
    *
    * This class extends Kross Meta Types so that it is possible
    * to create an array of pre-allocated meta-types, and then
    * use it obviating the need for heap managed memory.
    */
    class MetaTypeFlex: public MetaType
    {
    public:
        MetaTypeFlex() { m_type = QMetaType::Void; m_bOwn = false; }
        virtual ~MetaTypeFlex();
        void clear();

        void setBool( bool b ) { m_type = QMetaType::Bool; m_data.dBool = b; }
        void setChar( char c ) { m_type = QMetaType::Char; m_data.dChar = c; }
        void setUChar( unsigned char c ) { m_type = QMetaType::UChar; m_data.dUChar = c; }
        void setShort( short s ) { m_type = QMetaType::Short; m_data.dShort = s; }
        void setUShort( unsigned short s ) { m_type = QMetaType::UShort; m_data.dUShort = s; }
        void setInt( int s ) { m_type = QMetaType::Int; m_data.dInt = s; }
        void setUInt( unsigned int s ) { m_type = QMetaType::UInt; m_data.dUInt = s; }
        void setLong( long s ) { m_type = QMetaType::Long; m_data.dLong = s; }
        void setULong( unsigned long s ) { m_type = QMetaType::ULong; m_data.dULong = s; }
        void setLongLong( qint64 s ) { m_type = QMetaType::LongLong; m_data.dLongLong = s; }
        void setULongLong( quint64 s ) { m_type = QMetaType::ULongLong; m_data.dULongLong = s; }
        void setFloat( float s ) { m_type = QMetaType::Float; m_data.dFloat = s; }
        void setDouble( double s ) { m_type = QMetaType::Double; m_data.dDouble = s; }
        
        void setQString( const QString &qs );
        
        void setUserType( int type, void *data, bool bOwn = false ) 
        { 
            m_type = type; 
            m_data.dVoidStar = data; 
            m_bOwn = bOwn;
        }
        
        virtual int typeId() { return m_type; }
        virtual void* toVoidStar();
        
    private:
        int m_type;
        bool m_bOwn;
        
        union {
            char dChar;
            unsigned char dUChar;
            short dShort;
            unsigned short dUShort;
            int dInt;
            unsigned int dUInt;
            long dLong;
            unsigned long dULong;
            qint64 dLongLong;
            quint64 dULongLong;
            float dFloat;
            double dDouble;
            void *dVoidStar;
            bool dBool;
        } m_data;
    };
}

#endif
