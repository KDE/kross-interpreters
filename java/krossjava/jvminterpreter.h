/***************************************************************************
 * jvminterpreter.h
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

#ifndef KROSS_JVMINTERPRETER_H
#define KROSS_JVMINTERPRETER_H

#include "jvmconfig.h"

#include <kross/core/interpreter.h>

#include <QString>

namespace Kross {

    // Forward declarations.
    class Script;
    class Action;
    class JVMScript;
    class JVMModule;
    class JVMInterpreterPrivate;

    class JVMInterpreter : public Interpreter
    {
            friend class JVMScript;
        public:
            explicit JVMInterpreter(InterpreterInfo* info);
            virtual ~JVMInterpreter();
            virtual Script* createScript(Action* action);
        private:
            class Private;
            Private * const d;

            //This should probably become a more local class
            JNIEnv* getEnv() const;
            bool addClass(QString name, QByteArray array);
            jobject newObject(QString name);
    };

}

#endif
