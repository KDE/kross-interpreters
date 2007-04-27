/***************************************************************************
 * jvmscript.h
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

#ifndef KROSS_JVMSCRIPT_H
#define KROSS_JVMSCRIPT_H

#include "jvmconfig.h"
#include <kross/core/script.h>

namespace Kross {

    class JVMScript : public Script
    {
        public:
            explicit JVMScript(Interpreter* interpreter, Action* action);
            virtual ~JVMScript();

            virtual void execute();

            virtual QStringList functionNames() {
                return QStringList();
            }
            virtual QVariant callFunction(const QString& name, const QVariantList& args = QVariantList()) {
                Q_UNUSED(name);
                Q_UNUSED(args);
                return QVariant();
            }

        private:
            class Private;
            Private * const d;
    };

}

#endif
