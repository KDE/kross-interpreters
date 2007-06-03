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
#include "jvmvariant.h"
#include "testobject.h" //testcase

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

    //this is a testcase to be able to test the QObject functionality.
    TestObject* testobject = new TestObject(this, "MyTestObject");
    action->addObject(testobject, "MyTestObject");

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

//testcase
jstring JNICALL nameMethodImpl(JNIEnv *env, jobject self)
{
    Q_UNUSED(self);

    //TODO we would need the instance pointer here + it should be generic + move such functionality into JVMExtension
    //QString s = action()->object("MyTestObject")->name();
    QString s = "MyTestObject";
    return JavaType<QString>::toJObject(s, env);
}

void JVMScript::execute()
{
    JVMInterpreter* jvmi = static_cast< JVMInterpreter* >( interpreter() );

    krossdebug( QString("JVMScript executing file: %1").arg(action()->file()) );

    //testcase
    jclass clazz = (jclass) jvmi->getEnv()->NewGlobalRef( jvmi->getEnv()->FindClass("TestObjectImpl") );
    JNINativeMethod nativeMethod;
    nativeMethod.name = "name";
    nativeMethod.signature = "()Ljava/lang/String;";
    nativeMethod.fnPtr = (void*) nameMethodImpl;
    jvmi->getEnv()->RegisterNatives(clazz, &nativeMethod, 1);

    QFileInfo file(action()->file());
    //TODO: in some cases, the classname might not be given.
    //We need to gather it from the code, then, I think.
    QString classname = file.completeBaseName();
    jvmi->addToCP(file.absolutePath());

    //Add the code to the classloader, which compiles it if needed
    jvmi->addClass(classname, action()->code());

    jobject scriptweak = jvmi->newObject(classname);
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
