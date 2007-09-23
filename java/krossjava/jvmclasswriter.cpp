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
#include <QMetaObject>
#include <QMetaMethod>
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
    writeClassInfo(data);
    writeFields(data);
    writeMethods(data);
    writeAttributes(data);
}

void JVMClassWriter::writeHeader(QDataStream& data)
{
    // The magic item supplies the magic number identifying the class file format; it has the
    // value 0xCAFEBABE.
    data << (qint32) 0xCAFEBABE; //magic, u4

    //The values of the minor_version and major_version items are the minor and major version
    //numbers of this class file.
    data << (qint16) 0; //minor, u2
    data << (qint16) 50; //major, u2
}

void JVMClassWriter::writeConstantPool(QDataStream& data)
{

    /*
    * We will be using the following conventions for the constant pool. #n refers to a pool index.
    * #1 is the classname of this class, as a CONSTANT_Class_info structure.
    * #2 is the textual representation of the classname, as CONSTANT_Utf8_info.
    * #3 and #4 are the classinfo and name of the superclass (KrossQExtension).
    * #5 is "<init>".
    * #6 is "(Ljava/lang/Long;)V", the signature of the constructor.
    * #7 is "Code", used to identify the Code attribute of a method.
    * #8 is the superconstructor, #3.#9.
    * #9 is the signature of the constructor, #5:#6.
    * #10 is "(Ljava/lang/String;)Ljava/lang/Object;"
    * #11 is "(Ljava/lang/String;Ljava/lang/Object;)Ljava/lang/Object;"
    * This continues to #20, which has 10 Objects as arguments.
    * #21 is "invoke".
    * #22 is the signature invoke:(Ljava/lang/String;)Ljava/lang/Object;, #21:#10
    * Up until #32, which is the signature with 10 Objects as argments.
    * #33 is this.invoke(String), or #1.#22
    * Until #43, which is this.invoke(String,Object*10), or #1.#32
    * #44 is "java/lang/Boolean"
    * #45 is "java/lang/Integer"
    * #46 is "java/lang/Long"
    * #47 is "java/lang/Double"
    * #48 is "java/lang/String"
    * #49 is "java/util/ArrayList"
    * #50 is "java/util/Map"
    * #51 is "java/net/URL"
    * #52 is "[B" (byte array)
    * #53 is "[I" (int array)
    * #54 is "[D" (double array)
    * #55 is "[Ljava/lang/String;"
    * #56 until #67 are the classes corresponding to #44 to #55.
    * (note that the return type of KrossQObject is to be found in #3)
    * Starting with #68 comes, for each method:
    *   1) UTF8 name of method
    *   2) Constant string of the name (used in the code)
    *   3) UTF8 signature description
    *
    * It might be posible to reduce the size of the generated constant pool
    * by keeping track of generated signature descriptions and using a more
    * advanced technique to reuse them where possible, but the gains seem small.
    * Doing this might make this code a bit more maintainable, though.
    */

    QObject* object = m_extension->object();
    Q_ASSERT(object);
    const QMetaObject* metaobject = object->metaObject();
    const int methodCount = metaobject->methodCount();

    //Constant_pool_count is last index + 1, see verbose description above for the numbers
    data << (qint16) (67 + 3 * methodCount + 1);
    //Class
    data << (qint8) 7; //CONSTANT_Class
    data << (qint16) 2; //index of name - next pool item
    writeUtf8ToPool(data,object->objectName());
    //Superclass
    data << (qint8) 7; //CONSTANT_Class
    data << (qint16) 4; //index of name - next pool item
    writeUtf8ToPool(data,"org/kde/kdebindings/java/krossjava/KrossQExtension");

    //Constructor
    writeUtf8ToPool(data,"<init>");
    writeUtf8ToPool(data,"(Ljava/lang/Long;)V");
    writeUtf8ToPool(data,"Code");

    data << (qint8) 10; //CONSTANT_Methodref
    data << (qint16) 3; //Classindex, KrossQExtension
    data << (qint16) 9; //NameAndTypeRef, see next one
    data << (qint8) 12; //CONSTANT_NameAndType
    data << (qint16) 5; // "<init>" in the pool
    data << (qint16) 6; // "(Ljava/lang/Long;)V" in the pool

    //Various invoke() alternatives
    QString start("(Ljava/lang/String;");
    QString end(")Ljava/lang/Object;");
    for(int i=0;i<11;i++){
        writeUtf8ToPool(data,start + end);
        start += "Ljava/lang/Object;";
    }
    writeUtf8ToPool(data,"invoke");

    for(int i=0;i<11;i++){
        data << (qint8) 12; //CONSTANT_NameAndType
        data << (qint16) 21; //"invoke" in the pool
        data << (qint16) (10 + i); //signature string with i Objects
    }
    for(int i=0;i<11;i++){
        data << (qint8) 10; //CONSTANT_Methodref
        data << (qint16) 1; //Own class in the pool
        data << (qint16) (22 + i); //signature of method with i Objects
    }

    //Return types
    writeUtf8ToPool(data,"java/lang/Boolean");
    writeUtf8ToPool(data,"java/lang/Integer");
    writeUtf8ToPool(data,"java/lang/Long");
    writeUtf8ToPool(data,"java/lang/Double");
    writeUtf8ToPool(data,"java/lang/String");
    writeUtf8ToPool(data,"java/util/ArrayList");
    writeUtf8ToPool(data,"java/util/Map");
    writeUtf8ToPool(data,"java/net/URL");
    writeUtf8ToPool(data,"[B");
    writeUtf8ToPool(data,"[I");
    writeUtf8ToPool(data,"[D");
    writeUtf8ToPool(data,"[Ljava/lang/String;");
    for(int i=0;i<12;i++){
        data << (qint8) 7; //CONSTANT_Class
        data << (qint16) (44 + i); //index of corresponding string
    }

    //Methods
    for(int i = 0; i < methodCount; ++i) {
        QMetaMethod method = metaobject->method(i);
        QString signature(method.signature());
        // Method name
        writeUtf8ToPool(data,signature.left(signature.indexOf('(')));
        //String of name
        data << (qint8) 8; //CONSTANT_String
        data << (qint16) (68 + i * 3); //index of previous pool item
        //Parameter string
        QList<QByteArray> params = method.parameterTypes();
        QString sig("(");
        foreach(QByteArray param, params) {
            sig += toJavaType(param);
        }
        sig += ")";
        sig += toJavaType(QByteArray(method.typeName()));
        writeUtf8ToPool(data,sig);
    }
}

