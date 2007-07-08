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

    /**
    * The JVMInterpreter class implements a \a Kross::Interpreter for
    * the Java backend. At each time there exist exactly one instance
    * of the interpreter.
    */
    class JVMInterpreter : public Interpreter
    {
        public:

            /**
            * Constructor.
            * \param info the \a InterpreterInfo instance the Kross core
            * uses to provide details about the Java-interpreter without
            * actualy loading the krossjava library.
            */
            explicit JVMInterpreter(InterpreterInfo* info);

            /**
            * Destructor.
            */
            virtual ~JVMInterpreter();

            /**
            * Factory method that returns a \a JVMScript instance. There
            * can be 0..n instances of \a JVMScript around where each of
            * them represents an independend java project.
            *
            * \param action The \a Kross::Action that contains details
            * about the actual Java code we got e.g. from an application
            * that uses Kross.
            * \return The new \a JVMScript instance that implements a
            * abstract script container for the java language.
            */
            virtual Script* createScript(Action* action);

            //This should probably become a more local class
            JNIEnv* getEnv() const;
            bool addClass(const QString& name, const QByteArray& array);
            void addToCP(const QUrl& url);
            jobject newObject(const QString& name);
            bool addExtension(const QString& name, const QObject* obj, const QByteArray& clazz);
            //TODO: would this be the right place?
            static bool handleException(JNIEnv* env);

        private:
            /// \internal d-pointer class.
            class Private;
            /// \internal d-pointer instance.
            Private * const d;
    };

}

#endif
