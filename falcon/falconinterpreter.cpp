/***************************************************************************
 * falconinterpreter.cpp
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

#include "falconinterpreter.h"
#include "falconerrhand.h"
#include "falconscript.h"
#include "falconmodule.h"

// The in krossconfig.h defined KROSS_EXPORT_INTERPRETER macro defines an
// exported C function used as factory for Kross::FalconInterpreter instances.
KROSS_EXPORT_INTERPRETER( Kross::FalconInterpreter )

namespace Kross {

    class FalconInterpreterPrivate {
        public:
            /// Falcon module loader facilility
            Falcon::FlcLoader *m_loader;
            
            // Falcon interface for handling errors.
            Falcon::ErrorHandler *m_errHandler;
            
            /// Falcon standard module: the core module
            Falcon::Module *m_core_module;
            
            /// Module used by Falcon scripts being run by Kross
            Falcon::Module *m_kross_module;
            
            FalconInterpreterPrivate():
                m_loader(0),
                m_errHandler(0),
                m_core_module(0),
                m_kross_module(0)
            {}
    };

    FalconInterpreter::FalconInterpreter(InterpreterInfo* info)
        : Kross::Interpreter(info),
        d( new FalconInterpreterPrivate )
    {
        // Initialize the falcon interpreter.
        initialize();
        
        //First of all, we need a standard module loader.
        //TODO: set here extra load paths
        d->m_loader = new Falcon::FlcLoader( "." );
        
        // however, add system falcon paths, which are safe.
        d->m_loader->addFalconPath();
        
        // Then, tell that we're the ones listening for errors in compile/load steps
        d->m_errHandler = new Kross::KErrHandler( this );
        d->m_loader->errorHandler( d->m_errHandler );
        
        // we can create an instance of the built-in core module
        d->m_core_module = Falcon::core_module_init();
        
        // and get an instance of our module
        d->m_kross_module = CreateKrossModule();
        
        #ifdef KROSS_FALCON_INTERPRETER_DEBUG
            //TODO: Get Falcon Infos
            //krossdebug(QString("Falcon Version: %1").arg());
        #endif
    }
    
    FalconInterpreter::~FalconInterpreter()
    {
        // clear Falcon loader
        delete d->m_loader;
        delete d->m_errHandler;
        
        // remove our reference (and probably delete) for loaded modules.
        d->m_core_module->decref();
        
        d->m_kross_module->decref();
        delete d;
    }
    
    void FalconInterpreter::initialize()
    {
        // not necessary, but currently this is the only locale Falcon engine has.
        Falcon::setEngineLanguage( "C" );
        
        // If we had special KDE memory allocation functions, we should set them here.
        Falcon::EngineData data;
        
        // initialize the engine using gathered data.
        Falcon::Init( data );
    }
    
    void FalconInterpreter::finalize()
    {
        // nothing to do in Falcon.
    }
    
    Kross::Script* FalconInterpreter::createScript(Kross::Action* action)
    {
        //if(hadError()) return 0;
        return new FalconScript(this, action);
    }
    
    ::Falcon::Module* FalconInterpreter::coreModule()
    {
        return d->m_core_module;
    }
    
    ::Falcon::Module* FalconInterpreter::krossModule()
    {
        return d->m_kross_module;
    }

}
