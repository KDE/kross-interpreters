/***************************************************************************
 * testwindow.cpp
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

#include "testwindow.h"

#include "testobject.h"

#include <QLabel>
#include <QMenu>
#include <QGroupBox>
#include <QComboBox>
#include <QDir>
#include <QVBoxLayout>

#include <kdebug.h>
#include <kstandarddirs.h>

TestWindow::TestWindow(QStringList scripts) : KMainWindow()
{
    //Kross::Manager::self().addModule( Kross::Module::Ptr(new TestPluginModule("krosstestpluginmodule")) );

    //TODO: this is currently required for our classloader to find the class. Perhaps we should ease this a bit?
    setObjectName("TestWindow");

    QWidget* mainbox = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(mainbox);
    Q_UNUSED(layout);
    setCentralWidget(mainbox);
    setMinimumSize(600,420);

    TestObject* testobject = new TestObject(this, "TestObject");
    foreach(QString s, scripts) {
        Kross::Action* action = new Kross::Action(this, s);
        action->setFile(s);
        //FIXME
        action->addObject(this, "TestWindow");
        action->addObject(testobject, "TestObject");
        m_actions.append(action);
    }

    execute();
}

TestWindow::~TestWindow()
{
}

void TestWindow::execute()
{
    foreach(Kross::Action* action, m_actions) {
        kDebug() << "TestWindow::execute() File=" << action->file() << endl;
        action->trigger();
        if( action->hadError() ) {
            kWarning() << "TestWindow::execute() " << QString("%2\n%1").arg(action->errorTrace()).arg(action->errorMessage()).toLatin1().data() << endl;
        }
    }
}

#include "testwindow.moc"
