/***************************************************************************
 * pythonmodule.cpp
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

#include "pythonmodule.h"
#include "pythoninterpreter.h"

#include <QRegExp>

using namespace Kross;

namespace Kross {

    /// @internal
    class PythonModulePrivate
    {
        public:

            /**
             * The \a PythonInterpreter instance this module is
             * part of.
             */
            PythonInterpreter* const m_interpreter;

            #ifdef KROSS_PYTHON_MODULE_CTORDTOR_DEBUG
                /// \internal string for debugging.
                QString debuginfo;
            #endif

            PythonModulePrivate(PythonInterpreter* const interpreter)
                : m_interpreter(interpreter) {}
    };

}

PythonModule::PythonModule(PythonInterpreter* interpreter)
    : Py::ExtensionModule<PythonModule>("__main__")
    , d(new PythonModulePrivate(interpreter))
{
    #ifdef KROSS_PYTHON_MODULE_CTORDTOR_DEBUG
        d->debuginfo = QString("name=%1").arg(name().c_str());
        krossdebug( QString("PythonModule Ctor %1").arg(d->debuginfo) );
    #endif

    add_varargs_method("_import", &PythonModule::import, "Kross import hook.");
    initialize("The PythonModule is the __main__ python environment used as global object namespace.");
}

PythonModule::~PythonModule()
{
    #ifdef KROSS_PYTHON_MODULE_CTORDTOR_DEBUG
        krossdebug( QString("PythonModule Dtor %1").arg(d->debuginfo) );
    #endif

    delete d;
}

Py::Dict PythonModule::getDict()
{
    return moduleDictionary();
}

Py::Object PythonModule::import(const Py::Tuple& args)
{
    if(args.size() >= 2) {
        QString modname = args[1].as_string().c_str();

        Py::ExtensionObject<PythonExtension> extobj( args[0] );
        PythonExtension* extension = extobj.extensionObject();
        Action* action = dynamic_cast< Action* >( extension->object() );

        #ifdef KROSS_PYTHON_MODULE_IMPORT_DEBUG
            krossdebug( QString("PythonModule::import() module=%1 action=%2").arg(modname).arg(action ? action->objectName() : "NULL") );
        #endif

        if( action && action->hasObject(modname) ) {
            #ifdef KROSS_PYTHON_MODULE_IMPORT_DEBUG
                krossdebug( QString("PythonModule::import() module=%1 is internal local child").arg(modname) );
            #endif
            QObject* object = action->object(modname);
            Q_ASSERT(object);
            return Py::asObject( new PythonExtension(object) );
        }
        if(Kross::Manager::self().hasObject(modname)) {
            #ifdef KROSS_PYTHON_MODULE_IMPORT_DEBUG
                krossdebug( QString("PythonModule::import() module=%1 is internal global child").arg(modname) );
            #endif
            QObject* object = Kross::Manager::self().object(modname);
            Q_ASSERT(object);
            return Py::asObject( new PythonExtension(object) );
        }
    }
    return Py::None();
}
