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
#include "jvmextension.h"
#include "jvminterpreter.h"

using namespace Kross;

jobject JavaType<QVariant>::toJObject(const QVariant& v, JNIEnv* env)
{
    #ifdef KROSS_JVM_VARIANT_DEBUG
        krossdebug( QString("JavaType<QVariant>::toJObject variant.toString=%1 variant.typeid=%2 variant.typeName=%3").arg(v.toString()).arg(v.type()).arg(v.typeName()) );
    #endif
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
        case QVariant::Invalid: {
            //TODO: I don't see a problem with this, someone else?
            #ifdef KROSS_JVM_VARIANT_DEBUG
                krossdebug( QString("JavaType<QVariant>::toJObject variant=%1 is QVariant::Invalid. Returning NULL.").arg(v.toString()) );
            #endif
            return NULL;
        }
        case QVariant::UserType: {
            #ifdef KROSS_JVM_VARIANT_DEBUG
                krossdebug( QString("JavaType<QVariant>::toJObject variant=%1 is QVariant::UserType. Trying to cast now.").arg(v.toString()) );
            #endif
        } // fall through
        default: {
#if 0
            if( strcmp(v.typeName(),"float") == 0 ) {
                return JavaType<double>::toJObject(v.toDouble());
            }
#endif

            if( qVariantCanConvert< QWidget* >(v) ) {
                #ifdef KROSS_JVM_VARIANT_DEBUG
                    krossdebug( QString("JavaType<QVariant>::toJObject Casting '%1' to QWidget").arg(v.typeName()) );
                #endif
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
                #ifdef KROSS_JVM_VARIANT_DEBUG
                    krossdebug( QString("JavaType<QVariant>::toJObject Casting '%1' to QObject").arg(v.typeName()) );
                #endif
                QObject* obj = qvariant_cast< QObject* >(v);
                if(! obj) {
                    #ifdef KROSS_JVM_VARIANT_DEBUG
                        krosswarning( QString("JavaType<QVariant>::toJObject To QObject casted '%1' is NULL").arg(v.typeName()) );
                    #endif
                    return 0;
                }
                const JVMExtension* ext = JVMInterpreter::extension(obj);
                if(ext == 0)
                    ext = new JVMExtension(obj);
                return ext->javaobject();
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
    #ifdef KROSS_JVM_VARIANT_DEBUG
        krossdebug( QString("JavaType<QVariant>::toVariant") );
    #endif
    if(value == NULL)
        return QVariant();

    jclass cl = env->GetObjectClass(value);
    jclass other;

    other = env->FindClass("java/lang/Integer");
    if(env->IsAssignableFrom(cl, other) == JNI_TRUE)
        return JavaType<int>::toVariant(value, env);

    //uint and ulonglong are impossible to do in this direction,
    //because it makes more sense to cast Integer and Long
    //to int and longlong respectively.

    other = env->FindClass("java/lang/Double");
    if(env->IsAssignableFrom(cl, other) == JNI_TRUE)
        return JavaType<double>::toVariant(value, env);

    other = env->FindClass("java/lang/String");
    if(env->IsAssignableFrom(cl, other) == JNI_TRUE)
        return JavaType<QString>::toVariant(value, env);

    //TODO: Bytearray?

    other = env->FindClass("java/lang/Boolean");
    if(env->IsAssignableFrom(cl, other) == JNI_TRUE)
        return JavaType<bool>::toVariant(value, env);

    other = env->FindClass("java/util/ArrayList");
    if(env->IsAssignableFrom(cl, other) == JNI_TRUE)
        return JavaType<QVariantList>::toVariant(value, env);

    other = env->FindClass("java/lang/Long");
    if(env->IsAssignableFrom(cl, other) == JNI_TRUE)
        return JavaType<qlonglong>::toVariant(value, env);

    //TODO: Stringlist... Other arrays...

    other = env->FindClass("java/util/Map");
    if(env->IsAssignableFrom(cl, other) == JNI_TRUE)
        return JavaType<QVariantMap>::toVariant(value, env);

    other = env->FindClass("java/net/URL");
    if(env->IsAssignableFrom(cl, other) == JNI_TRUE)
        return JavaType<QUrl>::toVariant(value, env);

    #ifdef KROSS_JVM_VARIANT_DEBUG
        krossdebug( "Could not convert the jobject to a known QVariant, returning null." );
    #endif
    return QVariant();
}


MetaType* JVMMetaTypeFactory::create(JNIEnv* env, int typeId, int metaTypeId, jobject value)
{
    #ifdef KROSS_JVM_VARIANT_DEBUG
        krossdebug( QString("JVMMetaTypeFactory::create typeId=%1 typeName=%2").arg(QMetaType::typeName(typeId)).arg(typeId) );
    #endif
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
        case QVariant::Invalid: // fall through
        case QVariant::UserType: // fall through
        default: {
            if( JVMExtension::isJVMExtension(value, env) ) {
                #ifdef KROSS_JVM_VARIANT_DEBUG
                    krossdebug( QString("JVMMetaTypeFactory::create jobject with typeId '%1' is a JVMExtension object").arg(typeId) );
                #endif
                JVMExtension* extension;

                jclass KQExt = env->FindClass("org/kde/kdebindings/java/krossjava/KrossQExtension");
                jmethodID getpoint = env->GetMethodID(KQExt, "getPointer", "()J");
                jlong p = env->CallLongMethod(value, getpoint);
                extension = (JVMExtension*)JavaType<void*>::toVariant(p, env);

                Q_ASSERT(extension);
                QObject* object = extension->object();
                if(! object) {
                    krosswarning("JVMMetaTypeFactory::create QObject is NULL.");
                    return 0;
                }
                return new MetaTypeVoidStar( typeId, object, false );
            }

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
