/***************************************************************************
 * jvmscript.cpp
 * This file is part of the KDE project
 *
 * copyright (C)2007 by Vincent Verhoeven <verhoevenv@gmail.com>
 * copyright (C)2007 by Sebastian Sauer <mail@dipe.org>
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
            JNIEnv* env;

        explicit Private() : scriptobj(0), env(0) {

        }
    };

}

JVMScript::JVMScript(Interpreter* interpreter, Action* action)
    : Script(interpreter, action), d(new Private())
{
    krossdebug("JVMScript Ctor");

    //TODO: multiple scripts can run the same time and each of them would
    //need an own environment. So, probably just move the JNIEnv setup
    //to the JVMScript class?
    d->env = static_cast< JVMInterpreter* >( interpreter )->getEnv();
}

JVMScript::~JVMScript()
{
    krossdebug("JVMScript Dtor");
    if(d->env && d->scriptobj){
         d->env->DeleteGlobalRef(d->scriptobj);
         d->scriptobj = 0;
    }
    delete d;
}

void JVMScript::execute()
{
    //TODO: if the code is not compiled (aka a class) we need to do it
    //before somehow...

    krossdebug( QString("JVMScript executing file: %1").arg(action()->file()) );

    //TODO: get a better way to extract the filename :)
    //TODO: handle also the case if we don't got a file but just some
    //action()->code() which then need to be compiled before...
    //TODO: in case of failure, one should look at possible Java
    //exceptions being thrown
    QString classname = action()->file().section('.',0,0);
    jclass cls = d->env->FindClass( classname.toAscii().data() );
    if (cls == 0) {
      krosswarning( QString("Class '%1' not found!").arg(classname) );
      return;
    }
    jmethodID ctor = d->env->GetMethodID(cls, "<init>", "()V");
    if (ctor == 0) {
      krosswarning("Constructor not found!");
      return;
    }
    jobject scriptweak = d->env->NewObject(cls, ctor);
    if (scriptweak == 0) {
      krosswarning("Could not create new Java script object!");
      return;
    }
    //Global reference to keep the object from being garbage collected
    d->scriptobj = d->env->NewGlobalRef(scriptweak);

    //TODO how to "publish our own jobject instance" here? we may like
    //to create an publish an instance of JVMExtension here that the
    //Java code can use/access then...
}
