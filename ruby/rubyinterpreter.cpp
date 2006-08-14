/***************************************************************************
 * rubyinterpreter.cpp
 * This file is part of the KDE project
 * copyright (C)2005 by Cyrille Berger (cberger@cberger.net)
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
#include "rubyinterpreter.h"

#include <map>

#include <QRegExp>
#include <ksharedptr.h>

#include "../core/exception.h"
#include "../core/module.h"
#include "../core/manager.h"

#include "rubyconfig.h"
#include "rubyextension.h"
#include "rubymodule.h"
#include "rubyscript.h"

extern "C"
{
    /**
     * Exported and loadable function as entry point to use
     * the \a RubyInterpreter.
     * The krossruby library will be loaded dynamicly at runtime from e.g.
     * \a Kross::Manager::getInterpreter and this exported
     * function will be used to return an instance of the
     * \a RubyInterpreter implementation.
     */
    void* krossinterpreter(Kross::InterpreterInfo* info)
    {
#ifdef KROSS_RUBY_INTERPRETER_DEBUG
        krossdebug("krossinterpreter(info)");
#endif
        try {
            return new Kross::Ruby::RubyInterpreter(info);
        }
        catch(Kross::Exception::Ptr e) {
            Kross::krosswarning("krossinterpreter(Kross::InterpreterInfo* info): Unhandled exception.");
        }
        return 0;
    }
};


namespace Kross {

namespace Ruby {
typedef std::map<QString, VALUE> mStrVALUE;
typedef mStrVALUE::iterator mStrVALUE_it;
typedef mStrVALUE::const_iterator mStrVALUE_cit;
class RubyInterpreterPrivate {
    friend class RubyInterpreter;
};
    
RubyInterpreterPrivate* RubyInterpreter::d = 0;
    
RubyInterpreter::RubyInterpreter(Kross::InterpreterInfo* info): Kross::Interpreter(info)
{
#ifdef KROSS_RUBY_INTERPRETER_DEBUG
    krossdebug("RubyInterpreter::RubyInterpreter(info)");
#endif
    if(d == 0)
    {
        initRuby();
    }
    if(info->hasOption("safelevel") )
    {
        rb_set_safe_level( info->getOption("safelevel")->value.toInt() );
    } else {
        rb_set_safe_level(4); // if the safelevel option is undefined, set it to maximum level
    }
}


RubyInterpreter::~RubyInterpreter()
{
    finalizeRuby();
}


Kross::Script* RubyInterpreter::createScript(Kross::Action* Action)
{
    return new RubyScript(this, Action);
}

void RubyInterpreter::initRuby()
{
    d = new RubyInterpreterPrivate();
    ruby_init();
    ruby_init_loadpath();
    rb_define_global_function("require", (VALUE (*)(...))RubyInterpreter::require, 1);
}

void RubyInterpreter::finalizeRuby()
{
    delete d;
    d = 0;
    ruby_finalize();
}

VALUE RubyInterpreter::require (VALUE obj, VALUE name)
{
#ifdef KROSS_RUBY_INTERPRETER_DEBUG
    krossdebug("RubyInterpreter::require(obj,name)");
#endif
    QString modname = StringValuePtr(name);
    if(modname.startsWith("kross")) {
        krossdebug( QString("RubyInterpreter::require() module=%1").arg(modname) );
        if( modname.indexOf( QRegExp("[^a-zA-Z0-9\\_\\-]") ) >= 0 ) {
            krosswarning( QString("Denied import of Kross module '%1' cause of untrusted chars.").arg(modname) );
        }
        else {
            Kross::Module::Ptr module = Kross::Manager::scriptManager()->loadModule(modname);
            if(module)
            {
                new RubyModule(module, modname);
//                     VALUE rmodule = rb_define_module(modname.ascii());
//                     rb_define_module_function();
//                     VALUE rm = RubyExtension::toVALUE(module);
//                     rb_define_variable( ("$" + modname).ascii(), & RubyInterpreter::d->m_modules.insert( mStrVALUE::value_type( modname, rm) ).first->second );
                return Qtrue;
            }
            krosswarning( QString("Loading of Kross module '%1' failed.").arg(modname) );
        }
    } else {
        return rb_f_require(obj, name);
    }
    return Qfalse;
}

}

}
