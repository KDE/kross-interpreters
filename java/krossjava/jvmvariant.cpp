/***************************************************************************
 * jvmvariant.cpp
 * This file is part of the KDE project
 *
 * copyright (C)2007 by Vincent Verhoeven <verhoevenv@gmail.com>
 * copyright (C)2007 by Sebastian Sauer <mail@dipe.org>
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

#include "jvmvariant.h"
#include "jvmexception.h"

using namespace Kross;

jvalue JavaType<QVariant>::toJObject(const QVariant& v, JNIEnv* env)
{
    krossdebug( QString("JavaType<QVariant>::toJObject variant.toString=%1 variant.typeid=%2 variant.typeName=%3").arg(v.toString()).arg(v.type()).arg(v.typeName()) );
    switch( v.type() ) {
#if 0
        case QVariant::Int:
            return JavaType<int>::toJObject(v.toInt());
        case QVariant::UInt:
            return JavaType<uint>::toJObject(v.toUInt());
        case QVariant::Double:
            return JavaType<double>::toJObject(v.toDouble());
        case QVariant::ByteArray:
            return JavaType<QByteArray>::toJObject(v.toByteArray());
        case QVariant::String:
            return JavaType<QString>::toJObject(v.toString());
        case QVariant::Bool:
            return JavaType<bool>::toJObject(v.toBool());
        case QVariant::StringList:
            return JavaType<QStringList>::toJObject(v.toStringList());
        case QVariant::Map:
            return JavaType<QVariantMap>::toJObject(v.toMap());
        case QVariant::List:
            return JavaType<QVariantList>::toJObject(v.toList());
        case QVariant::LongLong:
            return JavaType<qlonglong>::toJObject(v.toLongLong());
        case QVariant::ULongLong:
            return JavaType<qlonglong>::toJObject(v.toULongLong());
        case QVariant::Url:
            return JavaType<QUrl>::toJObject(v.toUrl());
        case QVariant::Size:
            return JavaType<QSize>::toJObject(v.toSize());
        case QVariant::SizeF:
            return JavaType<QSizeF>::toJObject(v.toSizeF());
        case QVariant::Point:
            return JavaType<QPoint>::toJObject(v.toPoint());
        case QVariant::PointF:
            return JavaType<QPointF>::toJObject(v.toPointF());
        case QVariant::Rect:
            return JavaType<QRect>::toJObject(v.toRect());
        case QVariant::RectF:
            return JavaType<QRectF>::toJObject(v.toRectF());
#endif
        case QVariant::Invalid: {
            krossdebug( QString("JavaType<QVariant>::toJObject variant=%1 is QVariant::Invalid. Returning Py:None.").arg(v.toString()) );
        } // fall through
        case QVariant::UserType: {
            krossdebug( QString("JavaType<QVariant>::toJObject variant=%1 is QVariant::UserType. Trying to cast now.").arg(v.toString()) );
        } // fall through
        default: {
#if 0
            if( strcmp(v.typeName(),"float") == 0 ) {
                return JavaType<double>::toJObject(v.toDouble());
            }
#endif

            if( qVariantCanConvert< QWidget* >(v) ) {
                krossdebug( QString("JavaType<QVariant>::toJObject Casting '%1' to QWidget").arg(v.typeName()) );
#if 0
                QWidget* widget = qvariant_cast< QWidget* >(v);
                if(! widget) {
                    krosswarning( QString("JavaType<QVariant>::toJObject To QWidget casted '%1' is NULL").arg(v.typeName()) );
                    return 0;
                }
                return JVMExtension::toJObject( new JVMExtension(widget) );
#endif
            }

            if( qVariantCanConvert< QObject* >(v) ) {
                krossdebug( QString("JavaType<QVariant>::toJObject Casting '%1' to QObject").arg(v.typeName()) );
#if 0
                QObject* obj = qvariant_cast< QObject* >(v);
                if(! obj) {
                    #ifdef KROSS_RUBY_VARIANT_DEBUG
                        krosswarning( QString("JavaType<QVariant>::toJObject To QObject casted '%1' is NULL").arg(v.typeName()) );
                    #endif
                    return 0;
                }
                return JVMExtension::toJObject( new JVMExtension(obj) );
#endif
            }

            //QObject* obj = (*reinterpret_cast< QObject*(*)>( variantargs[0]->toVoidStar() ));
            //PyObject* qobjectptr = PyLong_FromVoidPtr( (void*) variantargs[0]->toVoidStar() );

            krosswarning( QString("JavaType<QVariant>::toJObject Not possible to convert the QVariant '%1' with type '%2' (%3) to a VALUE.").arg(v.toString()).arg(v.typeName()).arg(v.type()) );
            JVMException::throwNullPointerException(env);
            return jvalue();
        }
    }
}

QVariant JavaType<QVariant>::toVariant(jvalue value, JNIEnv* env)
{
    krossdebug( QString("JavaType<QVariant>::toVariant") );

    //TODO JNIType
    Q_UNUSED(value);
    Q_UNUSED(env);

    return QVariant();
}

#if 0
MetaType* RubyMetaTypeFactory::create(int typeId, int metaTypeId, VALUE value)
{
    krossdebug( QString("RubyMetaTypeFactory::create typeId=%1 typeName=%2").arg(QMetaType::typeName(typeId)).arg(typeId) );
    switch(typeId) {
        case QVariant::Int:
            return new RubyMetaTypeVariant<int>(value);
        case QVariant::UInt:
            return new RubyMetaTypeVariant<uint>(value);
        case QVariant::Double:
            return new RubyMetaTypeVariant<double>(value);
        case QVariant::Bool:
            return new RubyMetaTypeVariant<bool>(value);
        case QVariant::ByteArray:
            return new RubyMetaTypeVariant<QByteArray>(value);
        case QVariant::String:
            return new RubyMetaTypeVariant<QString>(value);
        case QVariant::StringList:
            return new RubyMetaTypeVariant<QStringList>(value);
        case QVariant::Map:
            return new RubyMetaTypeVariant<QVariantMap>(value);
        case QVariant::List:
            return new RubyMetaTypeVariant<QVariantList>(value);
        case QVariant::LongLong:
            return new RubyMetaTypeVariant<qlonglong>(value);
        case QVariant::ULongLong:
            return new RubyMetaTypeVariant<qulonglong>(value);
        case QVariant::Url:
            return new RubyMetaTypeVariant<QUrl>(value);
        case QVariant::Size:
            return new RubyMetaTypeVariant<QSize>(value);
        case QVariant::SizeF:
            return new RubyMetaTypeVariant<QSizeF>(value);
        case QVariant::Point:
            return new RubyMetaTypeVariant<QPoint>(value);
        case QVariant::PointF:
            return new RubyMetaTypeVariant<QPointF>(value);
        case QVariant::Rect:
            return new RubyMetaTypeVariant<QRect>(value);
        case QVariant::RectF:
            return new RubyMetaTypeVariant<QRectF>(value);
        case QVariant::Invalid: // fall through
        case QVariant::UserType: // fall through
        default: {
            if( JVMExtension::isJVMExtension(value) ) {
                #ifdef KROSS_RUBY_VARIANT_DEBUG
                    krossdebug( QString("RubyMetaTypeFactory::create VALUE with typeId '%1' is a JVMExtension object").arg(typeId) );
                #endif
                JVMExtension* extension;
                Data_Get_Struct(value, JVMExtension, extension);
                Q_ASSERT(extension);
                QObject* object = extension->object();
                if(! object) {
                    krosswarning("RubyMetaTypeFactory::create QObject is NULL.");
                    return 0;
                }
                return new MetaTypeVoidStar( typeId, object, false );
            }
            if( TYPE(value) == T_NIL ) {
                #ifdef KROSS_PYTHON_VARIANT_DEBUG
                    krossdebug( QString("RubyMetaTypeFactory::create VALUE is T_NIL. Create empty type '%1'").arg(metaTypeId) );
                #endif
                void* ptr = QMetaType::construct(metaTypeId, 0);
                return new MetaTypeVoidStar( metaTypeId, ptr, false );
            }
            //QVariant v = JavaType<QVariant>::toVariant(object);
            //krossdebug( QString("RubyVariant::create Converted VALUE '%1' with type '%2 %3' to QVariant with type '%4 %5'").arg(object.as_string().c_str()).arg(typeName).arg(typeId).arg(v.toString()).arg(v.typeName()) );
            //if(typeId == QVariant::Invalid) return new RubyVariantImpl<void>();
            //return new RubyVariantImpl<QVariant>(v);
            krosswarning( QString("RubyMetaTypeFactory::create Not possible to convert the VALUE to QVariant with '%1' and metaid '%2'").arg(QVariant::typeToName((QVariant::Type)typeId)).arg(typeId) );
            return 0;
        } break;
    }
}
#endif