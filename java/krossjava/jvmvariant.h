/***************************************************************************
 * jvmvariant.h
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

#ifndef KROSS_JVMVARIANT_H
#define KROSS_JVMVARIANT_H

#include "jvmconfig.h"
#include <kross/core/metatype.h>

#include <QString>
#include <QStringList>
#include <QVariant>
#include <QMetaType>
//#include <QSize>
//#include <QPoint>
//#include <QRect>
//#include <QUrl>
//#include <QDate>
//#include <QTime>
//#include <QDateTime>

namespace Kross {

    template<typename VARIANTTYPE /*, typename JAVATYPE = jobject */ >
    struct JavaType
    {
        // template-specialisations need to implement following both static
        // functions to translate between QVariant and Java types.
    };

    template<>
    struct JavaType<QVariant>
    {
        static jobject toJObject(const QVariant& v, JNIEnv* env);
        static QVariant toVariant(jobject value, JNIEnv* env);
    };

    template<>
    struct JavaType<void*>
    {
        //TODO: Is memcpy the way to go here?
        inline static jlong toJObject(const void* qobj, JNIEnv* env) {
            Q_UNUSED(env);
            jlong pointer = 0;
            memcpy(&pointer, &qobj, sizeof(qobj));
            return pointer;
        }
        inline static void* toVariant(jlong jobj, JNIEnv* env) {
            Q_UNUSED(env);
            void *qobj;
            memcpy(&qobj, &jobj, sizeof(qobj));
            return qobj;
        }
    };

    template<>
    struct JavaType<QString>
    {
        inline static jstring toJObject(const QString& s, JNIEnv* env) {
            if( s.isNull() ) return NULL;
            QByteArray ba = s.toUtf8();
            jstring js = env->NewStringUTF(ba.data());
            return js;
        }
        inline static QString toVariant(jobject value, JNIEnv* env) {
            if(value == NULL) return QString();
            //TODO: the static_casting is unfortunate... Is there another way?
            jstring jstr = static_cast<jstring>(value);
            const char *str = env->GetStringUTFChars(jstr, 0);
            QString s = str;
            env->ReleaseStringUTFChars(jstr, str);
            return s;
        }
    };

    template<>
    struct JavaType<QStringList>
    {
        inline static jobjectArray toJObject(const QStringList& list, JNIEnv* env) {
            const int count = list.count();
            jobjectArray objarray = env->NewObjectArray(count, env->FindClass("java/lang/String"), env->NewStringUTF(""));
            for(int i = 0; i < count; ++i)
                env->SetObjectArrayElement(objarray, i, JavaType<QString>::toJObject(list[i], env));
            return objarray;
        }
        inline static QStringList toVariant(jobject value, JNIEnv* env) {
            jobjectArray objarray = static_cast<jobjectArray>(value);
            const jsize len = env->GetArrayLength(objarray);
            const int count = len;
            QStringList list;
            for(int i = 0; i < count; i++) {
                jstring s = (jstring) env->GetObjectArrayElement(objarray, i);
                list << JavaType<QString>::toVariant(s, env);
            }
            return list;
        }
    };

    template<>
    struct JavaType<QByteArray>
    {
        inline static jbyteArray toJObject(const QByteArray& array, JNIEnv* env) {
            const int count = array.count();
            jbyteArray bytearray = env->NewByteArray(count);
            //TODO: there has to be a better way...
            jbyte bytes[count];
            for(int i = 0; i < count; ++i)
                bytes[i] = array[i];
            env->SetByteArrayRegion(bytearray, 0, count, bytes);
            return bytearray;
        }
        inline static QByteArray toVariant(jobject value, JNIEnv* env) {
            jbyteArray bytearray = static_cast<jbyteArray>(value);
            jsize len = env->GetArrayLength(bytearray);
            int count = len;
            char bytes[count];
            env->GetByteArrayRegion(bytearray, 0, len, (jbyte*)bytes);
            return QByteArray(bytes, count);
        }
    };

    template<>
    struct JavaType<int>
    {
        inline static jobject toJObject(int i, JNIEnv* env) {
            jclass cl = env->FindClass("java/lang/Integer");
            jmethodID ctor = env->GetMethodID(cl, "<init>", "(I)V");
            return env->NewObject(cl, ctor, i);
        }
        inline static int toVariant(jobject value, JNIEnv* env) {
            jclass cl = env->FindClass("java/lang/Integer");
            jmethodID getval = env->GetMethodID(cl, "intValue", "()I");
            return qint32(env->CallIntMethod(value, getval));
        }
    };

    template<>
    struct JavaType<uint>
    {
        inline static jobject toJObject(int i, JNIEnv* env) {
            jclass cl = env->FindClass("java/lang/Integer");
            jmethodID ctor = env->GetMethodID(cl, "<init>", "(I)V");
            return env->NewObject(cl, ctor, quint32(i));
        }
        inline static uint toVariant(jobject value, JNIEnv* env) {
            jclass cl = env->FindClass("java/lang/Integer");
            jmethodID getval = env->GetMethodID(cl, "intValue", "()I");
            return qint32(env->CallIntMethod(value, getval));
        }
    };

    template<>
    struct JavaType<double>
    {
        inline static jobject toJObject(double d, JNIEnv* env) {
            jclass cl = env->FindClass("java/lang/Double");
            jmethodID ctor = env->GetMethodID(cl, "<init>", "(D)V");
            return env->NewObject(cl, ctor, d);
        }
        inline static double toVariant(jobject value, JNIEnv* env) {
            jclass cl = env->FindClass("java/lang/Double");
            jmethodID getval = env->GetMethodID(cl, "doubleValue", "()D");
            return env->CallDoubleMethod(value, getval);
        }
    };

    template<>
    struct JavaType<bool>
    {
        inline static jobject toJObject(bool b, JNIEnv* env) {
            jclass cl = env->FindClass("java/lang/Boolean");
            jmethodID ctor = env->GetMethodID(cl, "<init>", "(Z)V");
            return env->NewObject(cl, ctor, b);
        }
        inline static bool toVariant(jobject value, JNIEnv* env) {
            jclass cl = env->FindClass("java/lang/Boolean");
            jmethodID getval = env->GetMethodID(cl, "booleanValue", "()Z");
            return env->CallBooleanMethod(value, getval);
        }
    };
