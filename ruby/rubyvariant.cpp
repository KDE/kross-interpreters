/***************************************************************************
 * rubyvariant.cpp
 * This file is part of the KDE project
 * copyright (C)2005 by Cyrille Berger (cberger@cberger.net)
 * copyright (C)2006 by Sebastian Sauer (mail@dipe.org)
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

#include "rubyvariant.h"
#include "rubyextension.h"
#include "rubyobject.h"

#include <QWidget>

using namespace Kross;

VALUE RubyType<QVariant>::toVALUE(const QVariant& v)
{
    #ifdef KROSS_RUBY_VARIANT_DEBUG
        krossdebug( QString("RubyType<QVariant>::toVALUE variant.toString=%1 variant.typeid=%2 variant.typeName=%3").arg(v.toString()).arg(v.type()).arg(v.typeName()) );
    #endif

    switch( v.type() ) {
        case QVariant::Int:
            return RubyType<int>::toVALUE(v.toInt());
        case QVariant::UInt:
            return RubyType<uint>::toVALUE(v.toUInt());
        case QVariant::Double:
            return RubyType<double>::toVALUE(v.toDouble());
        case QVariant::ByteArray:
            return RubyType<QByteArray>::toVALUE(v.toByteArray());
        case QVariant::String:
            return RubyType<QString>::toVALUE(v.toString());
        case QVariant::Bool:
            return RubyType<bool>::toVALUE(v.toBool());
        case QVariant::StringList:
            return RubyType<QStringList>::toVALUE(v.toStringList());
        case QVariant::Map:
            return RubyType<QVariantMap>::toVALUE(v.toMap());
        case QVariant::List:
            return RubyType<QVariantList>::toVALUE(v.toList());
        case QVariant::LongLong:
            return RubyType<qlonglong>::toVALUE(v.toLongLong());
        case QVariant::ULongLong:
            return RubyType<qlonglong>::toVALUE(v.toULongLong());

        case QVariant::Size:
            return RubyType<QSize>::toVALUE(v.toSize());
        case QVariant::SizeF:
            return RubyType<QSizeF>::toVALUE(v.toSizeF());
        case QVariant::Point:
            return RubyType<QPoint>::toVALUE(v.toPoint());
        case QVariant::PointF:
            return RubyType<QPointF>::toVALUE(v.toPointF());
        case QVariant::Rect:
            return RubyType<QRect>::toVALUE(v.toRect());
        case QVariant::RectF:
            return RubyType<QRectF>::toVALUE(v.toRectF());

        case QVariant::Color:
            return RubyType<QColor>::toVALUE( v.value<QColor>() );
        case QVariant::Url:
            return RubyType<QUrl>::toVALUE(v.toUrl());

        case QVariant::Date:
            return RubyType<QDate>::toVALUE( v.value<QDate>() );
        case QVariant::Time:
            return RubyType<QTime>::toVALUE( v.value<QTime>() );
        case QVariant::DateTime:
            return RubyType<QDateTime>::toVALUE( v.value<QDateTime>() );

        case QVariant::Invalid: {
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug( QString("RubyType<QVariant>::toVALUE variant=%1 is QVariant::Invalid. Returning Qnil.").arg(v.toString()) );
            #endif
            return Qnil;
        } // fall through

        case QVariant::UserType: {
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug( QString("RubyType<QVariant>::toVALUE variant=%1 is QVariant::UserType. Trying to cast now.").arg(v.toString()) );
            #endif
        } // fall through

        default: {
            if( strcmp(v.typeName(),"float") == 0 ) {
                #ifdef KROSS_RUBY_VARIANT_DEBUG
                    krossdebug( QString("RubyType<QVariant>::toVALUE Casting '%1' to double").arg(v.typeName()) );
                #endif
                return RubyType<double>::toVALUE(v.toDouble());
            }

            if( qVariantCanConvert< Kross::Object::Ptr >(v) ) {
                #ifdef KROSS_RUBY_VARIANT_DEBUG
                    krossdebug( QString("RubyType<QVariant>::toPyObject Casting '%1' to Kross::Object::Ptr").arg(v.typeName()) );
                #endif
                Kross::Object::Ptr obj = v.value< Kross::Object::Ptr >();
                Kross::RubyObject* rbobj = dynamic_cast< Kross::RubyObject* >(obj.data());
                if(! obj) {
                    #ifdef KROSS_RUBY_VARIANT_DEBUG
                        krossdebug( QString("RubyType<QVariant>::toPyObject To Kross::RubyObject* casted '%1' is NULL").arg(v.typeName()) );
                    #endif
                    return Qnil;
                }
                return rbobj->rbObject();
            }

            if( qVariantCanConvert< QWidget* >(v) ) {
                #ifdef KROSS_RUBY_VARIANT_DEBUG
                    krossdebug( QString("RubyType<QVariant>::toVALUE Casting '%1' to QWidget").arg(v.typeName()) );
                #endif
                QWidget* widget = qvariant_cast< QWidget* >(v);
                if(! widget) {
                    #ifdef KROSS_RUBY_VARIANT_DEBUG
                        krosswarning( QString("RubyType<QVariant>::toVALUE To QWidget casted '%1' is NULL").arg(v.typeName()) );
                    #endif
                    return Qnil;
                }
                return RubyExtension::toVALUE( new RubyExtension(widget), true /*owner*/ );
            }

            if( qVariantCanConvert< QObject* >(v) ) {
                #ifdef KROSS_RUBY_VARIANT_DEBUG
                    krossdebug( QString("RubyType<QVariant>::toVALUE Casting '%1' to QObject*").arg(v.typeName()) );
                #endif
                QObject* obj = qvariant_cast< QObject* >(v);
                if(! obj) {
                    #ifdef KROSS_RUBY_VARIANT_DEBUG
                        krosswarning( QString("RubyType<QVariant>::toVALUE To QObject casted '%1' is NULL").arg(v.typeName()) );
                    #endif
                    return Qnil;
                }
                return RubyExtension::toVALUE( new RubyExtension(obj), true /*owner*/ );
            }

            //QObject* obj = (*reinterpret_cast< QObject*(*)>( variantargs[0]->toVoidStar() ));
            //PyObject* qobjectptr = PyLong_FromVoidPtr( (void*) variantargs[0]->toVoidStar() );

            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krosswarning( QString("RubyType<QVariant>::toVALUE Not possible to convert the QVariant '%1' with type '%2' (%3) to a VALUE.").arg(v.toString()).arg(v.typeName()).arg(v.type()) );
            #endif
            //throw Py::TypeError( QString("Variant of type %1 can not be casted to a Ruby object.").arg(v.typeName()).toLatin1().constData() );
            return 0;
        }
    }
}

