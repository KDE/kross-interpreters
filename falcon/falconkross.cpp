/***************************************************************************
 * falconkross.cpp
 * This file is part of the KDE project
 * copyright (C)2004-2006 by Sebastian Sauer (mail@dipe.org)
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
 ************************************************************************/
 
#include "falconkross.h"
#include "falconkfvm.h"
#include <falcon/module.h>
#include <falcon/vm.h>

namespace Kross
{
    static void _falcon_Kross_action( Falcon::VMachine * )
    {
    }
    
    static void _falcon_Kross_objGen( Falcon::VMachine *vm )
    {
        KFVM *kvm = static_cast<KFVM *>(vm);
        Falcon::CoreObject *obj = kvm->QObjectToKrossObject( new QObject() );
        kvm->retval( obj );
    }
    
    void DeclareFalconKross( Falcon::Module *self )
    {
        Falcon::Symbol *kross_class = self->addClass( "%Kross" );
        kross_class->exported( false );
        
        self->addClassMethod( kross_class, "action", _falcon_Kross_action );
        self->addClassMethod( kross_class, "objGen", _falcon_Kross_objGen );
        
        // create a singletone instance of %XChat class.
        Falcon::Symbol *o_kross = new Falcon::Symbol( self, self->addString( "Kross" ) );
        o_kross->setInstance( kross_class );
        o_kross->exported( true );
        self->addGlobalSymbol( o_kross );
    }
}
