/***************************************************************************
 * falconmodule.h
 * This file is part of the KDE project
 * copyright (C)2004-2007 by jonnymind@falconpl.org
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

#ifndef KROSS_FALCONMODULE_H
#define KROSS_FALCONMODULE_H


namespace Falcon {
    /// Forward declaration for Module class;
    class Module;
}

namespace Kross {
    
    
    
    /**
    * Creates the Kross-Falcon integration module.
    * This factory function returns a Falcon module which is injected
    * in scripts. The module contains the glue between Falcon and Kross/KDE;
    * QVariant special types, Action reflections and so on.
    */
    
    Falcon::Module *CreateKrossModule();
}

#endif