QVariant RubyType<QVariant>::toVariant(VALUE value)
{
    #ifdef KROSS_RUBY_VARIANT_DEBUG
        krossdebug(QString("RubyType<QVariant>::toVariant of type=%1 inspect=%2").arg(TYPE(value)).arg(STR2CSTR(rb_inspect(value))));
    #endif

    switch( TYPE(value) )
    {
        case T_DATA: {
            if(! RubyExtension::isRubyExtension(value)) {
                #ifdef KROSS_RUBY_VARIANT_DEBUG
                    krosswarning("Cannot yet convert standard ruby type to Kross::RubyExtension object.");
                #endif
                return QVariant();
            }

            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug("RubyType<QVariant>::toVariant VALUE is a Kross::RubyExtension");
            #endif
            RubyExtension* extension;
            Data_Get_Struct(value, RubyExtension, extension);
            Q_ASSERT(extension);
            QObject* object = extension->object();
            if(! object) {
                krossdebug("RubyType<QVariant>::toVariant QObject is NULL. Returning QVariant::Invalid.");
                return QVariant();
            }
            //krossdebug(QString(">>>>>>>> %1").arg(object->objectName()));
            return qVariantFromValue( object );
        }

        case T_FLOAT:
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug("  VALUE is a T_FLOAT");
            #endif
            return RubyType<double>::toVariant(value);
        case T_STRING:
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug("  VALUE is a T_STRING");
            #endif
            return RubyType<QString>::toVariant(value);
        case T_ARRAY:
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug("  VALUE is a T_ARRAY");
            #endif
            return RubyType<QVariantList>::toVariant(value);
        case T_FIXNUM:
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug("  VALUE is a T_FIXNUM");
            #endif
            return RubyType<int>::toVariant(value);
        case T_HASH:
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug("  VALUE is a T_HASH");
            #endif
            return RubyType<QVariantMap>::toVariant(value);
        case T_BIGNUM:
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug("  VALUE is a T_BIGNUM");
            #endif
            return RubyType<qlonglong>::toVariant(value);

        case T_FALSE:
        case T_TRUE:
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug("  VALUE is a T_TRUE/T_FALSE");
            #endif
            return RubyType<bool>::toVariant(value);

        case T_SYMBOL: // hmmm... where is this used?
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug("  VALUE is a T_SYMBOL");
            #endif
            return QString(rb_id2name(SYM2ID(value)));

        case T_NIL:
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug("  VALUE is a T_NIL. Returning QVariant::Invalid.");
            #endif
            return QVariant();

        case T_OBJECT:
        {
            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug("  VALUE is a T_OBJECT.");
            #endif
            Kross::RubyObject* robj = new Kross::RubyObject(value);
            Kross::Object::Ptr p(robj);
            QVariant result;
            result.setValue(p);
            return result;
        }

        case T_MATCH:
        case T_FILE:
        case T_STRUCT:
        case T_REGEXP:
        case T_MODULE:
        case T_ICLASS:
        case T_CLASS:
        default:
            krosswarning(QString("Cannot convert the ruby type '%1'").arg(TYPE(value)));
            return QVariant();
    }
}

