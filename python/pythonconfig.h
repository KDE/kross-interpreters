/***************************************************************************
 * pythonconfig.h
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

#ifndef KROSS_PYTHON_CONFIG_H
#define KROSS_PYTHON_CONFIG_H

// Prevent warnings
#if defined(_XOPEN_SOURCE)
  #undef _XOPEN_SOURCE
#endif
#if defined(_POSIX_C_SOURCE)
  #undef _POSIX_C_SOURCE
#endif

/* Prevent multiple conflicting definitions of swab from stdlib.h and unistd.h
See also http://bugs.kde.org/show_bug.cgi?id=156950 */
#if defined(__sun) || defined(sun)
  #if defined(_XPG4)
    #undef _XPG4
  #endif
#endif

// The Python.h needs to be included first.
#include <Python.h>
#include <object.h>
#include <compile.h>
#include <eval.h>
#include <frameobject.h>

#include <kross/core/krossconfig.h>

// Include the PyCXX stuff.
#include "CXX/Config.hxx"
#include "CXX/Objects.hxx"
#include "CXX/Extensions.hxx"
#include "CXX/Exception.hxx"

// The version of this python plugin. This will be exported
// to the scripting code. That way we're able to write
// scripting code for different incompatible Kross python
// bindings by checking the version. You should increment
// this number only if you really know what you're doing.
//#define KROSS_PYTHON_VERSION 1

// Enable debugging for Kross::PythonInterpreter
//#define KROSS_PYTHON_INTERPRETER_DEBUG

// Enable debugging for Kross::PythonScript
#define KROSS_PYTHON_SCRIPT_CTOR_DEBUG
#define KROSS_PYTHON_SCRIPT_DTOR_DEBUG
//#define KROSS_PYTHON_SCRIPT_INIT_DEBUG
//#define KROSS_PYTHON_SCRIPT_FINALIZE_DEBUG
#define KROSS_PYTHON_SCRIPT_EXEC_DEBUG
//#define KROSS_PYTHON_SCRIPT_CALLFUNC_DEBUG
#define KROSS_PYTHON_SCRIPT_AUTOCONNECT_DEBUG

// Enable debugging for Kross::PythonModule
//#define KROSS_PYTHON_MODULE_CTORDTOR_DEBUG
//#define KROSS_PYTHON_MODULE_IMPORT_DEBUG

// Enable debugging for Kross::PythonExtension
//#define KROSS_PYTHON_EXTENSION_CTORDTOR_DEBUG
//#define KROSS_PYTHON_EXTENSION_TOPYOBJECT_DEBUG
//#define KROSS_PYTHON_EXTENSION_GETATTR_DEBUG
//#define KROSS_PYTHON_EXTENSION_SETATTR_DEBUG
//#define KROSS_PYTHON_EXTENSION_CALL_DEBUG
//#define KROSS_PYTHON_EXTENSION_CONNECT_DEBUG
//#define KROSS_PYTHON_EXTENSION_NUMERIC_DEBUG

// Enable debugging for Kross::Function
//#define KROSS_PYTHON_FUNCTION_DEBUG

// Enable debugging for Kross::PythonType and Kross::PythonMetaTypeFactory
//#define KROSS_PYTHON_VARIANT_DEBUG

#define KROSS_PYTHON_EXCEPTION_DEBUG

#endif
