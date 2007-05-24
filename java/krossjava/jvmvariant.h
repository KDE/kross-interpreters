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
//#include <QMetaType>
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
    struct JavaType<QString>
    {
        inline static jstring toJObject(JNIEnv *env, const QString& s) {
            //if( s.isNull() ) return jstring;
            QByteArray ba = s.toUtf8();
            jstring js = env->NewStringUTF(ba.data());
            return js;
        }
        inline static QString toVariant(JNIEnv *env, jstring value) {
            const char *str = env->GetStringUTFChars(value, 0);
            QString s = str;
            env->ReleaseStringUTFChars(value, str);
            return s;
        }
    };

    template<>
    struct JavaType<QStringList>
    {
        inline static jobjectArray toJObject(JNIEnv *env, const QStringList& list) {
            const int count = list.count();
            jobjectArray objarray = env->NewObjectArray(count, env->FindClass("java/lang/String"), env->NewStringUTF(""));
            for(int i = 0; i < count; ++i)
                env->SetObjectArrayElement(objarray, i, JavaType<QString>::toJObject(env, list[i]));
            return objarray;
        }
        inline static QStringList toVariant(JNIEnv *env, jobjectArray objarray) {
            jsize len = env->GetArrayLength(objarray);
            int count = len;
            QStringList list;
            for(int i = 0; i < count; i++) {
                jstring s = (jstring) env->GetObjectArrayElement(objarray, i);
                list << JavaType<QString>::toVariant(env, s);
            }
            return list;
        }
    };

    template<>
    struct JavaType<QByteArray>
    {
        inline static jbyteArray toJObject(JNIEnv *env, const QByteArray& array) {
            const int count = array.count();
            jbyteArray bytearray = env->NewByteArray(count);
            //TODO: there has to be a better way...
            jbyte bytes[count];
            for(int i = 0; i < count; ++i)
                bytes[i] = array[i];
            env->SetByteArrayRegion(bytearray, 0, count, bytes);
            return bytearray;
        }
        inline static QByteArray toVariant(JNIEnv *env, jbyteArray bytearray) {
            jsize len = env->GetArrayLength(bytearray);
            int count = len;
            char bytes[count];
            env->GetByteArrayRegion(bytearray, 0, len, (jbyte*)bytes);
            return QByteArray(bytes);
        }
    };

#if 0
    template<>
    struct JavaType<QVariant>
    {
        static jvalue toJObject(const QVariant& v);
        static QVariant toVariant(jvalue value);
    };
#endif

    template<>
    struct JavaType<int>
    {
        inline static jint toJObject(JNIEnv *env, int i) {
            return qint32(i); //jint is signed 32 bits
        }
        inline static int toVariant(JNIEnv *env, jint value) {
            return qint32(value);
        }
    };

#if 0
    template<>
    struct JavaType<uint>
    {
        inline static VALUE toJObject(uint i) {
            return UINT2NUM(i);
        }
        inline static uint toVariant(VALUE value) {
            switch( TYPE(value) ) {
                case T_FIXNUM:
                    return FIX2UINT(value);
                case T_BIGNUM:
                    return rb_big2uint(value);
                case T_FLOAT:
                    return (uint)(RFLOAT(value)->value);
                default:
                    break;
            }
            rb_raise(rb_eTypeError, "Unsigned integer must be a fixed number");
            return 0;
        }
    };

    template<>
    struct JavaType<double>
    {
        inline static VALUE toJObject(double d) {
            return rb_float_new(d);
        }
        inline static double toVariant(VALUE value) {
            return NUM2DBL(value);
        }
    };
#endif

    template<>
    struct JavaType<bool>
    {
        inline static jboolean toJObject(bool b) {
            return b ? JNI_TRUE : JNI_FALSE;
        }
        inline static bool toVariant(jboolean value) {
            return value == JNI_TRUE;
        }
    };

    template<>
    struct JavaType<qlonglong>
    {
        inline static jlong toJObject(qlonglong l) {
            return qint64(l); //jlong is a signed 64 bits
        }
        inline static qlonglong toVariant(jlong value) {
            return qint64(value);
        }
    };

#if 0
    template<>
    struct JavaType<qulonglong>
    {
        inline static VALUE toJObject(qulonglong l) {
            return UINT2NUM((unsigned long)l);
        }
        inline static qulonglong toVariant(VALUE value) {
            return NUM2UINT(value);
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

    template<>
    struct JavaType<QUrl>
    {
        inline static VALUE toJObject(const QUrl& url) {
            return JavaType<QString>::toJObject( url.toString() );
        }
        inline static QUrl toVariant(VALUE value) {
            return QUrl( JavaType<QString>::toVariant(value) );
        }
    };

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

    template<>
    struct JavaType<QVariantMap>
    {
        inline static VALUE toJObject(const QVariantMap& map) {
            VALUE h = rb_hash_new();
            QMap<QString, QVariant>::ConstIterator it(map.constBegin()), end(map.end());
            for(; it != end; ++it)
                rb_hash_aset(h, JavaType<QString>::toJObject(it.key()), JavaType<QVariant>::toJObject(it.value()) );
            return h;
        }
        inline static int convertHash(VALUE key, VALUE value, VALUE  vmap) {
            QVariantMap* map; 
            Data_Get_Struct(vmap, QVariantMap, map);
            if (key != Qundef)
                map->insert(STR2CSTR(key), JavaType<QVariant>::toVariant(value));
            return ST_CONTINUE;
        }
        inline static QVariantMap toVariant(VALUE value) {
            if( TYPE(value) != T_HASH ) {
                rb_raise(rb_eTypeError, "QVariantMap must be a hash");
                return QVariantMap();
            }
            QVariantMap map;
            VALUE vmap = Data_Wrap_Struct(rb_cObject, 0,0, &map);
            rb_hash_foreach(value, (int (*)(...))convertHash, vmap);
            return map;
        }
    };

    /**
     * The RubyMetaTypeFactory helper class us used as factory within
     * \a RubyExtension to translate an argument into a \a MetaType
     * needed for QGenericArgument's data pointer.
     */
    class RubyMetaTypeFactory
    {
        public:
            static MetaType* create(int typeId, int metaTypeId, VALUE valueect = Qnil);
    };

    template<typename VARIANTTYPE>
    class RubyMetaTypeVariant : public MetaTypeVariant<VARIANTTYPE>
    {
        public:
            RubyMetaTypeVariant(VALUE value)
                : MetaTypeVariant<VARIANTTYPE>(
                    (TYPE(value) == T_NIL)
                        ? QVariant().value<VARIANTTYPE>()
                        : JavaType<VARIANTTYPE>::toVariant(value)
                ) {}

            virtual ~RubyMetaTypeVariant() {}
    };
#endif

}

#endif