/*
MetaType* RubyMetaTypeFactory::create(const char* typeName, VALUE value)
{
    int typeId = QVariant::nameToType(typeName);
    #ifdef KROSS_RUBY_VARIANT_DEBUG
        krossdebug( QString("RubyMetaTypeFactory::create typeName=%1 metatype.id=%2 variant.id=%3").arg(typeName).arg(QMetaType::type(typeName)).arg(typeId) );
    #endif
    return RubyMetaTypeFactory::create(typeId, value);
}
*/

MetaType* RubyMetaTypeFactory::create(int typeId, int metaTypeId, VALUE value)
{
    #ifdef KROSS_RUBY_VARIANT_DEBUG
        krossdebug( QString("RubyMetaTypeFactory::create typeName=%1 typeId=%2").arg(QMetaType::typeName(metaTypeId)).arg(metaTypeId) );
    #endif

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

        case QVariant::Color:
            return new RubyMetaTypeVariant<QColor>(value);
        case QVariant::Url:
            return new RubyMetaTypeVariant<QUrl>(value);

        case QVariant::Date:
            return new RubyMetaTypeVariant<QDate>(value);
        case QVariant::Time:
            return new RubyMetaTypeVariant<QTime>(value);
        case QVariant::DateTime:
            return new RubyMetaTypeVariant<QDateTime>(value);

        case QVariant::Invalid: // fall through
        case QVariant::UserType: // fall through
        default: {
            if( RubyExtension::isRubyExtension(value) ) {
                #ifdef KROSS_RUBY_VARIANT_DEBUG
                    krossdebug( QString("RubyMetaTypeFactory::create VALUE with typeId '%1' is a RubyExtension object").arg(typeId) );
                #endif

                RubyExtension* extension;
                Data_Get_Struct(value, RubyExtension, extension);
                Q_ASSERT(extension);

                QObject* object = extension->object();
                if(! object) {
                    krosswarning("RubyMetaTypeFactory::create QObject is NULL.");
                    return 0;
                }

                return new MetaTypeVoidStar( typeId, object, false /*owner*/ );
            }

            if( metaTypeId > 0 ) {
                //if( TYPE(value) == T_NIL ) {
                    switch(metaTypeId) {
                        case QMetaType::QObjectStar: // fall through
                        case QMetaType::QWidgetStar: {
                            if( TYPE(value) == T_DATA ) {
                                QByteArray clazzname = rb_class2name(CLASS_OF(value));
                                #ifdef KROSS_RUBY_VARIANT_DEBUG
                                    krossdebug( QString("RubyType<QVariant>::toVariant VALUE is class='%1' inspect='%2'").arg(clazzname.constData()).arg(STR2CSTR(rb_inspect(value))) );
                                #endif

                                if( clazzname.startsWith("Qt::") ) { // QtRuby
                                    VALUE src = RubyType<QString>::toVALUE("Qt::Internal");
                                    VALUE module = rb_funcall(CLASS_OF(value),rb_intern("module_eval"), 1, src);
                                    VALUE result = rb_funcall(module, rb_intern("smoke2kross"), 1, value);
                                    QObject* obj = 0;
                                    switch(metaTypeId) {
                                        case QMetaType::QWidgetStar:
                                            Data_Get_Struct(result, QWidget, obj);
                                            break;
                                        case QMetaType::QObjectStar:
                                            Data_Get_Struct(result, QObject, obj);
                                            break;
                                        default:
                                            break;
                                    }
                                    #ifdef KROSS_RUBY_VARIANT_DEBUG
                                        krossdebug( QString("RubyType<QVariant>::toVariant QtRuby result=%1 [%2] obj=%3 [%4]").arg(STR2CSTR(rb_inspect(result))).arg(STR2CSTR(rb_inspect(CLASS_OF(result)))).arg(obj ? obj->objectName() : "NULL").arg(obj ? obj->metaObject()->className() : "NULL") );
                                    #endif
                                    return new MetaTypeVoidStar( metaTypeId, obj, false /*owner*/ );
                                }
                            }
                            #ifdef KROSS_RUBY_VARIANT_DEBUG
                                krossdebug( QString("RubyMetaTypeFactory::create VALUE is T_NIL. Create empty type '%1'").arg(metaTypeId) );
                            #endif
                            void* ptr = 0; //QMetaType::construct( metaTypeId, 0 );
                            return new MetaTypeVoidStar( metaTypeId, ptr, false /* owner */ );
                        } break;
                        default:
                            break;
                    }
                //}

                // this is a dirty hack to downcast KUrl's to QUrl's
                // without the need to link against kdelibs.
                const char* typeName = QMetaType::typeName(metaTypeId);
                if( strcmp(typeName,"KUrl") == 0 ) {
                    return new RubyMetaTypeVariant<QUrl>(value);
                }
            }

            QVariant v = RubyType<QVariant>::toVariant(value);

            if( qVariantCanConvert< Kross::Object::Ptr >(v) ) {
                #ifdef KROSS_PYTHON_VARIANT_DEBUG
                    krossdebug( QString("PythonType<QVariant>::toPyObject Casting '%1' to Kross::Object::Ptr").arg(v.typeName()) );
                #endif
                if( Kross::Object::Ptr ptr = v.value< Kross::Object::Ptr >() )
                    return new Kross::MetaTypeVariant<Kross::Object::Ptr>(ptr);
            }

            #ifdef KROSS_RUBY_VARIANT_DEBUG
                krossdebug( QString("RubyVariant::create Converted VALUE with type '%1 %2 %3' to QVariant with typename=%3 toString=%4").arg(QMetaType::typeName(typeId)).arg(typeId).arg(metaTypeId).arg(v.typeName()).arg(v.toString()) );
            #endif
            return new Kross::MetaTypeVariant< QVariant >( v );
        } break;
    }
}
