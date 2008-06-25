/***************************************************************************
 * falconprovider.h
 * This file is part of the KDE project
 * copyright (C)2004-2007 by jonnymind@falconpl.org
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

#ifndef KROSS_FALCONPROVIDER_H
#define KROSS_FALCONPROVIDER_H

#include <falcon/flexymodule.h>
#include <kross/core/krossconfig.h>
#include <kross/core/metafunction.h>
#include <kross/core/action.h>

namespace Kross {
    
    class FalconProviderPrivate;
    
    /**
    * Class Providing objects that the script requests.
    * 
    * This class is meant to enable scripts to ask for symbols at
    * runtime. It implements what is usually performed by request() or import()
    * callbacks in other scripting languages.
    */
    class FalconProvider : public Falcon::FlexyModule
    {
        public:

            /**
             * Constructor.
             *
             * \param action The \a Action instance that is bound to the parent script.
             */
            explicit FalconProvider(Action* action);

            /**
             * Destructor.
             */
            virtual ~FalconProvider();
            
            /**
            * Callback generated when the script needs a symbol.
            */
            virtual Falcon::Symbol *onSymbolRequest( const Falcon::String &name );

        private:
            /// Private d-pointer class.
            FalconProviderPrivate * const d;
    };
}

#endif
