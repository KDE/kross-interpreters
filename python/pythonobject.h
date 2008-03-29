/***************************************************************************
 * pythonobject.h
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

#ifndef KROSS_PYTHON_OBJECT_H
#define KROSS_PYTHON_OBJECT_H

#include "pythonconfig.h"
#include "pythonextension.h"
#include <kross/core/object.h>

#include <QString>
#include <QStringList>

namespace Kross {
    /**
     * The PythonObject class is used for Instances of Python
     * Classes by the \a PythonExtension class.
     */
    class PythonObject : public Kross::Object
    {
        public:

            explicit PythonObject();
            /**
             * Constructor.
             *
             * \param object The Py::Object this \a PythonObject
             *        provides access to.
             */
            explicit PythonObject(const Py::Object& object);

            /**
             * Destructor.
             */
            virtual ~PythonObject();

            /**
             * Call a method the object provides.
             *
             * \param name Name of the method.
             * \param args Optional list of arguments.
             * \return The call-result.
             */
            virtual QVariant callMethod(const QString& name,
                    const QVariantList& args = QVariantList());

            /**
             * Return a list of callable method-names this
             * object provides.
             *
             * \return List of supported calls.
             */
            virtual QStringList methodNames();

            /**
             * Return python object used in this object.
             *
             * \return python object.
             */
            Py::Object pyObject();

         private:
            /// Private d-pointer class.
            class Private;
            Private* const d;
    };
}

#endif
