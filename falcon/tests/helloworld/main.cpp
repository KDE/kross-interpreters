/***************************************************************************
 * main.cpp
 * Test for Kross - Falcon actions.
 *
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

// First some Qt and KDE includes
#include <QString>
#include <KApplication>
#include <KAboutData>
#include <KMessageBox>
#include <KCmdLineArgs>
#include <KLocalizedString>
// Also include the MainWindow class
#include "mainwindow.h"
 
int main (int argc, char *argv[])
{
    // Used to store information about a program.
    KAboutData aboutData("krosshello",
        0,
        ki18n("Kross+Falcon Hello World"),
        "1.0",
        ki18n("Hello World application for Kross + Falcon"),
        KAboutData::License_GPL,
        ki18n("(c) 2008"),
        ki18n("This test shows a minimal usage of Falcon actions under the Kross hood."),
        "http://kross.dipe.org",
        "submit@bugs.kde.org");
 
    // Access to the command-line arguments.
    KCmdLineArgs::init( argc, argv, &aboutData );
    // Initialize the application.
    KApplication app;
 
    // Create and show the main window.
    MainWindow* window = new MainWindow();
    window->show();
 
    // Finally execute the application.
    return app.exec();
}
