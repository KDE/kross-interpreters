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

#ifndef KROSS_JVMCLASSWRITER_H
#define KROSS_JVMCLASSWRITER_H

#include "jvmconfig.h"

#include <QDataStream>

namespace Kross {

    class JVMExtension;

    /**
    * Writes Java bytecode.
    *
    * \see http://java.sun.com/docs/books/jvms/second_edition/html/ClassFile.doc.html
    */
    class JVMClassWriter
    {
        public:
            explicit JVMClassWriter(JVMExtension* extension);
            ~JVMClassWriter();
            void writeInterface(QDataStream& data);

        private:
            JVMExtension* m_extension;

            void writeHeader(QDataStream& data);
            void writeConstantPool(QDataStream& data);
            void writeClassInfo(QDataStream& data);
            void writeFields(QDataStream& data);
            void writeMethods(QDataStream& data);
            void writeAttributes(QDataStream& data);

            void writeUtf8ToPool(QDataStream& data, const QString& str);
            QString toJavaType(const QByteArray& type);
            qint16 toConstantpoolIndex(const QByteArray& type);
    };

}

#endif
