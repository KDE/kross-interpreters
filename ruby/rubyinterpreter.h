/***************************************************************************
 * rubyinterpreter.h
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
#ifndef KROSS_RUBYRUBYINTERPRETER_H
#define KROSS_RUBYRUBYINTERPRETER_H

#include <ruby.h>

#include <api/interpreter.h>

namespace Kross {

namespace Ruby {

class RubyInterpreterPrivate;
/**
 * This class is the bridget between kross and Ruby
 * @author Cyrille Berger
 */
class RubyInterpreter : public Kross::Api::Interpreter
{
    public:
        RubyInterpreter(Kross::Api::InterpreterInfo* info);
        ~RubyInterpreter();

        virtual Kross::Api::Script* createScript(Kross::Api::ScriptContainer* scriptcontainer);
    public:
        /// Increase the number of ruby script which are executed at a given time.
        void incCountScript();
        /**
         * Decrease the number of script which are executed at a given time, when the number reaches zero
         * this function will unload the modules.
         */ 
        void decCountScript();

    private:
        void initRuby();
        void finalizeRuby();
        static VALUE require (VALUE, VALUE);
    private:
        static RubyInterpreterPrivate* d;
};

}

}

#endif