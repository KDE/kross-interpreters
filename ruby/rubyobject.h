/***************************************************************************
 * rubyobject.h
 * This file is part of the KDE project
 * copyright (C)2004-2005 by Sebastian Sauer (mail@dipe.org)
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

#ifndef KROSS_RUBY_OBJECT_H
#define KROSS_RUBY_OBJECT_H

#include "rubyconfig.h"
#include <kross/core/object.h>

#include <QString>
#include <QStringList>

namespace Kross {

    /**
     * The RubyObject class is used for Instances of Ruby
     * Classes by the \a RubyExtension class.
     */
    class RubyObject : public Kross::Object
    {
        public:

            /**
             * Default constructor.
             */
            explicit RubyObject();

            /**
             * Constructor.
             *
             * \param object The Py::Object this \a RubyObject
             *        provides access to.
             */
            explicit RubyObject(const VALUE& object);

            /**
             * Destructor.
             */
            virtual ~RubyObject();

            /**
             * Pass a call to the object. Objects like \a Class
             * are able to handle call's by just implementating
             * this function.
             *
             * \param name Each call has a name that says what
             *        should be called. In the case of a \a Class
             *        the name is the functionname.
             * \param args The list of arguments passed to
             *        the call.
             * \return The call-result as QVariant
             */
            virtual QVariant callMethod(const QString& name,
                    const QVariantList& args = QVariantList());

            /**
             * Return a list of supported callable objects.
             *
             * \return List of supported calls.
             */
            virtual QStringList methodNames();

            /**
             * Return ruby object used in this object.
             *
             * \return ruby object.
             */
            VALUE rbObject();

         private:
            /// Private d-pointer class.
            class Private;
            Private* const d;
    };
}

#endif
