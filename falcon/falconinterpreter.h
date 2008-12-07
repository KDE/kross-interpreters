/***************************************************************************
 * falconinterpreter.h
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

#ifndef KROSS_FALCONINTERPRETER_H
#define KROSS_FALCONINTERPRETER_H

#include "falconconfig.h"
#include <kross/core/krossconfig.h>
#include <kross/core/interpreter.h>
#include <kross/core/action.h>
#include <kross/core/manager.h>

#include <QString>

namespace Kross {

    // Forward declarations.
    class FalconScript;
    class FalconModule;
    class FalconInterpreterPrivate;

    /**
     * Falcon interpreter bridge.
     *
     * Implements the \a Kross::Interpreter for the falcon interpreter
     * backend and provides with the \a Kross::FalconInterpreter::createScript
     * a factory method to create \a Kross::FalconScript instances.
     */
    class FalconInterpreter : public Interpreter
    {
        public:

            /**
             * Constructor.
             *
             * \param info The \a Kross::InterpreterInfo instance
             *        which describes the \a FalconInterpreter for
             *        applications using Kross.
             */
            explicit FalconInterpreter(InterpreterInfo* info);

            /**
             * Destructor.
             */
            virtual ~FalconInterpreter();

            /**
             * \return a \a FalconScript instance.
             */
            virtual Kross::Script* createScript(Action* Action);
            
            /**
             * \return the instance of the Core Module available to all the script.
             */
            virtual ::Falcon::Module* coreModule();
            
            /**
             * \return the instance of the Kross integration module available for all the scripts.
             */
            virtual ::Falcon::Module* krossModule();

        private:
            /// Private d-pointer class.
            FalconInterpreterPrivate * const d;
            
            /// Initialize the falcon interpreter.
            inline void initialize();
            /// Finalize the falcon interpreter.
            inline void finalize();
    };

}

#endif
