/***************************************************************************
 * jvmscript.cpp
 * This file is part of the KDE project
 * copyright (C)2007 by Sebastian Sauer (mail@dipe.org)
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

#include "jvmscript.h"
#include "jvminterpreter.h"
#include <kross/core/action.h>

using namespace Kross;

namespace Kross {

    /// \internal
    class JVMScript::Private
    {
        public:
            jobject scriptobj;

        explicit Private() : scriptobj(0) {

        }
    };

}

JVMScript::JVMScript(Interpreter* interpreter, Action* action)
    : Script(interpreter, action), d(new Private())
{
    krossdebug("JVMScript Ctor");
}

JVMScript::~JVMScript()
{
    krossdebug("JVMScript Dtor");
    delete d;
}

void JVMScript::execute()
{
    //TODO: if the code is not compiled (aka a class) we need to do it
    //before somehow...

    krossdebug("JVMScript execute");
    krossdebug( QString("executing file %1").arg(action()->file()) );

    //TODO: multiple scripts can run the same time and each of them would
    //need an own environment. So, probably just move the JNIEnv setup
    //to the JVMScript class?
    JNIEnv* env = static_cast< JVMInterpreter* >( interpreter() )->getEnv();

    //TODO: get a better way to extract the filename :)
    //TODO: handle also the case if we don't got a file but just some
    //action()->code() which then need to be compiled before...
    QString classname = action()->file().section('.',0,0);
    jclass cls = env->FindClass( classname.toAscii().data() );
    if (cls == 0) {
      krosswarning( QString("Class '%1' not found!").arg(classname) );
      return;
    }
    jmethodID ctor = env->GetMethodID(cls, "<init>", "()V");
    if (ctor == 0) {
      krosswarning("Constructor not found!");
      return;
    }
    //This might need a global reference...
    d->scriptobj = env->NewObject(cls, ctor);
}
