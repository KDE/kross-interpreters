/***************************************************************************
 * falconkerror.cpp
 * This file is part of the KDE project
 * copyright (C)2007-2008 by Giancarlo Niccolai (jonnymind@falconpl.org)
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

#include "falconkerror.h"
#include <falcon/cobject.h>
#include <falcon/vm.h>

namespace Kross {

    static void _falcon_KrossError_init ( ::Falcon::VMachine *vm )
    {
        Falcon::CoreObject *einst = vm->self().asObject();
        if( einst->getUserData() == 0 )
            einst->setUserData( new Falcon::ErrorCarrier( new FalconKrossError ) );
        
        ::Falcon::core::Error_init( vm );
    }

    void DeclareFalconKrossError( ::Falcon::Module *self )
    {
        Falcon::Symbol *error_class = self->addExternalRef( "Error" ); // it's external
        Falcon::Symbol *kerr_cls = self->addClass( "KrossError", _falcon_KrossError_init );
        kerr_cls->getClassDef()->addInheritance(  new Falcon::InheritDef( error_class ) );
    }
}
