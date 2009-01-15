/***************************************************************************
 * falconscript.h
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

#ifndef KROSS_FALCONSCRIPT_H
#define KROSS_FALCONSCRIPT_H

#include "falconconfig.h"
#include <kross/core/script.h>

namespace Kross {

    class FalconScriptPrivate;
    
    /**
     * Falcon script.
     *
     * The FalconScript class implements @a Kross::Script for the
     * falcon backend to provide the functionality to execute
     * falcon code within a script-container.
     */
    class FalconScript : public Kross::Script
    {
        public:

            /**
             * Constructor.
             *
             * \param interpreter The \a Kross::Falcon::FalconInterpreter used
             *       to create this FalconScript instance.
             * \param action The with this FalconScript associated
             *       \a Kross::Action instance that spends us
             *       e.g. the falcon scripting code.
             */
            explicit FalconScript(Kross::Interpreter* interpreter, Kross::Action* action);

            /**
             * Destructor.
             */
            virtual ~FalconScript();

            /**
             * Execute the script.
             */
            virtual void execute();

            /**
             * \return the list of functionnames.
             */
            virtual QStringList functionNames();

            /**
             * Call a function in the script.
             *
             * \param name The name of the function which should be called.
             * \param args The optional list of arguments.
             */
            virtual QVariant callFunction(const QString& name, const QVariantList& args = QVariantList());

            /**
             * Evaluate some scripting code.
             *
             * \param code The scripting code to evaluate.
             * \return The return value of the evaluation.
             */
            virtual QVariant evaluate(const QByteArray& code);

        private:
            /// Private d-pointer class.
            FalconScriptPrivate * const d;

            /// Initialize the script.
            bool initialize();
    };

}

#endif