qint16 JVMClassWriter::toConstantpoolIndex(const QByteArray& type)
{
    int tp = QVariant::nameToType( type.constData() );
    switch(tp) {
        case QVariant::Int:
            return (qint16)57;
        case QVariant::UInt:
            return (qint16)57;
        case QVariant::Double:
            return (qint16)59;
        case QVariant::Bool:
            return (qint16)56;
        case QVariant::ByteArray:
            return (qint16)64;
        case QVariant::String:
            return (qint16)60;
        case QVariant::StringList:
            return (qint16)67;
        case QVariant::Map:
            return (qint16)62;
        case QVariant::List:
            return (qint16)61;
        case QVariant::LongLong:
            return (qint16)58;
        case QVariant::ULongLong:
            return (qint16)58;
        case QVariant::Url:
            return (qint16)63;
        case QVariant::Size:
            return (qint16)65;
        case QVariant::SizeF:
            return (qint16)66;
        case QVariant::Point:
            return (qint16)65;
        case QVariant::PointF:
            return (qint16)66;
        case QVariant::Rect:
            return (qint16)65;
        case QVariant::RectF:
            return (qint16)66;
        case QVariant::Invalid: // fall through
        case QVariant::UserType: // fall through
        default:
            //TODO: think about this case, when can we do this?
            return (qint16)1;
    }
}

QString JVMClassWriter::toJavaType(const QByteArray& type)
{
    int tp = QVariant::nameToType( type.constData() );
    switch(tp) {
        case QVariant::Int:
            return "Ljava/lang/Integer;";
        case QVariant::UInt:
            return "Ljava/lang/Integer;";
        case QVariant::Double:
            return "Ljava/lang/Double;";
        case QVariant::Bool:
            return "Ljava/lang/Boolean;";
        case QVariant::ByteArray:
            return "[B";
        case QVariant::String:
            return "Ljava/lang/String;";
        case QVariant::StringList:
            return "[Ljava/lang/String;";
        case QVariant::Map:
            return "Ljava/util/Map;";
        case QVariant::List:
            return "Ljava/util/ArrayList;";
        case QVariant::LongLong:
            return "Ljava/lang/Long;";
        case QVariant::ULongLong:
            return "Ljava/lang/Long;";
        case QVariant::Url:
            return "Ljava/net/URL;";
        case QVariant::Size:
            return "[I";
        case QVariant::SizeF:
            return "[D";
        case QVariant::Point:
            return "[I";
        case QVariant::PointF:
            return "[D";
        case QVariant::Rect:
            return "[I";
        case QVariant::RectF:
            return "[D";
        case QVariant::Invalid: // possible fall through?
            if(type.isEmpty())
                return "V";
        case QVariant::UserType: // fall through
        default:
            //TODO: think about this case, when can we do this?
            return "Lorg/kde/kdebindings/java/krossjava/KrossQExtension;";
    }
}

void JVMClassWriter::writeClassInfo(QDataStream& data)
{
    //TODO
    //Access flags. We use the value ACC_PUBLIC|ACC_SUPER. ACC_PUBLIC denotes this is a public
    //class. According to specification, all new compilers to the instruction set of the Java
    //virtual machine should set the ACC_SUPER flag.
    data << (qint16) (0x0001|0x0020);

    //The following class information denotes indices into the constant pool.
    //Class
    data << (qint16) 1;
    //Superclass
    data << (qint16) 3;

    //Interfaces - we don't (directly) implement any, so interface_count = 0.
    data << (qint16) 0;
}

