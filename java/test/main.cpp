/***************************************************************************
 * main.cpp
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

// for std namespace
#include <iostream>

// Qt
#include <QtCore/QFile>

// KDE
#include <kapplication.h>
#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <kurl.h>

// the main window
#include "testwindow.h"

#define ERROR_OK 0
#define ERROR_HELP -1
#define ERROR_NOSUCHFILE -2
#define ERROR_OPENFAILED -3
#define ERROR_NOINTERPRETER -4
#define ERROR_EXCEPTION -6

KApplication* app = 0;

int main(int argc, char **argv)
{
    int result = ERROR_OK;

    KAboutData about("kross",
                     0,
                     ki18n("Kross"),
                     "0.1",
                     ki18n("KDE application to run Kross scripts."),
                     KAboutData::License_LGPL,
                     ki18n("(C) 2006 Sebastian Sauer"),
                     ki18n("Run Kross scripts."),
                     "http://kross.dipe.org",
                     "kross@dipe.org");
    about.addAuthor(ki18n("Sebastian Sauer"), ki18n("Author"), "mail@dipe.org");

    // Initialize command line args
    KCmdLineArgs::init(argc, argv, &about);
    // Tell which options are supported and parse them.
    KCmdLineOptions options;
    options.add("+file", ki18n("Scriptfile"));

    KCmdLineArgs::addCmdLineOptions(options);
    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();

    // If no options are defined.
    if(args->count() < 1) {
        std::cout << "Syntax: " << KCmdLineArgs::appName().toLocal8Bit().constData() << " scriptfile1 [scriptfile2] [scriptfile3] ..." << std::endl;
        return ERROR_HELP;
    }

    // Each argument is a scriptfile to open
    QStringList scripts;
    for(int i = 0; i < args->count(); i++)
        scripts.append( args->arg(i) );

    app = new KApplication(true);
    TestWindow *mainWin = new TestWindow(scripts);
    mainWin->show();
    args->clear();
    result = app->exec();
    delete app;
    return result;
}
