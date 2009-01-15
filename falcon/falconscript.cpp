/***************************************************************************
 * falconscript.cpp
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

#include "falconscript.h"
#include "falconinterpreter.h"
#include "falconkfvm.h"
#include <kross/core/action.h>

#include <QMetaObject>
#include <QMetaMethod>
#include "falconerrhand.h"

using namespace Kross;

namespace Kross {

    /// \internal
    class FalconScriptPrivate
    {
        public:
            /// The Falcon virtual machine running the script
            KFVM *m_vm;
            
            /// The topmost (main) module
            Falcon::Module *m_mainModule;
            
            /// true when initialization has been performed.
            bool m_bInitialized;
                        
            FalconScriptPrivate():
                m_vm(0),
                m_mainModule(0),
                m_bInitialized( false )
            {}
            
            ~FalconScriptPrivate() {
            }
    };

FalconScript::FalconScript(Kross::Interpreter* interpreter, Kross::Action* action)
    : Kross::Script(interpreter, action),
    d( new FalconScriptPrivate )
{
    #ifdef KROSS_FALCON_SCRIPT_CTOR_DEBUG
        krossdebug("FalconScript::Constructor.");
    #endif
    d->m_vm = new KFVM( action );
    d->m_vm->errorHandler( new KErrHandler( this ), true );
    
    #ifdef KROSS_FALCON_SCRIPT_CTOR_DEBUG
        krossdebug("FalconScript::Constructor Linking base modules.");
    #endif
    
    // Linking in the relevant modules
    d->m_vm->link( static_cast<FalconInterpreter *>( interpreter )->coreModule() );
    d->m_vm->link( static_cast<FalconInterpreter *>( interpreter )->krossModule() );
}

FalconScript::~FalconScript()
{
    #ifdef KROSS_FALCON_SCRIPT_DTOR_DEBUG
        krossdebug("FalconScript::Destructor.");
    #endif
    
    if ( d->m_mainModule != 0 )
        d->m_mainModule->decref();
        
    delete d->m_vm;
    delete d;
}


bool FalconScript::initialize()
{
    #ifdef KROSS_FALCON_SCRIPT_INIT_DEBUG
        krossdebug( QString("FalconScript::initialize") );
    #endif

    if(action()->code().isNull()) {
        setError( QString("Invalid scripting code for script '%1'").arg(action()->objectName()) );
        return false;
    }
    if(action()->objectName().isNull()) {
        setError( QString("Name for the script is invalid!") );
        return false;
    }

    Q_ASSERT( ! action()->objectName().isNull() );
    QFileInfo fi( action()->objectName() );
    QString n = QFileInfo(fi.absolutePath(), fi.baseName()).absoluteFilePath();
    
    // Falcon supports UNICODE filenames, and it can import from both UTF8 and UTF16 (and other encodings).
    // utf8 is less efficient, but utf16 may not be complete under some platforms.
    QByteArray filename = n.isNull() ? action()->objectName().toUtf8() : n.toUtf8();
    filename.replace('.','_'); // points are used as module-delimiters
    char* name = filename.data();

    // We need a separate loader, as the interpreter loader will raise errors on the interpreter object
    // an alternate strategy may be that of switching the handler
    Falcon::FlcLoader scriptLoader( "." );
    scriptLoader.addFalconPath();
    // we can exploit the same handler as the VM.
    scriptLoader.errorHandler( d->m_vm->errorHandler() );
    
    // I suppose the input is utf8...
    scriptLoader.sourceEncoding( "utf-8" );
    
    #ifdef KROSS_FALCON_SCRIPT_INIT_DEBUG
        krossdebug( QString("FalconScript::initialize() module='%1':\n").
            arg( QString(action()->code()) ) );
    #endif
        
    // create an input stream containing the code.
    // This String constructor just takes the input data as reference; there is no copy involved.
    // However, the data is copied by StringStream, and that is good: it seems that data() returns
    // a transient data.
    Falcon::StringStream input( Falcon::String(action()->code().data(), action()->code().size()) );

    d->m_mainModule = scriptLoader.loadSource( &input, "kross.stdin" );
    if ( d->m_mainModule == 0 )
    {
        #ifdef KROSS_FALCON_SCRIPT_INIT_DEBUG
            krossdebug( QString("FalconScript::initialize() module='%1' failed to load").
                arg( action()->objectName() ) );
        #endif
        // error already raised
        return false;
    }
    
    // Setting Falcon module information
    Falcon::String modName, modPath;
    modName.fromUTF8( name );
    modPath.fromUTF8( action()->file().toUtf8().data() );
    d->m_mainModule->name( modName );
    d->m_mainModule->path( modPath );

    #ifdef KROSS_FALCON_SCRIPT_INIT_DEBUG
        krossdebug( QString("FalconScript::initialize() name=%1 loaded; resolving dependencies").arg(action()->objectName()) );
    #endif
    
    // the runtime is used to load user modules; it inherits the loader error handler
    Falcon::Runtime runtime( &scriptLoader );
    if( ! runtime.addModule( d->m_mainModule ) )
    {
        #ifdef KROSS_FALCON_SCRIPT_INIT_DEBUG
            krossdebug( QString("FalconScript::initialize() name=%1 falied resolving dependencies").arg(action()->objectName()) );
        #endif
        d->m_mainModule->decref();
        d->m_mainModule = 0;
        return false;
    }
    
    // We loaded the module and all the explicit dependencies; time to link
    #ifdef KROSS_FALCON_SCRIPT_INIT_DEBUG
        krossdebug( QString("FalconScript::initialize() name=%1 linking").arg(action()->objectName()) );
    #endif
    
    if ( ! d->m_vm->link( &runtime ) )
    {
        #ifdef KROSS_FALCON_SCRIPT_INIT_DEBUG
            krossdebug( QString("FalconScript::initialize() name=%1 linking failed").arg(action()->objectName()) );
        #endif
        return false;
    }
    
    // falcon modules are usually very interested in knowing about themselves...
    Falcon::Item *i_scriptName = d->m_vm->findGlobalItem( "scriptName" );  // is in core module...
    Q_ASSERT( i_scriptName != 0 );
    *i_scriptName = new Falcon::GarbageString( d->m_vm, modName );
    
    Falcon::Item *i_scriptPath = d->m_vm->findGlobalItem( "scriptPath" );  // is in core module...
    Q_ASSERT( i_scriptPath != 0 );
    *i_scriptPath = new Falcon::GarbageString( d->m_vm, modPath );
    
    //we're ready to fire.
    d->m_bInitialized = true;
    return true;
}


void FalconScript::execute()
{
    #ifdef KROSS_FALCON_SCRIPT_EXEC_DEBUG
        krossdebug( QString("FalconScript::execute") );
    #endif

    if( hadError() ) {
        #ifdef KROSS_FALCON_SCRIPT_EXEC_DEBUG
            krosswarning( QString("FalconScript::execute Abort cause of prev error: %1\n%2").arg(errorMessage()).arg(errorTrace()) );
        #endif
        return;
    }

    #ifdef KROSS_FALCON_SCRIPT_EXEC_DEBUG
        krossdebug( QString("FalconScript::execute Initializing") );
    #endif
    if( ! d->m_bInitialized ) { // initialize if not already done before.
        if(! initialize() )
            return;
    }
    
    Q_ASSERT( d->m_mainModule != 0 );

    #ifdef KROSS_FALCON_SCRIPT_EXEC_DEBUG
        krossdebug( QString("FalconScript::execute Launching") );
    #endif
    
    d->m_vm->launch();
}

QStringList FalconScript::functionNames()
{
    if(! d->m_bInitialized ) { // initialize if not already done before.
        if(! initialize())
            return QStringList();
    }
    
    QStringList functions;
    const Falcon::Map &stab = d->m_mainModule->symbolTable().map();
    
    Falcon::MapIterator iter = stab.begin();
    while( iter.hasCurrent() )
    {
        Falcon::Symbol *sym = *(Falcon::Symbol **) iter.currentValue();
        
        // We require the symbols to be functions and exported to be called.
        if( sym->isFunction() && sym->exported() )
        {
            Falcon::AutoCString funcName( sym->name() );
            functions.append( QString().fromUtf8(funcName.c_str()) );
        }
        iter.next();
    }
    
    return functions;
}

QVariant FalconScript::callFunction(const QString& name, const QVariantList& args)
{
    #ifdef KROSS_FALCON_SCRIPT_CALLFUNC_DEBUG
        QString s;
        foreach(QVariant v, args)
            s += v.toString() + ',';
        krossdebug( QString("FalconScript::callFunction() name=%1 args=[%2]").arg(name).arg(s) );
    #endif

    if( hadError() ) {
        #ifdef KROSS_FALCON_SCRIPT_CALLFUNC_DEBUG
            krosswarning( QString("FalconScript::callFunction() Abort cause of prev error: %1\n%2").arg(errorMessage()).arg(errorTrace()) );
        #endif
        return QVariant();
    }

    if(! d->m_bInitialized ) { // initialize if not already done before.
        #ifdef KROSS_FALCON_SCRIPT_CALLFUNC_DEBUG
            krossdebug( QString("FalconScript::callFunction() Initializing") );
        #endif
        if( ! initialize() )
            return QVariant();
    }
    
    // find the function in the main module.
    // Find the callable symbol
    Falcon::String funcName;
    funcName.fromUTF8( name.toUtf8() );
    Falcon::Item *callable = d->m_vm->findGlobalItem( funcName );
    
    if ( callable == 0 ) 
    {
        // the symbol is not exported, or it does not exist.
        setError( QString( "Kross: Symbol '%s' not found" ).arg( name ) );
        return QVariant();
    }
    
    if ( ! callable->isCallable() )
    {
        // The symbol exists, but it has currently some non-callable value associated.
        setError( QString( "Kross: Symbol '%s' is not a callable item." ).arg( name ) );
        return QVariant();
    }
    
    for ( QVariantList::size_type i = 0; i < args.size(); i ++ )
    {
        Falcon::Item item;
        
        // there may be a conversion error
        if( ! d->m_vm->variantToItem( args[i], item ) )
        {
            // in that case, the error has already been raised and stored in our lastError.
            // we just have to return
            return QVariant();
        }

        d->m_vm->pushParameter( item );
    }
    
    d->m_vm->callItem( *callable, args.size() );
    
    // now let's convert the return value into our return variant
    QVariant retvar;
    if ( ! d->m_vm->itemToVariant( d->m_vm->regA(), retvar ) )
    {
        // again, conversion error (already logged)
        return QVariant();
    }
    
    return retvar;
}

QVariant FalconScript::evaluate(const QByteArray& code)
{
    //TODO
    Q_UNUSED(code);
    return QVariant();
}

}

