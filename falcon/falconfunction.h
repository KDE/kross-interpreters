/***************************************************************************
 * falconfunction.h
 * This file is part of the KDE project
 * copyright (C)2006 by Sebastian Sauer (mail@dipe.org)
 *
 * Parts of the code are from kjsembed4 SlotProxy
 * Copyright (C) 2005, 2006 KJSEmbed Authors.
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

#ifndef KROSS_FALCONFUNCTION_H
#define KROSS_FALCONFUNCTION_H

#include "falconconfig.h"
#include "falconkfvm.h"
#include <kross/core/krossconfig.h>
#include <kross/core/metafunction.h>

#include <QObject>
#include <QMetaObject>
#include <QMetaMethod>
#include <QByteArray>

namespace Kross {

    /**
     * The FalconFunction class implements a QObject to provide
     * an adaptor between Qt signals+slots and Falcon functions.
     */
    class FalconFunction : public MetaFunction
    {
        public:

            /**
            * Constructor.
            *
            * \param sender The QObject instance that sends the signal.
            * \param signal The signature of the signal the QObject emits.
            * \param callable The callable Falcon item that should
            *        be executed if the QObject emits the signal.
            * \param vm the Kross-Falcon vm where the execution will take place.
            */
            FalconFunction(QObject* sender, const QByteArray& signal, const Falcon::Item& callable, KFVM *vm );

            /**
            * Destructor.
            */
            virtual ~FalconFunction();

            /**
            * This method got called if a method this QObject instance
            * defines should be invoked.
            */
            int qt_metacall(QMetaObject::Call _c, int _id, void **_a);
            
        private:
            Falcon::GarbageLock *m_callLock;
            QVariant m_tmpResult;
            KFVM *m_vm;
    };

}

#endif