void JVMClassWriter::writeFields(QDataStream& data)
{
    //The classes are only wrappers for methods and don't contain member fields,
    //thus fields_count = 0
    data << (qint16) 0;
}

void JVMClassWriter::writeMethods(QDataStream& data)
{
    QObject* object = m_extension->object();
    Q_ASSERT(object);
    const QMetaObject* metaobject = object->metaObject();
    const int methodCount = metaobject->methodCount();
    // The methods_count is the number of wrapped methods + 1 for the constructor
    data << (qint16) (methodCount + 1);

    //CONSTRUCTOR
    //Access flags, ACC_PUBLIC
    data << (qint16) 0x0001;
    //Name, index in constant pool
    data << (qint16) 5;
    //Descriptor, index in constant pool that gives the signature
    data << (qint16) 6;
    //Attributes. We only use one attribute, Code.
    data << (qint16) 1;

    //Attribute name, "Code" in the constant pool.
    data << (qint16) 7;
    //Next part is copied from what a standard compiler outputs, excuse the lack of info.
    //Length of Code attribute (minus 6 first bytes)
    data << (qint32) 18;
    //Max stack depth
    data << (qint16) 2;
    //Max locals
    data << (qint16) 2;
    //Code length
    data << (qint32) 6; //feel free to count them right below here :)
    //Bytecode
    data << (qint8) 0x2a; //aload_0, push 'this' on operand stack
    data << (qint8) 0x2b; //aload_1, push first argument on operand stack
    data << (qint8) 0xb7; //invokespecial ...
    data << (qint8) 0x00;
    data << (qint8) 0x08; //... with as argument the superconstructor in constant pool
    data << (qint8) 0xb1; //return
    //Exceptions, we don't use them here so count = 0
    data << (qint16) 0;
    //Attributes, we don't use those either
    data << (qint16) 0;

    //WRAPPER METHODS
    for(int i = 0; i < methodCount; ++i) {
        QMetaMethod method = metaobject->method(i);
        int numargs = method.parameterTypes().size();
        bool voidreturn = QString(method.typeName()).isEmpty();
        //Access flags, ACC_PUBLIC
        data << (qint16) 0x0001;
        //Name
        data << (qint16) (68 + i * 3);
        //Descriptor
        data << (qint16) (68 + i * 3 + 2);
        //Attributes. We only use one attribute, Code.
        data << (qint16) 1;

        //Attribute name, "Code" in the constant pool.
        data << (qint16) 7;
        //Length of Code attribute (minus 6 first bytes)
        if(voidreturn)
            data << (qint32) (12 + 9 + 2 * numargs);
        else
            data << (qint32) (12 + 11 + 2 * numargs);
        //Max stack depth, each argument + string + 'this' reference
        data << (qint16) (numargs + 2);
        //Max locals, arguments + string
        data << (qint16) (numargs + 1);
        //Code length
        if(voidreturn)
            data << (qint32) (9 + 2 * numargs);
        else
            data << (qint32) (11 + 2 * numargs); //2 extra bytes for the checkcast
        //Bytecode
        data << (qint8) 0x2a; //aload_0, push 'this' on operand stack
        data << (qint8) 0x13; //ldc_w, load string for constant pool on operand stack
        data << (qint16) (68 + 3*i + 1); //string that represents the method to be called
        for(int i = 1; i <= numargs; i++){
            //note, there are shorthand forms for i <= 4, but it's not really needed here,
            //using the same 2-byte version eases size calculations.
            data << (qint8) 0x19; //aload, push an argument on operand stack...
            data << (qint8) i; //...the ith variable in the frame, being the ith argument
        }
        data << (qint8) 0xb7; //invokespecial ...
        data << (qint16) (33 + numargs); //... with as argument the correct invoke method
        if(voidreturn){
            data << (qint8) 0x57; //pop, to get rid of the return argument
            data << (qint8) 0xb1; //return
        } else {
            data << (qint8) 0xc0; //checkcast
            data << (qint16) toConstantpoolIndex(method.typeName());
            data << (qint8) 0xb0; //areturn, return java object
        }
        //Exceptions, we don't use them here so count = 0
        data << (qint16) 0;
        //Attributes, we don't use those either
        data << (qint16) 0;
    }
}

void JVMClassWriter::writeAttributes(QDataStream& data)
{
    //We don't need any atributes like SourceFile or Deprecated, so we set attribute_count
    //to 0.
    data << (qint16) 0;
}

void JVMClassWriter::writeUtf8ToPool(QDataStream& data, const QString& str)
{
    QByteArray ba = str.toUtf8();
    data << (qint8) 1; //CONSTANT_Utf8
    data << (qint16) (ba.size()); //length in bytes, not null-terminated
    data.writeRawData(ba.data(), ba.size()); //raw bytes, not null-terminated
}
