/***************************************************************************
 * pythonutils.h
 * copyright (C)2003 by Sebastian Sauer (mail@dipe.org)
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
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 ***************************************************************************/

#ifndef KROSS_PYTHONUTILS_H
#define KROSS_PYTHONUTILS_H

#include <Python.h>
#include "../CXX/Objects.hxx"
#include "../CXX/Extensions.hxx"

#include <qstring.h>
#include <qstringlist.h>
#include <qmap.h>
#include <qvariant.h>
#include <qvaluevector.h>

//#include <kdebug.h>
//#include <qguardedptr.h>

//#include <kexidb/driver.h>
//#include <kexidb/connection.h>

namespace Kross
{

    /**
     * The PythonUtils class is a storage for some global
     * needed functions.
     */
    class PythonUtils
    {
        public:

            /**
             * Check the number of defined Py::Tuple arguments
             * and throw an Py::Exception if they arn't between
             * minparams and maxparams.
             *
             * \param minparams Minimal parameters.
             * \param maxparams Maximal parameters.
             */
            static void checkArgs(const Py::Tuple& args, uint minparams, uint maxparams);

            /**
             * Converts a QString to a Py::Object. If
             * the QString isNull() then Py::None() will
             * be returned.
             *
             * \param s The QString to convert.
             * \return The to a Py::String converted QString.
             */
            static Py::Object toPyObject(const QString& s);

            /**
             * Converts a QStringList to a Py::List.
             *
             * \param list The QStringList to convert.
             * \return The to a Py::List converted QStringList.
             */
            static Py::List toPyObject(QStringList list);

            /**
             * Converts a QMap to a Py::Dict.
             *
             * \param map The QMap to convert.
             * \return The to a Py::Dict converted QMap.
             */
            static Py::Dict toPyObject(QMap<QString, QVariant> map);

            /**
             * Converts a QValueList to a Py::List.
             *
             * \param list The QValueList to convert.
             * \return The to a Py::List converted QValueList.
             */
            static Py::List toPyObject(QValueList<QVariant> list);

            /**
             * Converts a QValueVector to a Py::List.
             *
             * \param list The QValueVector to convert.
             * \return The to a Py::List converted QValueVector.
             */
            static Py::List toPyObject(QValueVector<QVariant> list);

            /**
             * Converts a QVariant to a Py::Object.
             *
             * \param variant The QVariant to convert.
             * \return The to a Py::Object converted QVariant.
             */
            static Py::Object toPyObject(QVariant variant);

            /**
             * Converts a Py::Object to uint. The function
             * does the check if the Py::Object is valid.
             *
             * \param obj The Py::Object to convert.
             * \return The to uint converted Py::Object.
             */
            static uint toUInt(Py::Object obj);

            /**
             * Convert a Py::Object to a QVariant.
             *
             * \param obj The Py::Object to convert.
             * \return The to QVariant converted Py::Object.
             */
            static QVariant toVariant(Py::Object obj);

        private:
            /// Private Constructor. Not needed to instanciate this class.
            PythonUtils() {}
    };

}

#endif
