/***************************************************************************
 * rubycallcache.h
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

#ifndef KROSS_RUBYCALLCACHE_H
#define KROSS_RUBYCALLCACHE_H

#include "rubyconfig.h"

#include <QVarLengthArray>
class QObject;
class QVariant;

namespace Kross {

    struct RubyCallCachePrivate;

    /**
    * The RubyCallCache class implements a cache for calling functions
    * within the RubyExtension::callMetaMethod() method.
    */
    class RubyCallCache
    {
        public:

            /**
            * Constructor.
            *
            * \param object The QObject that provides the method that
            * should be called.
            * \param methodindex The method-index. The methods are
            * accessible by there index cause of performance reasons.
            * \param hasreturnvalue If true then the method does
            * provide a return-value else the method doesn't provide
            * anything back (void).
            * \param ntypes Array of QVariant::Type numbers for the
            * return-value (index 0) and the arguments (index >=1).
            * \param nmetatypes Array of QMetaType::Type numbers for the
            * return-value (index 0) and the arguments (index >=1).
            */
            RubyCallCache(QObject* object, int methodindex, bool hasreturnvalue, QVarLengthArray<int> ntypes, QVarLengthArray<int> nmetatypes);

            /**
            * Destructor.
            */
            ~RubyCallCache();

            /**
            * Execute the method and pass \p argc numbers of
            * arguments as \p argv array to the method.
            */
            VALUE execfunction( int argc, VALUE *argv );

            /**
            * Executes the method using the call-cache.
            */
            VALUE toValue();

            /**
            * Static function that executes the method using the call-cache.
            */
            static VALUE method_cacheexec(int argc, VALUE *argv, VALUE self);

            /**
            * Called by Ruby if an call-cache object got destructed.
            */
            static void delete_object(void* object);

        private:
            RubyCallCachePrivate * const d;
            VALUE m_self;
    };

}

#endif
