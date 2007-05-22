/***************************************************************************
 * jvminterpreter.cpp
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

#include "jvminterpreter.h"
#include "jvmscript.h"
#include "jvmvariant.h"

#include <kross/core/action.h>
#include <kross/core/manager.h>

// The in krossconfig.h defined KROSS_EXPORT_INTERPRETER macro defines an
// exported C function used as factory for Kross::JVMInterpreter instances.
KROSS_EXPORT_INTERPRETER( Kross::JVMInterpreter )

using namespace Kross;

namespace Kross {

    /// \internal
    class JVMInterpreter::Private
    {
        public:
            JNIEnv *env;
            JavaVM *jvm;
            JavaVMInitArgs vm_args;
            jobject classloader;
            jclass clclass;
            jmethodID addclass;
            jmethodID newinst;

            explicit Private() : env(0), jvm(0), classloader(0), addclass(0) {
                vm_args.version  = JNI_VERSION_1_2; /* Specifies the JNI version used */
                vm_args.ignoreUnrecognized = JNI_TRUE; /* JNI won't complain about unrecognized options */
            }
            bool initialize() {
                krossdebug("JVMInterpreter initialize");
                jint res = JNI_CreateJavaVM( &jvm, (void **)&env, &vm_args );
                if(res < 0)
                    return false;

                //Create classloader
                jclass clclass = env->FindClass( "org.kde.kdebindings.java.krossjava.KrossClassLoader" );
                if (clclass == 0) {
                  krosswarning( "Class 'KrossClassLoader' not found! Is kross.jar accessible?" );
                  return false;
                }
                addclass = env->GetMethodID(clclass, "addClass", "(Ljava/lang/String;[B)V");
                newinst = env->GetMethodID(clclass, "newInstance", "(Ljava/lang/String;)Ljava/lang/Object;");
                if (addclass == 0 || newinst == 0) {
                  krosswarning("Classloader method not found!");
                  return false;
                }
                jmethodID ctor = env->GetMethodID(clclass, "<init>", "()V");
                if (ctor == 0) {
                  krosswarning("Classloader constructor not found!");
                  return false;
                }
                jobject loaderweak = env->NewObject(clclass, ctor);
                if (loaderweak == 0) {
                  krosswarning("Could not create classloader!");
                  return false;
                }
                classloader = env->NewGlobalRef(loaderweak);

                return true;
            }
            bool finalize() {
                jint res = jvm->DestroyJavaVM();
                //return ( ! env->ExceptionOccurred() );
                //Note, this will always fail according to DestroyJavaVM specification
                return res >= 0;
            }
    };

}

JVMInterpreter::JVMInterpreter(InterpreterInfo* info)
    : Interpreter(info), d(new Private())
{
    krossdebug("JVMInterpreter Ctor");

    JavaVMOption    options[2];
    options[0].optionString = "-Djava.class.path=kross.jar:.:/myClassDir";
    options[1].optionString = "-Djava.library.path=.:/myLibDir";
    d->vm_args.options  = options;
    d->vm_args.nOptions = 2;

    if( ! d->initialize() ) {
        krosswarning("JVMInterpreter Ctor: Failed to initialize");
    }
}

JVMInterpreter::~JVMInterpreter()
{
    krossdebug("JVMInterpreter Dtor");
    if( ! d->finalize() ) {
        krosswarning("JVMInterpreter Ctor: Failed to finalize");
    }
    delete d;
}


Script* JVMInterpreter::createScript(Action* action)
{
    return new JVMScript(this, action);
}

JNIEnv* JVMInterpreter::getEnv() const
{
    return d->env;
}

bool JVMInterpreter::addClass(QString name, QByteArray array)
{
    jstring jname = JavaType<QString>::toJObject(d->env,name);
    jbyteArray jarray = JavaType<QByteArray>::toJObject(d->env,array);
    d->env->CallVoidMethod(d->classloader,d->addclass,jname,jarray);

    if(d->env->ExceptionOccurred()){
        d->env->ExceptionDescribe();
        return false;
    }
    return true;
}

//TODO: a way to add arguments? Would be hard though.
jobject JVMInterpreter::newObject(QString fullname)
{
    jstring jname = JavaType<QString>::toJObject(d->env,fullname);
    jobject obj = d->env->CallObjectMethod(d->classloader,d->newinst,jname);

    if(d->env->ExceptionOccurred()){
        d->env->ExceptionDescribe();
        return 0;
    }
    return obj;
}
