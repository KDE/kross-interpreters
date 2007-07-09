/***************************************************************************
 * jvmextension.cpp
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

#include "jvmextension.h"
#include "jvminterpreter.h"
#include "jvmclasswriter.h"
#include "jvmvariant.h"
//#include <kross/core/metatype.h>

//#include <QMap>
#include <QString>
#include <QPointer>
#include <QFile>
//#include <QMetaObject>
//#include <QMetaMethod>
//#include <QHash>
//#include <QVarLengthArray>

using namespace Kross;

namespace Kross {

    /// @internal d-pointer class.
    class JVMExtension::Private {
        public:
            /// The wrapped QObject.
            QPointer<QObject> m_object;

            /// For debugging.
            QString debuginfo;
    };

}

JVMExtension::JVMExtension(JVMInterpreter* interpreter, const QString& name, QObject* object)
    : d(new Private())
{
    d->m_object = object;

    d->debuginfo = object ? QString("name=%1 class=%2").arg(object->objectName()).arg(object->metaObject()->className()) : "NULL";
    krossdebug(QString("JVMExtension Ctor %1").arg(d->debuginfo));

    //testcase (TODO: something like this should go into JVMExtension)
    QFile toclass( QString("%1.class").arg(name) ); //e.g. "TestObject.class"
    toclass.open(QIODevice::ReadOnly);
    interpreter->addExtension(name, this, toclass.readAll());
    toclass.close();

/*
    //this is a testcase
    QFile ifacefile( QString("My%1.class").arg(name) );
    ifacefile.open(QIODevice::WriteOnly);
    QDataStream data(&ifacefile);
    ClassFileWriter writer(this);
    writer.writeInterface(data);
    ifacefile.close();
*/

    //TODO what we could do here is to create a class on the fly and register native
    //callbacks using the env->RegisterNatives method to be able to provide a Java
    //wrapper that calls slots and get/set properties fo the QObject.
    /*
        const QMetaObject* metaobject = d->m_object->metaObject();

        { // initialize methods.
            const int count = metaobject->methodCount();
            for(int i = 0; i < count; ++i) {
                QMetaMethod member = metaobject->method(i);
                const QString signature = member.signature();
                const QByteArray name = signature.left(signature.indexOf('(')).toLatin1();
                if(! d->m_methods.contains(name))
                    d->m_methods.insert(name, i);
            }
        }

        { // initialize properties
            const int count = metaobject->propertyCount();
            for(int i = 0; i < count; ++i) {
                QMetaProperty prop = metaobject->property(i);
                d->m_properties.insert(prop.name(), i);
                if(prop.isWritable())
                    d->m_properties.insert(QByteArray(prop.name()).append('='), i);
            }
        }

        { // initialize enumerations
            const int count = metaobject->enumeratorCount();
            for(int i = 0; i < count; ++i) {
                QMetaEnum e = metaobject->enumerator(i);
                const int kc = e.keyCount();
                for(int k = 0; k < kc; ++k) {
                    const QByteArray name = e.key(k);
                    d->m_enumerations.insert(name, e.value(k));
                }
            }
        }
    */
}

JVMExtension::~JVMExtension()
{
    krossdebug( QString("JVMExtension Dtor %1").arg(d->debuginfo) );
    delete d;
}

QObject* JVMExtension::object() const
{
    return d->m_object;
}

jobject JVMExtension::callQMethod(JNIEnv* env, jstring method, jobjectArray args)
{
    QString mname = JavaType<QString>::toVariant(method,env);

    //TODO: arguments...
    QMetaObject::invokeMethod(d->m_object, mname.toAscii());

    return 0;
}

/*
ClassFile {
        u4 magic;
        u2 minor_version;
        u2 major_version;
        u2 constant_pool_count;
        cp_info constant_pool[constant_pool_count-1];
        u2 access_flags;
        u2 this_class;
        u2 super_class;
        u2 interfaces_count;
        u2 interfaces[interfaces_count];
        u2 fields_count;
        field_info fields[fields_count];
        u2 methods_count;
        method_info methods[methods_count];
        u2 attributes_count;
        attribute_info attributes[attributes_count];
    }
*/
