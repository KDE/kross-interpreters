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

jobject JavaType<QVariant>::toJObject(const QVariant& v, JNIEnv* env)
{
    krossdebug( QString("JavaType<QVariant>::toJObject variant.toString=%1 variant.typeid=%2 variant.typeName=%3").arg(v.toString()).arg(v.type()).arg(v.typeName()) );
    switch( v.type() ) {
        case QVariant::Int:
            return JavaType<int>::toJObject(v.toInt(), env);
        case QVariant::UInt:
            return JavaType<uint>::toJObject(v.toUInt(), env);
        case QVariant::Double:
            return JavaType<double>::toJObject(v.toDouble(), env);
        case QVariant::ByteArray:
            return JavaType<QByteArray>::toJObject(v.toByteArray(), env);
        case QVariant::String:
            return JavaType<QString>::toJObject(v.toString(), env);
        case QVariant::Bool:
            return JavaType<bool>::toJObject(v.toBool(), env);
        case QVariant::StringList:
            return JavaType<QStringList>::toJObject(v.toStringList(), env);
#if 0
        case QVariant::Map:
            return JavaType<QVariantMap>::toJObject(v.toMap(), env);
        case QVariant::List:
            return JavaType<QVariantList>::toJObject(v.toList(), env);
        case QVariant::LongLong:
            return JavaType<qlonglong>::toJObject(v.toLongLong(), env);
        case QVariant::ULongLong:
            return JavaType<qlonglong>::toJObject(v.toULongLong(), env);
        case QVariant::Url:
            return JavaType<QUrl>::toJObject(v.toUrl(), env);
        case QVariant::Size:
            return JavaType<QSize>::toJObject(v.toSize(), env);
        case QVariant::SizeF:
            return JavaType<QSizeF>::toJObject(v.toSizeF(), env);
        case QVariant::Point:
            return JavaType<QPoint>::toJObject(v.toPoint(), env);
        case QVariant::PointF:
            return JavaType<QPointF>::toJObject(v.toPointF(), env);
        case QVariant::Rect:
            return JavaType<QRect>::toJObject(v.toRect(), env);
        case QVariant::RectF:
            return JavaType<QRectF>::toJObject(v.toRectF(), env);
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

            krosswarning( QString("JavaType<QVariant>::toJObject Not possible to convert the QVariant '%1' with type '%2' (%3) to a jobject.").arg(v.toString()).arg(v.typeName()).arg(v.type()) );
            JVMException::throwNullPointerException(env);
            return NULL;
        }
    }
}

QVariant JavaType<QVariant>::toVariant(jobject value, JNIEnv* env)
{
    krossdebug( QString("JavaType<QVariant>::toVariant") );

    //TODO JNIType
    Q_UNUSED(value);
    Q_UNUSED(env);

    return QVariant();
}


MetaType* JVMMetaTypeFactory::create(JNIEnv* env, int typeId, int metaTypeId, jobject value)
{
    krossdebug( QString("JVMMetaTypeFactory::create typeId=%1 typeName=%2").arg(QMetaType::typeName(typeId)).arg(typeId) );
    switch(typeId) {
        case QVariant::Int:
            return new JVMMetaTypeVariant<int>(value, env);
        case QVariant::UInt:
            return new JVMMetaTypeVariant<uint>(value, env);
        case QVariant::Double:
            return new JVMMetaTypeVariant<double>(value, env);
        case QVariant::Bool:
            return new JVMMetaTypeVariant<bool>(value, env);
        case QVariant::ByteArray:
            return new JVMMetaTypeVariant<QByteArray>(value, env);
        case QVariant::String:
            return new JVMMetaTypeVariant<QString>(value, env);
        case QVariant::StringList:
            return new JVMMetaTypeVariant<QStringList>(value, env);
#if 0
        case QVariant::Map:
            return new JVMMetaTypeVariant<QVariantMap>(value, env);
        case QVariant::List:
            return new JVMMetaTypeVariant<QVariantList>(value, env);
        case QVariant::LongLong:
            return new JVMMetaTypeVariant<qlonglong>(value, env);
        case QVariant::ULongLong:
            return new JVMMetaTypeVariant<qulonglong>(value, env);
        case QVariant::Url:
            return new JVMMetaTypeVariant<QUrl>(value, env);
        case QVariant::Size:
            return new JVMMetaTypeVariant<QSize>(value, env);
        case QVariant::SizeF:
            return new JVMMetaTypeVariant<QSizeF>(value, env);
        case QVariant::Point:
            return new JVMMetaTypeVariant<QPoint>(value, env);
        case QVariant::PointF:
            return new JVMMetaTypeVariant<QPointF>(value, env);
        case QVariant::Rect:
            return new JVMMetaTypeVariant<QRect>(value, env);
        case QVariant::RectF:
            return new JVMMetaTypeVariant<QRectF>(value, env);
#endif
        case QVariant::Invalid: // fall through
        case QVariant::UserType: // fall through
        default: {
#if 0
            if( JVMExtension::isJVMExtension(value) ) {
                #ifdef KROSS_JVM_VARIANT_DEBUG
                    krossdebug( QString("JVMMetaTypeFactory::create jobject with typeId '%1' is a JVMExtension object").arg(typeId) );
                #endif
                JVMExtension* extension;
                Data_Get_Struct(value, JVMExtension, extension);
                Q_ASSERT(extension);
                QObject* object = extension->object();
                if(! object) {
                    krosswarning("JVMMetaTypeFactory::create QObject is NULL.");
                    return 0;
                }
                return new MetaTypeVoidStar( typeId, object, false );
            }
#endif
            if( value == NULL ) {
                #ifdef KROSS_JVM_VARIANT_DEBUG
                    krossdebug( QString("JVMMetaTypeFactory::create jobject is NULL. Create empty type '%1'").arg(metaTypeId) );
                #endif
                void* ptr = QMetaType::construct(metaTypeId, 0);
                return new MetaTypeVoidStar( metaTypeId, ptr, false );
            }
            //QVariant v = JavaType<QVariant>::toVariant(object);
            //krossdebug( QString("RubyVariant::create Converted VALUE '%1' with type '%2 %3' to QVariant with type '%4 %5'").arg(object.as_string().c_str()).arg(typeName).arg(typeId).arg(v.toString()).arg(v.typeName()) );
            //if(typeId == QVariant::Invalid) return new RubyVariantImpl<void>();
            //return new RubyVariantImpl<QVariant>(v);
            krosswarning( QString("JVMMetaTypeFactory::create Not possible to convert the jobject to QVariant with '%1' and metaid '%2'").arg(QVariant::typeToName((QVariant::Type)typeId)).arg(typeId) );
            return 0;
        } break;
    }
}
