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

    class JVMInterpreter;

    /**
    * This class wraps a QObject instance into the world of Java. For this, we
    * provide a dynamic created jobject class that provides access to the QObject.
    *
    * More details how this maybe could be done are at;
    * \li \a http://java.sun.com/docs/books/jni/html/jniTOC.html The Java Native Interface - Programmer's Guide and Specification
    * \li \a http://java.sun.com/docs/books/jni/html/other.html#29535 Registering Native Methods
    * \li \a http://java.sun.com/docs/books/jni/html/other.html#30982 Reflection Support
    */
    class JVMExtension {
        public:

            /**
            * Constructor.
            *
            * @param object The QObject instance this extension provides access to.
            */
            JVMExtension(QObject* object);

            /**
            * Destructor.
            */
            ~JVMExtension();

            /**
            * \return the QObject this \a JVMExtension wraps.
            */
            QObject* object() const;

            /**
            * \return the Java Object this \a JVMExtension provides.
            */
            jobject javaobject() const;

            /**
            * Handle a QObject function call.
            */
            jobject callQMethod(JNIEnv *env, jstring method, int numargs, jobject args[]);

            /**
            * Connect a signal of the wrapped QObject to a given Java method.
            */
            jboolean doConnect(JNIEnv *env, jstring signal, jobject receiver, jobject method);

            /**
            * \return true if the given jobject is a subclass of KrossQExtension and not NULL, false otherwise.
            */
            static bool isJVMExtension(jobject obj, JNIEnv* env);

        private:
            /// \internal d-pointer class.
            class Private;
            /// \internal d-pointer instance.
            Private * const d;
    };

}

#endif
