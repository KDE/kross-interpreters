/***************************************************************************
 * falconconfig.h
 * This file is part of the KDE project
 * copyright (C)2007-2008 by Giancarlo Niccolai (jonnymind@falconpl.org)
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

#ifndef KROSS_FALCON_CONFIG_H
#define KROSS_FALCON_CONFIG_H

// Prevent warnings
#if defined(_XOPEN_SOURCE)
  #undef _XOPEN_SOURCE
#endif
#if defined(_POSIX_C_SOURCE)
  #undef _POSIX_C_SOURCE
#endif

// Falcon stuff may be included at any point
#include <falcon/engine.h>
#include <kross/core/krossconfig.h>

// The version of this falcon plugin. This will be exported
// to the scripting code. That way we're able to write
// scripting code for different incompatible Kross falcon
// bindings by checking the version. You should increment
// this number only if you really know what you're doing.
//#define KROSS_FALCON_VERSION 1

// Enable debugging for Kross::FalconInterpreter
#define KROSS_FALCON_INTERPRETER_DEBUG

// Enable debugging for Kross::FalconScript
#define KROSS_FALCON_SCRIPT_CTOR_DEBUG
#define KROSS_FALCON_SCRIPT_DTOR_DEBUG
#define KROSS_FALCON_SCRIPT_INIT_DEBUG
#define KROSS_FALCON_SCRIPT_FINALIZE_DEBUG
#define KROSS_FALCON_SCRIPT_EXEC_DEBUG
#define KROSS_FALCON_VM_DEBUG
//#define KROSS_FALCON_SCRIPT_CALLFUNC_DEBUG
#define KROSS_FALCON_SCRIPT_AUTOCONNECT_DEBUG

// Enable debugging for Kross::FalconModule
#define KROSS_FALCON_MODULE_CTORDTOR_DEBUG
//#define KROSS_FALCON_MODULE_IMPORT_DEBUG

// Enable debugging for Kross::FalconExtension
//#define KROSS_FALCON_EXTENSION_CTORDTOR_DEBUG
//#define KROSS_FALCON_EXTENSION_TOPYOBJECT_DEBUG
//#define KROSS_FALCON_EXTENSION_GETATTR_DEBUG
//#define KROSS_FALCON_EXTENSION_SETATTR_DEBUG
//#define KROSS_FALCON_EXTENSION_CALL_DEBUG
//#define KROSS_FALCON_EXTENSION_CONNECT_DEBUG
//#define KROSS_FALCON_EXTENSION_NUMERIC_DEBUG

// Enable debugging for Kross::Function
//#define KROSS_FALCON_FUNCTION_DEBUG

// Enable debugging for Kross::FalconType and Kross::FalconMetaTypeFactory
//#define KROSS_FALCON_VARIANT_DEBUG

#define KROSS_FALCON_EXCEPTION_DEBUG

#endif
