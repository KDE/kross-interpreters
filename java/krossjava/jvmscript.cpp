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
    krossdebug("JVMScript execute");
    krossdebug( QString("executing file %1").arg(action()->file()) );

    JNIEnv* env = ((JVMInterpreter*)interpreter())->getEnv();

    //TODO: get a better way to extract the filename :)
    jclass cls = env->FindClass( action()->file().section('.',0,0).toAscii().data() );
    if (cls == 0) {
      krosswarning("Class not found!");
      return;
    }
    jmethodID mid =
      env->GetStaticMethodID(cls, "main", "([Ljava/lang/String;)V");
    if (mid == 0) {
      krosswarning("main() method not found");
      return;
    }
    jstring jstr;
    jobjectArray args = env->NewObjectArray(1, env->FindClass("java/lang/String"), jstr);
    if (args == 0) {
      krosswarning("Could not create a new object - out of memory?");
      return;
    }
    env->CallStaticVoidMethod(cls, mid, args);
}
