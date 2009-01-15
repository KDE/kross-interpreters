/***************************************************************************
 * rubyscript.h
 * This file is part of the KDE project
 * copyright (C)2005 by Cyrille Berger (cberger@cberger.net)
 * copyright (C)2006 by Sebastian Sauer (mail@dipe.org)
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

#ifndef KROSS_RUBYRUBYSCRIPT_H
#define KROSS_RUBYRUBYSCRIPT_H

#include "rubyconfig.h"
#include <kross/core/krossconfig.h>
#include <kross/core/script.h>

namespace Kross {

    class RubyModule;
    class RubyFunction;
    class RubyScriptPrivate;

    /**
    *  Handle ruby scripts. This class implements
    * \a Kross::Script for ruby.
    * @author Cyrille Berger
    */
    class RubyScript : public Kross::Script
    {
            friend class RubyScriptPrivate;
        public:

            /**
            * Constructor.
            *
            * @param interpreter The @a RubyInterpreter instance used to
            *        create this script.
            * @param action The @a Kross::Action
            *        instance this @a RubyScript does handle the
            *        backend-work for.
            */
            RubyScript(Kross::Interpreter* interpreter, Kross::Action* action);

            /**
            * Destructor.
            */
            ~RubyScript();

            /**
            * \return a \a RubyModule instance defined with the \p modname for
            * that wraps the QObject \p object . If there exist no such
            * module yet we create a new one and remember it.
            */
            RubyModule* module(QObject* object, const QString& modname);

            /**
            * \return true if the as argument passed \p value is a
            * \a RubyScript object else false is returned.
            */
            static bool isRubyScript(VALUE value);

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

            #if 0
            /**
            * Return a list of class types this script supports.
            */
            virtual const QStringList& getClassNames();

            /**
             * Create and return a new class instance.
             */
            virtual Kross::Object::Ptr classInstance(const QString& name);
            #endif

            /**
            * Connect a QObject signal with a Ruby method function.
            *
            * \param sender The QObject instance that sends the signal.
            * \param signal The signature of the signal the QObject emits.
            * \param method The callable ruby methods that should
            * be executed if the QObject emits the signal.
            */
            RubyFunction* connectFunction(QObject* sender, const QByteArray& signal, VALUE method);

        private:
            /// Private d-pointer.
            RubyScriptPrivate * const d;
    };

}

#endif
