/***************************************************************************
 * jvmexception.cpp
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

#ifndef KROSS_JVMEXCEPTION_H
#define KROSS_JVMEXCEPTION_H

#include "jvmconfig.h"

//#include <QStringList>
//#include <QVariant>
#include <QObject>

namespace Kross {

    /**
    * Holds convenience methods to throw various Java exceptions.
    */
    class JVMException {
        public:

            /**
            * Throws a java.io.IOException.
            * @param env the JNIEnv in which to throw the exception
            */
            static void throwIOException(JNIEnv *env) {
                jclass clazz = env->FindClass("java/io/IOException");
                jmethodID method = env->GetMethodID(clazz, "<init>", "()V");
                jthrowable throwObj = (jthrowable) env->NewObject(clazz, method);
                env->Throw(throwObj);
            }

            /**
            * Throws a java.io.EOFException.
            * @param env the JNIEnv in which to throw the exception
            */
            static void throwEOFException(JNIEnv *env) {
                jclass clazz = env->FindClass("java/io/EOFException");
                jmethodID method = env->GetMethodID(clazz, "<init>", "()V");
                jthrowable throwObj = (jthrowable) env->NewObject(clazz, method);
                env->Throw(throwObj);
            }

            /**
            * Throws a java.lang.ArrayIndexOutOfBoundsException.
            * @param env the JNIEnv in which to throw the exception
            */
            static void throwArrayIndexOutOfBoundsException(JNIEnv *env) {
                jclass clazz = env->FindClass("java/lang/ArrayIndexOutOfBoundsException");
                jmethodID method = env->GetMethodID(clazz, "<init>", "()V");
                jthrowable throwObj = (jthrowable) env->NewObject(clazz, method);
                env->Throw(throwObj);
            }

            /**
            * Throws a java.lang.NullPointerException.
            * @param env the JNIEnv in which to throw the exception
            */
            static void throwNullPointerException(JNIEnv *env) {
                jclass clazz = env->FindClass("java/lang/NullPointerException");
                jmethodID method = env->GetMethodID(clazz, "<init>", "()V");
                jthrowable throwObj = (jthrowable) env->NewObject(clazz, method);
                env->Throw(throwObj);
            }

            /**
            * Throws a java.lang.IllegalArgumentException.
            * @param env the JNIEnv in which to throw the exception
            */
            static void throwIllegalArgumentException(JNIEnv *env) {
                jclass clazz = env->FindClass("java/lang/IllegalArgumentException");
                jmethodID method = env->GetMethodID(clazz, "<init>", "()V");
                jthrowable throwObj = (jthrowable) env->NewObject(clazz, method);
                env->Throw(throwObj);
            }
    };

}

#endif
