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

#ifndef KROSS_FALCONERRHAND_H
#define KROSS_FALCONERRHAND_H

#include <falcon/errhand.h>
#include <kross/core/krossconfig.h>
#include <kross/core/interpreter.h>

namespace Kross {
    
    class KErrHandlerPrivate;
    
    /// Specific Falcon error handler, which sends the error description to the notified object
    class KErrHandler: public Falcon::ErrorHandler
    {
    public:
        KErrHandler( ErrorInterface *notified );
        virtual ~KErrHandler();
        virtual void handleError( Falcon::Error *error );
    
    private:
        /// Private d-pointer class.
        KErrHandlerPrivate * const d;
    };

}


#endif