#if 0
    template<>
    struct JavaType<qlonglong>
    {
        inline static jobject toJObject(qlonglong l, JNIEnv* env) {
            return qint64(l); //jlong is a signed 64 bits
        }
        inline static qlonglong toVariant(jobject value, JNIEnv* env) {
            return qint64(value);
        }
    };

    template<>
    struct JavaType<qulonglong>
    {
        inline static jobject toJObject(qulonglong l, JNIEnv* env) {
            return quint64(l);
        }
        inline static qulonglong toVariant(jobject value, JNIEnv* env) {
            return quint64(value);
        }
    };

    template<>
    struct JavaType<QSize>
    {
        inline static VALUE toJObject(const QSize& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<int>::toJObject(s.width()));
            rb_ary_push(l, JavaType<int>::toJObject(s.height()));
            return l;
        }
        inline static QSize toVariant(VALUE value) {
            if( TYPE(value) != T_ARRAY || RARRAY(value)->len != 2 ) {
                rb_raise(rb_eTypeError, "QSize must be an array with 2 elements");
                return QSize();
            }
            return QSize( JavaType<int>::toVariant( rb_ary_entry(value,0) ), JavaType<int>::toVariant( rb_ary_entry(value,1) ) );
        }
    };

    template<>
    struct JavaType<QSizeF>
    {
        inline static VALUE toJObject(const QSizeF& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<double>::toJObject(s.width()));
            rb_ary_push(l, JavaType<double>::toJObject(s.height()));
            return l;
        }
        inline static QSizeF toVariant(VALUE value) {
            if( TYPE(value) != T_ARRAY || RARRAY(value)->len != 2 ) {
                rb_raise(rb_eTypeError, "QSizeF must be an array with 2 elements");
                return QSizeF();
            }
            return QSizeF( JavaType<double>::toVariant( rb_ary_entry(value,0) ), JavaType<double>::toVariant( rb_ary_entry(value,1) ) );

        }
    };

    template<>
    struct JavaType<QPoint>
    {
        inline static VALUE toJObject(const QPoint& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<int>::toJObject(s.x()));
            rb_ary_push(l, JavaType<int>::toJObject(s.y()));
            return l;
        }
        inline static QPoint toVariant(VALUE value) {
            if( TYPE(value) != T_ARRAY || RARRAY(value)->len != 2 ) {
                rb_raise(rb_eTypeError, "QPoint must be an array with 2 elements");
                return QPoint();
            }
            return QPoint( JavaType<int>::toVariant( rb_ary_entry(value,0) ), JavaType<int>::toVariant( rb_ary_entry(value,1) ) );
        }
    };

    template<>
    struct JavaType<QPointF>
    {
        inline static VALUE toJObject(const QPointF& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<double>::toJObject(s.x()));
            rb_ary_push(l, JavaType<double>::toJObject(s.y()));
            return l;
        }
        inline static QPointF toVariant(VALUE value) {
            if( TYPE(value) != T_ARRAY || RARRAY(value)->len != 2 ) {
                rb_raise(rb_eTypeError, "QPointF must be an array with 2 elements");
                return QPointF();
            }
            return QPointF( JavaType<double>::toVariant( rb_ary_entry(value,0) ), JavaType<double>::toVariant( rb_ary_entry(value,1) ) );
        }
    };

    template<>
    struct JavaType<QRect>
    {
        inline static VALUE toJObject(const QRect& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<int>::toJObject(s.x()));
            rb_ary_push(l, JavaType<int>::toJObject(s.y()));
            rb_ary_push(l, JavaType<int>::toJObject(s.width()));
            rb_ary_push(l, JavaType<int>::toJObject(s.height()));
            return l;
        }
        inline static QRect toVariant(VALUE value) {
            if( TYPE(value) != T_ARRAY || RARRAY(value)->len != 4 ) {
                rb_raise(rb_eTypeError, "QRect must be an array with 4 elements");
                return QRect();
            }
            return QRect( JavaType<int>::toVariant( rb_ary_entry(value,0) ), JavaType<int>::toVariant( rb_ary_entry(value,1) ),
                           JavaType<int>::toVariant( rb_ary_entry(value,2) ), JavaType<int>::toVariant( rb_ary_entry(value,3) ) );
        }
    };

    template<>
    struct JavaType<QRectF>
    {
        inline static VALUE toJObject(const QRectF& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<double>::toJObject(s.x()));
            rb_ary_push(l, JavaType<double>::toJObject(s.y()));
            rb_ary_push(l, JavaType<double>::toJObject(s.width()));
            rb_ary_push(l, JavaType<double>::toJObject(s.height()));
            return l;
        }
        inline static QRectF toVariant(VALUE value) {
            if( TYPE(value) != T_ARRAY || RARRAY(value)->len != 4 ) {
                rb_raise(rb_eTypeError, "QRectF must be an array with 4 elements");
                return QRectF();
            }
            return QRectF( JavaType<double>::toVariant( rb_ary_entry(value,0) ), JavaType<double>::toVariant( rb_ary_entry(value,1) ),
                           JavaType<double>::toVariant( rb_ary_entry(value,2) ), JavaType<double>::toVariant( rb_ary_entry(value,3) ) );
        }
    };
