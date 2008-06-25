/***************************************************************************
 * mainwindow.h
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


#ifndef MAINWINDOW_H
#define MAINWINDOW_H
 
#include <QComboBox>
#include <QLabel>
 
// The main window to display our combobox and the label.
class MainWindow : public QWidget
{
    Q_OBJECT
  public:
    // The constructor.
    MainWindow(QWidget *parent=0);
  private Q_SLOTS:
    // This slot is called when the item in the combobox is changed.
    void scriptActivated(const QString &);
  private:
    QLabel* lblHello;
    QComboBox* cmbScripts;
};
 
#endif
