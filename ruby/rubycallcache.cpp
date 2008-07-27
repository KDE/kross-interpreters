/***************************************************************************
 * rubycallcache.cpp
 * This file is part of the KDE project
 * copyright (C)2006 by Cyrille Berger (cberger@cberger.net)
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

#include "rubycallcache.h"
#include "rubyvariant.h"
#include "rubyinterpreter.h"

#include <kross/core/manager.h>
#include <kross/core/metatype.h>

#include <QVariant>
#include <QMetaObject>
#include <QMetaMethod>

namespace Kross {

    struct RubyCallCachePrivate
    {
        RubyCallCachePrivate(QObject* nobject, int nmethodindex, bool nhasreturnvalue, QVarLengthArray<int> ntypes, QVarLengthArray<int> nmetatypes)
            : object(nobject), methodindex(nmethodindex), hasreturnvalue(nhasreturnvalue), types(ntypes), metatypes(nmetatypes)
        {
        }
        QObject* object;
        int methodindex;
        QMetaMethod metamethod;
        bool hasreturnvalue;
        QVarLengthArray<int> types, metatypes;
        static VALUE s_rccObject;

        #ifdef KROSS_RUBY_CALLCACHE_CTORDTOR_DEBUG
            /// \internal string for debugging.
            QString debuginfo;
        #endif
    };

    VALUE RubyCallCachePrivate::s_rccObject = 0;

    RubyCallCache::RubyCallCache(QObject* object, int methodindex, bool hasreturnvalue, QVarLengthArray<int> ntypes, QVarLengthArray<int> nmetatypes)
        : d(new RubyCallCachePrivate(object, methodindex, hasreturnvalue, ntypes, nmetatypes)), m_self(0)
    {
        Q_ASSERT(object);
        d->metamethod = d->object->metaObject()->method(d->methodindex);
        #ifdef KROSS_RUBY_CALLCACHE_CTORDTOR_DEBUG
            d->debuginfo = QString("name=%1 class=%2 methodindex=%3 signature=%4").arg(object->objectName()).arg(object->metaObject()->className()).arg(d->methodindex).arg(d->metamethod.signature());
            krossdebug( QString("RubyCallCache Ctor %1 ").arg(d->debuginfo) );
        #endif
    }

    RubyCallCache::~RubyCallCache()
    {
        #ifdef KROSS_RUBY_CALLCACHE_CTORDTOR_DEBUG
            krossdebug( QString("RubyCallCache Dtor %1 ").arg(d->debuginfo) );
        #endif
        delete d;
    }

    VALUE RubyCallCache::execfunction( int argc, VALUE *argv )
    {
        int typelistcount = d->types.count();
        QVarLengthArray<MetaType*> variantargs( typelistcount );
        QVarLengthArray<void*> voidstarargs( typelistcount );

        #ifdef KROSS_RUBY_CALLCACHE_DEBUG
            krossdebug( QString("RubyCallCache::execfunction signature=%1 typeName=%2 argc=%3 typelistcount=%4").arg(d->metamethod.signature()).arg(d->metamethod.typeName()).arg(argc).arg(typelistcount) );
            for(int i = 0; i < d->types.count(); ++i)
                krossdebug( QString("  argument index=%1 typeId=%2 typeName=%3 metaTypeId=%4").arg(i).arg(d->types[i]).arg(QVariant::typeToName( (QVariant::Type)d->types[i] )).arg(d->metatypes[i]) );
        #endif

        Q_ASSERT(argc >= typelistcount);

        // set the return value
        if(d->hasreturnvalue)
        {
            //krossdebug( QString("RubyCallCache::execfunction argv[1]=%1").arg(argc > 1 ? STR2CSTR(rb_inspect(argv[1])) : "") );
            MetaType* returntype = RubyMetaTypeFactory::create( d->metamethod.typeName(), d->types[0], d->metatypes[0] );
            //MetaType* returntype = RubyMetaTypeFactory::create( d->types[0], d->metatypes[0], argc > 1 ? argv[1] : Qnil );
            variantargs[0] = returntype;
            voidstarargs[0] = returntype->toVoidStar();
        }
        else
        {
            variantargs[0] = 0;
            voidstarargs[0] = (void*)0;
        }

        // set the arguments values
        QList<QByteArray> typelist = d->metamethod.parameterTypes();
        for(int idx = 1; idx < typelistcount; ++idx)
        {
            #ifdef KROSS_RUBY_CALLCACHE_DEBUG
                krossdebug( QString("RubyCallCache::execfunction param idx=%1 inspect=%2 QVariantType=%3 QMetaType=%4").arg(idx).arg(STR2CSTR(rb_inspect(argv[idx]))).arg(QVariant::typeToName((QVariant::Type)d->types[idx])).arg(QMetaType::typeName(d->metatypes[idx])) );
            #endif

            MetaType* metatype = RubyMetaTypeFactory::create( typelist[idx-1], d->types[idx], d->metatypes[idx], argv[idx] );
            if(! metatype) { // Seems RubyMetaTypeFactory::create returned an invalid RubyType.
                krosswarning( QString("RubyCallCache::execfunction Aborting cause RubyMetaTypeFactory::create returned NULL.") );
                for(int i = 0; i < idx; ++i) // Clear already allocated instances.
                    delete variantargs[i];
                return Qfalse; // abort execution.
            }
            variantargs[idx] = metatype;
            voidstarargs[idx] = metatype->toVoidStar();
        }

        // call the method now
        int r = d->object->qt_metacall(QMetaObject::InvokeMetaMethod, d->methodindex, &voidstarargs[0]);
        #ifdef KROSS_RUBY_CALLCACHE_DEBUG
            krossdebug( QString("RESULT nr=%1").arg(r) );
        #else
            Q_UNUSED(r);
        #endif

        // the return value
        VALUE retvalue = 0;

        // eval the return-value
        if(d->hasreturnvalue)
        {
            QVariant result;

            if( Kross::MetaTypeHandler* handler = Kross::Manager::self().metaTypeHandler(d->metamethod.typeName()) ) {
                #ifdef KROSS_RUBY_CALLCACHE_DEBUG
                    krossdebug( QString("Returnvalue of type '%2' has a handler").arg(d->metamethod.typeName()) );
                #endif
                void *ptr = (*reinterpret_cast<void*(*)>( variantargs[0]->toVoidStar() ));
                result = handler->callHandler(ptr);
            }
            else {
                result = QVariant(variantargs[0]->typeId(), variantargs[0]->toVoidStar());

                if( ! Kross::Manager::self().strictTypesEnabled() ) {
                    if( result.type() == QVariant::Invalid && QByteArray(d->metamethod.typeName()).endsWith("*") ) {
                        //#ifdef KROSS_RUBY_CALLCACHE_DEBUG
                            krossdebug( QString("Returnvalue of type '%2' will be reinterpret_cast<QObject*>").arg(d->metamethod.typeName()) );
                        //#endif
                        QObject* obj = (*reinterpret_cast<QObject*(*)>( variantargs[0]->toVoidStar() ));
                        result.setValue( (QObject*) obj );
                    }
                }
            }

            #ifdef KROSS_RUBY_CALLCACHE_DEBUG
                krossdebug( QString("RubyCallCache::execfunction Returnvalue typeId=%1 metamethod.typename=%2 variant.toString=%3 variant.typeName=%4").arg(variantargs[0]->typeId()).arg(d->metamethod.typeName()).arg(result.toString()).arg(result.typeName()) );
            #endif

            // set the return value
            retvalue = RubyType<QVariant>::toVALUE(result);
        }

        // free the return-value and the arguments
        for(int idx = 0; idx < typelistcount; ++idx)
        {
            delete variantargs[idx];
        }

        return retvalue;
    }

    void RubyCallCache::delete_object(void* object)
    {
        #ifdef KROSS_RUBY_CALLCACHE_CTORDTOR_DEBUG
            krossdebug("RubyCallCache::delete_object");
        #endif
        RubyCallCache* callcache = static_cast< RubyCallCache* >(object);
        delete callcache;
        callcache = 0;
    }

    VALUE RubyCallCache::method_cacheexec(int argc, VALUE *argv, VALUE self)
    {
        #ifdef KROSS_RUBY_CALLCACHE_DEBUG
            krossdebug("RubyCallCache::method_cacheexec");
        #endif
        RubyCallCache* callcache;
        Data_Get_Struct(self, RubyCallCache, callcache);
        return callcache->execfunction(argc, argv);
    }

    VALUE RubyCallCache::toValue()
    {
        if(m_self == 0)
        {
            if(RubyCallCachePrivate::s_rccObject  == 0)
            {
                RubyCallCachePrivate::s_rccObject = rb_define_class_under(RubyInterpreter::krossModule(), "CallCache", rb_cObject);
                rb_define_method(RubyCallCachePrivate::s_rccObject, "cacheexec",  (VALUE (*)(...))RubyCallCache::method_cacheexec, -1);
            }
            m_self = Data_Wrap_Struct(RubyCallCachePrivate::s_rccObject, 0, RubyCallCache::delete_object, this);
        }
        return m_self;
    }

}
