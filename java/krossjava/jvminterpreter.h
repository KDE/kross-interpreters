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
    class JVMExtension;
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

            /**
            * \return A valid JNIEnv that can be used in the current thread.
            */
            static JNIEnv* getEnv();

            /**
            * \return The Java Virtual Machine used to run the Java code.
            */
            static JavaVM* getJVM();

            /**
            * Defines a new Java class. This can be used in two ways:
            * either by passing in the bytecode of a Java class, or the
            * data of a JAR file.
            *
            * \param name The name by which to register this class. This is
            * used to prevent double definitions of the same code.
            * \param data The Java bytecode that makes up this class or
            * the raw contents of a JAR file.
            * \return The Java class name of the executable script, or the
            * empty string "" if none could be found. For class data, this
            * returns the (real) classname, for JAR data, it returns the
            * value of the "Kross-Main" attribute in the manifest.
            */
            static QString addClass(const QString& name, const QByteArray& data);

            /**
            * Adds a URL to the Java classpath.
            *
            * \param url A URL to add to the search path when looking for
            * class definitions.
            */
            static void addToCP(const QUrl& url);

            /**
            * Creates a new Java object of the given class. If an exception
            * occurred, it will be handled.
            *
            * \param name The name of a Java class.
            * \return A new object of the given class, or NULL if an
            * exception was thrown while attempting to create the object.
            */
            static jobject newObject(const QString& name);

            /**
            * Registers a \a JVMExtension. This creates a Java object to
            * mirror the C object and stores the links between the
            * involved objects.
            *
            * \param name The name of the extension.
            * \param obj The JVMExtension to bridge to Java.
            * \param clazz The raw data of the Java class.
            * \param wrapped The QObject wrapped by \p obj.
            * \return A KrossQExtension mirrorring the JVMExtension in Java.
            */
            static jobject addExtension(const QString& name, const JVMExtension* obj, const QByteArray& clazz, const QObject* wrapped);

            /**
            * Returns the registered JVMExtension associated to a QObject.
            * \param obj A bridged QObject.
            * \return The JVMExtension associated with this QObject, or 0
            * if none could be found.
            */
            static const JVMExtension* extension(const QObject* obj);

            /**
            * Checks to see if an exception has occurred in the Java world.
            * If so, it clears the exception state and prints out the
            * stacktrace of the thrown exception.
            *
            * \return True if an exception occurred, false if not.
            */
            static bool handleException();
#ifdef KROSS_JVM_INTERPRETER_DEBUG
            /**
            * Prints some debug information about a Java object. Printed
            * are the toString() method and the runtime class.
            *
            * \param obj A Java object to know more about.
            */
            static void showDebugInfo(jobject obj);
#endif

        private:
            /// \internal d-pointer class.
            class Private;

            //TODO: is it okay for a d-pointer to be static?
            //sebsauer: normaly it isn't since if there are multiple instances of the JVMInterpreter
            //          around they would need to share it. While in our case the JVMInterpreter class
            //          is a singleton and therefore static should be ok, it may better to use
            //          K_GLOBAL_STATIC ( http://www.englishbreakfastnetwork.org/apidocs/apidox-kde-4.0/kdelibs-apidocs/kdecore/html/group__KDEMacros.html#g75ca0c60b03dc5e4f9427263bf4043c7 )
            //          but JNIEnv* shouldn't be static anyway.

            /// \internal d-pointer instance.
            static Private * const d;
    };

}

#endif
