/***************************************************************************
 * falconmcinfo.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2006 by Giancarlo Niccolai (jonnymind@falconpl.org)
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

#include "falconmcinfo.h"
#include <falcon/vm.h>

namespace Kross
{
    static void _falcon_qmetaclassinfo_init( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        Falcon::Item *i_name = vm->param(0);
        Falcon::Item *i_value = vm->param(1);
        
        if ( ( i_name != 0 && ! i_name->isString() && i_name->isNil() ) ||
             ( i_value != 0 && ! i_value->isString() )
            )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).
                origin( Falcon::e_orig_runtime ).
                extra( "S,S" ) ) );
            return;
        }

        self->setProperty( "name", *i_name );
        self->setProperty( "value", *i_value );
    }

    void DeclareFalconMetaclassInfo( Falcon::Module *self )
    {
        Falcon::Symbol *qmci_class = self->addClass( "QMetaClassInfo", _falcon_qmetaclassinfo_init );
        self->addClassProperty( qmci_class, "name" );
        self->addClassProperty( qmci_class, "value" );
    }
    
}
