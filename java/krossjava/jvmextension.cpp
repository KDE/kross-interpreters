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
    QFile toiface( QString("%1.class").arg(name) ); //e.g. "TestObject.class"
    QFile toclass( QString("%1Impl.class").arg(name) ); //e.g. "TestObjectImpl.class"
    toiface.open(QIODevice::ReadOnly);
    toclass.open(QIODevice::ReadOnly);
    interpreter->addExtension(name, object, toiface.readAll(), toclass.readAll());
    toiface.close();
    toclass.close();


/*
    //this is a testcase
    QByteArray iface = createInterface();
    QFile ifacefile( QString("My%1.class").arg(name) );
    ifacefile.open(QIODevice::WriteOnly);
    ifacefile.write(iface, iface.length());
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
QByteArray JVMExtension::createInterface()
{
    const QMetaObject* metaobject = d->m_object->metaObject();
    const int methodCount = metaobject->methodCount();

    QByteArray bytecode;
    QDataStream data(&bytecode, QIODevice::WriteOnly);

    // The magic item supplies the magic number identifying the class file format; it has the
    // value 0xCAFEBABE.
    data << (qint32) 0xCAFEBABE; //magic, u4

    //The values of the minor_version and major_version items are the minor and major version
    //numbers of this class file.
    data << (qint16) 1; //major, u2
    data << (qint16) 6; //minor, u2

    //The value of the constant_pool_count item is equal to the number of entries in the
    //constant_pool table plus one.
    data << (qint16) methodCount + 1;

    //The constant_pool is a table of structures representing various string constants, class
    //and interface names, field names, and other constants that are referred to within the
    //ClassFile structure and its substructures.
    for(int i = 0; i < methodCount; ++i) {
        //The format of each constant_pool table entry is indicated by its first "tag" byte.
        //http://java.sun.com/docs/books/jvms/second_edition/html/ClassFile.doc.html#20080
        data << (qint8) 11; //CONSTANT_InterfaceMethodref
    }

    //TODO

    return bytecode;
}
