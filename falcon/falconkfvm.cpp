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

#include <QChar>
#include <QString>
#include <QVariant>
#include <QMetaType>

namespace Kross {
    
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
                Falcon::DictIterator *iter = dict->begin();
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
                    skey.fromUtf8( ctgt.c_str() );
                    
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
                    Falcon::Item *i_x = cobj->getProperty( "x" );
                    Falcon::Item *i_y = cobj->getProperty( "y" );
                    Q_ASSERT( i_x != 0 & i_y != 0 );
                    // have we a numeric instance? -- return a QPointF 
                    if ( i_x->isNumeric() || i_y->isNumeric() )
                    {
                        variant.setValue( QPointF( i_x->forceNumeric(), i_y->forceNumeric() ) );
                    }
                    else {
                        variant.setValue( QPoint( i_x->forceInteger(), i_y->forceInteger() ) );
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
                    value->fromUTF8( qliter->toUtf8() );
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
                    key->fromUTF8( qmiter.key().toUtf8() );
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
                raiseError( new Falcon::ParamError( Falcon::ErrorParam( Falcon::e_param_type ).
                     origin( Falcon::e_orig_vm ).
                     extra( "Can't convert QVariant into Falcon::Item" ) )
                  );
                  
                  return false;
        }
        
        return true;
    }

}