#endif

    template<>
    struct JavaType<QUrl>
    {
        inline static jstring toJObject(const QUrl& url, JNIEnv* env) {
            return JavaType<QString>::toJObject(url.toString(), env);
        }
        inline static QUrl toVariant(jstring value, JNIEnv* env) {
            return QUrl( JavaType<QString>::toVariant(value, env) );
        }
    };

#if 0
    template<>
    struct JavaType<QVariantList>
    {
        inline static VALUE toJObject(const QVariantList& list) {
            VALUE l = rb_ary_new();
            foreach(QVariant v, list)
                rb_ary_push(l, JavaType<QVariant>::toJObject(v));
            return l;
        }
        inline static QVariantList toVariant(VALUE value) {
            if( TYPE(value) != T_ARRAY ) {
                rb_raise(rb_eTypeError, "QVariantList must be an array");
                return QVariantList();
            }
            QVariantList l;
            for(int i = 0; i < RARRAY(value)->len; i++)
                l.append( JavaType<QVariant>::toVariant( rb_ary_entry(value, i) ) );
            return l;
        }
    };
#endif
    template<>
    struct JavaType<QVariantMap>
    {
        inline static jobject toJObject(const QVariantMap& map, JNIEnv* env) {
            jclass cl = env->FindClass("java/util/HashMap");
            jmethodID ctor = env->GetMethodID(cl, "<init>", "()V");
            jobject h = env->NewObject(cl, ctor);
            QMap<QString, QVariant>::ConstIterator it(map.constBegin()), end(map.end());
            jmethodID put = env->GetMethodID(cl, "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
            for(; it != end; ++it)
                env->CallObjectMethod(h, put, JavaType<QString>::toJObject(it.key(), env), JavaType<QVariant>::toJObject(it.value(), env) );
            return h;
        }
        inline static QVariantMap toVariant(jobject value, JNIEnv* env) {
            //if( TYPE(value) != T_HASH ) {
            //    rb_raise(rb_eTypeError, "QVariantMap must be a hash");
            //    return QVariantMap();
            //}
            QVariantMap map;
            //Admire...
            jclass hmcl = env->FindClass("java/util/HashMap");
            jclass setcl = env->FindClass("java/util/Set");
            jclass itcl = env->FindClass("java/util/Iterator");
            jclass entrycl = env->FindClass("java/util/Map$Entry");
            jmethodID getentryset = env->GetMethodID(hmcl, "entrySet", "()Ljava/util/Set;");
            jmethodID getit = env->GetMethodID(setcl, "iterator", "()Ljava/util/Iterator;");
            jmethodID hasnext = env->GetMethodID(itcl, "hasNext", "()Z");
            jmethodID next = env->GetMethodID(itcl, "next", "()Ljava/lang/Object;");
            jmethodID getkey = env->GetMethodID(entrycl, "getKey", "()Ljava/lang/Object;");
            jmethodID getval = env->GetMethodID(entrycl, "getValue", "()Ljava/lang/Object;");
            jobject set = env->CallObjectMethod(value, getentryset);
            jobject it = env->CallObjectMethod(set, getit);

            while(env->CallBooleanMethod(it, hasnext) == JNI_TRUE) {
                jobject entry = env->CallObjectMethod(it, next);
                jobject key = env->CallObjectMethod(entry, getkey);
                jobject value = env->CallObjectMethod(entry, getval);
                map.insert(JavaType<QString>::toVariant(key, env), JavaType<QVariant>::toVariant(value, env));
            }
            return map;
        }
    };

    /**
     * The JVMMetaTypeFactory helper class us used as factory within
     * \a JVMExtension to translate an argument into a \a MetaType
     * needed for QGenericArgument's data pointer.
     */
    class JVMMetaTypeFactory
    {
        public:
            static MetaType* create(JNIEnv *env, int typeId, int metaTypeId, jobject valueect = NULL);
    };

    template<typename VARIANTTYPE>
    class JVMMetaTypeVariant : public MetaTypeVariant<VARIANTTYPE>
    {
        public:
            JVMMetaTypeVariant(jobject value, JNIEnv* env)
                : MetaTypeVariant<VARIANTTYPE>(
                    (value == NULL)
                        ? QVariant().value<VARIANTTYPE>()
                        : JavaType<VARIANTTYPE>::toVariant(value, env)
                ) {}

            virtual ~JVMMetaTypeVariant() {}
    };

}

#endif
