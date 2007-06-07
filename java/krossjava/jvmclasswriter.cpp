/***************************************************************************
 * jvmclasswriter.cpp
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

#include "jvmclasswriter.h"
#include "jvmextension.h"
//#include "jvminterpreter.h"
//#include <kross/core/metatype.h>

//#include <QMap>
#include <QString>
#include <QFile>
//#include <QMetaObject>
//#include <QMetaMethod>
//#include <QHash>
//#include <QVarLengthArray>

using namespace Kross;

JVMClassWriter::JVMClassWriter(JVMExtension* extension)
    : m_extension(extension)
{
}

JVMClassWriter::~JVMClassWriter()
{
}

void JVMClassWriter::writeInterface(QDataStream& data)
{
    writeHeader(data);
    writeConstantPool(data);
    //TODO
}

void JVMClassWriter::writeHeader(QDataStream& data)
{
    // The magic item supplies the magic number identifying the class file format; it has the
    // value 0xCAFEBABE.
    data << (qint32) 0xCAFEBABE; //magic, u4

    //The values of the minor_version and major_version items are the minor and major version
    //numbers of this class file.
    data << (qint16) 1; //major, u2
    data << (qint16) 6; //minor, u2
}

void JVMClassWriter::writeConstantPool(QDataStream& data)
{
    QObject* object = m_extension->object();
    Q_ASSERT(object);
    const QMetaObject* metaobject = object->metaObject();
    const int methodCount = metaobject->methodCount();

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
        //TODO
    }
}
