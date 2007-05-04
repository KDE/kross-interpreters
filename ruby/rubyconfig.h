/***************************************************************************
 * pythonconfig.h
 * This file is part of the KDE project
 * copyright (C)2005 by Cyrille Berger (cberger@cberger.net)
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

#ifndef KROSS_RUBY_CONFIG_H
#define KROSS_RUBY_CONFIG_H

#include <ruby.h>
#include <env.h>
#include <rubysig.h>
#include <node.h>
#include <version.h>

#include <st.h>
//#include <typeinfo>

#include <kross/core/krossconfig.h>

// If this is defined we call explicit rb_gc() manualy on many places. This is
// normaly not needed and Ruby will handle the garbage collection for you. So,
// while it's probably better/faster to don't rb_gc() manualy it's still useful
// for debugging to detect and catch problems faster.
#define KROSS_RUBY_EXPLICIT_GC

// Following defines are for debugging only. If defined, we are more verbose.
#define KROSS_RUBY_SCRIPT_CTORDTOR_DEBUG
//#define KROSS_RUBY_SCRIPT_CONNECTFUNCTION_DEBUG
//#define KROSS_RUBY_SCRIPT_CALLFUNCTION_DEBUG
//#define KROSS_RUBY_SCRIPT_FUNCTIONNAMES_DEBUG
//#define KROSS_RUBY_SCRIPT_EXECUTE_DEBUG
//#define KROSS_RUBY_SCRIPT_METHODADDED_DEBUG
//#define KROSS_RUBY_SCRIPT_DEBUG

//#define KROSS_RUBY_INTERPRETER_CTORDTOR_DEBUG
//#define KROSS_RUBY_INTERPRETER_DEBUG

#define KROSS_RUBY_EXTENSION_CTORDTOR_DEBUG
//#define KROSS_RUBY_EXTENSION_CALLCONNECT_DEBUG
//#define KROSS_RUBY_EXTENSION_DEBUG

#define KROSS_RUBY_CALLCACHE_CTORDTOR_DEBUG
//#define KROSS_RUBY_CALLCACHE_DEBUG

#define KROSS_RUBY_MODULE_CTORDTOR_DEBUG
//#define KROSS_RUBY_MODULE_DEBUG

//#define KROSS_RUBY_VARIANT_DEBUG

//#define KROSS_RUBY_FUNCTION_CTORDTOR_DEBUG
//#define KROSS_RUBY_FUNCTION_DEBUG

#endif
