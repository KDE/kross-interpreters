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
    /// \internal
    template<>
    struct JavaType<QVariant>
    {
        static VALUE toVALUE(const QVariant& v);
        static QVariant toVariant(VALUE value);
    };

    /// \internal
    template<>
    struct JavaType<int>
    {
        inline static VALUE toVALUE(int i) {
            return INT2FIX(i);
        }
        inline static int toVariant(VALUE value) {
            switch( TYPE(value) ) {
                case T_FIXNUM:
                    return FIX2INT(value);
                case T_BIGNUM:
                    return rb_big2int(value);
                case T_FLOAT:
                    return (int)(RFLOAT(value)->value);
                default:
                    break;
            }
            rb_raise(rb_eTypeError, "Integer must be a fixed number");
            return 0;
        }
    };

    /// \internal
    template<>
    struct JavaType<uint>
    {
        inline static VALUE toVALUE(uint i) {
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

    /// \internal
    template<>
    struct JavaType<double>
    {
        inline static VALUE toVALUE(double d) {
            return rb_float_new(d);
        }
        inline static double toVariant(VALUE value) {
            return NUM2DBL(value);
        }
    };

    /// \internal
    template<>
    struct JavaType<bool>
    {
        inline static VALUE toVALUE(bool b) {
            return b ? Qtrue : Qfalse;
        }
        inline static bool toVariant(VALUE value) {
            switch( TYPE(value) ) {
                case T_TRUE:
                    return true;
                case T_FALSE:
                    return false;
                default: {
                    rb_raise(rb_eTypeError, "Boolean value expected");
                    return false;
                } break;
            }
        }
    };

    /// \internal
    template<>
    struct JavaType<qlonglong>
    {
        inline static VALUE toVALUE(qlonglong l) {
            return /*INT2NUM*/ LONG2NUM((long)l);
        }
        inline static qlonglong toVariant(VALUE value) {
            return NUM2LONG(value);
        }
    };

    /// \internal
    template<>
    struct JavaType<qulonglong>
    {
        inline static VALUE toVALUE(qulonglong l) {
            return UINT2NUM((unsigned long)l);
        }
        inline static qulonglong toVariant(VALUE value) {
            return NUM2UINT(value);
        }
    };

    /// \internal
    template<>
    struct JavaType<QByteArray>
    {
        inline static VALUE toVALUE(const QByteArray& ba) {
            return rb_str_new(ba.constData(), ba.size());
        }
        inline static QByteArray toVariant(VALUE value) {
            if( TYPE(value) != T_STRING ) {
                rb_raise(rb_eTypeError, "QByteArray must be a string");
                //return STR2CSTR( rb_inspect(value) );
                return QByteArray("");
            }
            long length = LONG2NUM( RSTRING(value)->len );
            if( length < 0 )
                return QByteArray("");
            char* ca = rb_str2cstr(value, &length);
            return QByteArray(ca, length);
        }
    };

    /// \internal
    template<>
    struct JavaType<QSize>
    {
        inline static VALUE toVALUE(const QSize& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<int>::toVALUE(s.width()));
            rb_ary_push(l, JavaType<int>::toVALUE(s.height()));
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

    /// \internal
    template<>
    struct JavaType<QSizeF>
    {
        inline static VALUE toVALUE(const QSizeF& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<double>::toVALUE(s.width()));
            rb_ary_push(l, JavaType<double>::toVALUE(s.height()));
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

    /// \internal
    template<>
    struct JavaType<QPoint>
    {
        inline static VALUE toVALUE(const QPoint& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<int>::toVALUE(s.x()));
            rb_ary_push(l, JavaType<int>::toVALUE(s.y()));
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

    /// \internal
    template<>
    struct JavaType<QPointF>
    {
        inline static VALUE toVALUE(const QPointF& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<double>::toVALUE(s.x()));
            rb_ary_push(l, JavaType<double>::toVALUE(s.y()));
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

    /// \internal
    template<>
    struct JavaType<QRect>
    {
        inline static VALUE toVALUE(const QRect& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<int>::toVALUE(s.x()));
            rb_ary_push(l, JavaType<int>::toVALUE(s.y()));
            rb_ary_push(l, JavaType<int>::toVALUE(s.width()));
            rb_ary_push(l, JavaType<int>::toVALUE(s.height()));
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

    /// \internal
    template<>
    struct JavaType<QRectF>
    {
        inline static VALUE toVALUE(const QRectF& s) {
            VALUE l = rb_ary_new();
            rb_ary_push(l, JavaType<double>::toVALUE(s.x()));
            rb_ary_push(l, JavaType<double>::toVALUE(s.y()));
            rb_ary_push(l, JavaType<double>::toVALUE(s.width()));
            rb_ary_push(l, JavaType<double>::toVALUE(s.height()));
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

    /// \internal
    template<>
    struct JavaType<QUrl>
    {
        inline static VALUE toVALUE(const QUrl& url) {
            return JavaType<QString>::toVALUE( url.toString() );
        }
        inline static QUrl toVariant(VALUE value) {
            return QUrl( JavaType<QString>::toVariant(value) );
        }
    };

    /// \internal
    template<>
    struct JavaType<QVariantList>
    {
        inline static VALUE toVALUE(const QVariantList& list) {
            VALUE l = rb_ary_new();
            foreach(QVariant v, list)
                rb_ary_push(l, JavaType<QVariant>::toVALUE(v));
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

    /// \internal
    template<>
    struct JavaType<QVariantMap>
    {
        inline static VALUE toVALUE(const QVariantMap& map) {
            VALUE h = rb_hash_new();
            QMap<QString, QVariant>::ConstIterator it(map.constBegin()), end(map.end());
            for(; it != end; ++it)
                rb_hash_aset(h, JavaType<QString>::toVALUE(it.key()), JavaType<QVariant>::toVALUE(it.value()) );
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

    /// \internal
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
