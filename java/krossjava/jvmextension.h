/***************************************************************************
 * jvmextension.cpp
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

#ifndef KROSS_JVMEXTENSION_H
#define KROSS_JVMEXTENSION_H

#include "jvmconfig.h"

//#include <QStringList>
//#include <QVariant>
#include <QObject>

namespace Kross {

    /**
    * This class wraps a QObject instance into the world of Java. For this, we
    * provide a dynamic created jobject class that provides access to the QObject.
    *
    * More details how this maybe could be done are at;
    * \a http://java.sun.com/docs/books/jni/html/other.html#29535 Registering Native Methods
    * \a http://java.sun.com/docs/books/jni/html/other.html#30982 Reflection Support
    */
    class JVMExtension {
        public:

            /**
            * Constructor.
            *
            * @param object The QObject instance this extension provides access to.
            */
            explicit JVMExtension(QObject* object);

            /**
            * Destructor.
            */
            ~JVMExtension();

            /**
            * \return the QObject this \a RubyExtension wraps.
            */
            QObject* object() const;

        private:
            class Private;
            Private * const d;
    };

}

#endif
