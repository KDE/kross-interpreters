/***************************************************************************
* falconerrhand.cpp
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

#include "falconerrhand.h"
#include <falcon/autowstring.h>
#include <falcon/autocstring.h>
#include <QTextStream>

namespace Kross {

    class KErrHandlerPrivate {
        public:
            /// The (Kross) error interface on which we're going to publish the erroneous event on.
            ErrorInterface *m_notified;
        
            KErrHandlerPrivate( ErrorInterface *notified ):
                m_notified( notified )
            {}
    };


    KErrHandler::KErrHandler( ErrorInterface *notified ):
        d( new KErrHandlerPrivate( notified ) )
    {}
        
    void KErrHandler::handleError( Falcon::Error *error )
    {
        // convert error to string without traceback.
        Falcon::String errorDesc;
        error->heading( errorDesc );
        
        // Get the trace steps;
        QString sTraceback;
        QTextStream traceStream(&sTraceback);
        
        Falcon::String sModule, sSymbol;
        Falcon::uint32 line, pc;
        
        error->rewindStep();
        while( error->nextStep( sModule, sSymbol, line, pc ) )
        {
            Falcon::AutoWString mod( sModule );
            Falcon::AutoWString sym( sSymbol );
            traceStream << mod.w_str() << "." << sym.w_str() << ": " << line << " (PC:" << pc << ")\n";
        }
        
        // send the error description to the notified object
        Falcon::AutoCString ed( errorDesc );
        
        d->m_notified->setError( QString().fromUtf8( ed.c_str() ), sTraceback, error->line() > 0 ? error->line() : -1 );
    }
    
    
    KErrHandler::~KErrHandler()
    {
        delete d;
    }

}

