/***************************************************************************
 * jvmscript.h
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

#ifndef KROSS_JVMSCRIPT_H
#define KROSS_JVMSCRIPT_H

#include "jvmconfig.h"
#include <kross/core/script.h>

namespace Kross {

    /**
    * The JVMScript class implements the \a Kross::Script class to provide
    * an abstract script containerfor the java language. 
    */
    class JVMScript : public Script
    {
        public:

            /**
            * Constructor.
            *
            * \param interpreter The \a JVMInterpreter instance used
            * to create this JVMScript instance.
            * \param action The \a Kross::Action that contains details
            * about the actual Java code we got e.g. from an application
            * that uses Kross.
            */
            explicit JVMScript(Interpreter* interpreter, Action* action);

            /**
            * Destructor.
            */
            virtual ~JVMScript();

            /**
            * Execute the java code.
            */
            virtual void execute();

            /**
            * Return a list of methodnames the java code provides.
            */
            virtual QStringList functionNames() {
                //TODO
                return QStringList();
            }

            /**
            * Call a method in the java code.
            */
            virtual QVariant callFunction(const QString& name, const QVariantList& args = QVariantList()) {
                //TODO
                Q_UNUSED(name);
                Q_UNUSED(args);
                return QVariant();
            }

            /**
             * Evaluate some scripting code.
             *
             * \param code The scripting code to evaluate.
             * \return The return value of the evaluation.
             */
            virtual QVariant evaluate(const QByteArray& code) {
                //TODO
                Q_UNUSED(code);
                return QVariant();
            }

        private:
            /// \internal d-pointer class.
            class Private;
            /// \internal d-pointer instance.
            Private * const d;
    };

}

#endif

