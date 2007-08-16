/***************************************************************************
 * testwindow.h
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

#ifndef TEST_TESTWINDOW_H
#define TEST_TESTWINDOW_H

#include <kross/core/action.h>
//#include <kross/core/manager.h>

#include <QString>
#include <QList>
#include <kmainwindow.h>

class TestWindow : public KMainWindow
{
        Q_OBJECT
    public:
        TestWindow(QStringList scripts);
        virtual ~TestWindow();
    signals:
        void scriptHook();
    public slots:
        void emitScriptHook() { emit scriptHook(); }
    private slots:
        void execute();
    private:
        QList<Kross::Action*> m_actions;
};

#endif
