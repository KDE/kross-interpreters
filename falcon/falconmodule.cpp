/***************************************************************************
 * falconmodule.cpp
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

#include "falconmodule.h"
#include <QtGlobal>
#include <falcon/module.h>
#include <falcon/item.h>
#include <falcon/vm.h>
#include <falcon/error.h>

namespace Kross {

    /****************************************************
    Falcon QPoint class
    *****************************************************/
    void  _qpoint_init( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        Falcon::Item *i_x = vm->param(0);
        Falcon::Item *i_y = vm->param(1);
        
        if ( ( i_x != 0 && ! i_x->isOrdinal() ) ||
             ( i_y != 0 && ! i_y->isOrdinal() )
            )
        {
            vm->raiseModError( new Falcon::ParamError( 
                Falcon::ErrorParam( Falcon::e_inv_params, __LINE__ ).origin( Falcon::e_orig_runtime ) ) );
            return;
        }

        self->setProperty( "x", i_x == 0 ? (Falcon::int64) 0: *i_x );
        self->setProperty( "y", i_x == 0 ? (Falcon::int64) 0: *i_x );
    }

    void  _qpoint_compare( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
        Falcon::Item *i_other = vm->param(0);
        
        // let the VM handle the situation
        if( ! i_other->isObject() )
        {
            vm->retnil();
            return;
        }
        
        // if the other item has an X and an Y (regarless of class), order it.
        Falcon::Item *i_other_x = i_other->asObject()->getProperty( "x" );
        if ( i_other_x == 0 )
        {
            // let the VM do normal comparation
            vm->retnil();
            return;
        }

        Falcon::Item *i_other_y = i_other->asObject()->getProperty( "y" );
        if ( i_other_y == 0 )
        {
            // let the VM do normal comparation (again)
            vm->retnil();
            return;
        }
        
        // get our data; we know we have X and Y
        Falcon::Item *i_x = self->getProperty( "x" );
        Falcon::Item *i_y = self->getProperty( "y" );
        
        Q_ASSERT( i_x != 0 && i_y != 0 );
        
        // Let's use VM comparation, that would eventually resolve to other calls
        int cmp = vm->compareItems( *i_x, *i_other_x );
        if ( cmp != 0 || vm->hadError() ) 
        {
            vm->retval( (Falcon::int64) cmp ); // if we had error, the retval has no meaning
            return;
        }

        // VM decided that X are the same; try the same for y.
        cmp = vm->compareItems( *i_y, *i_other_y );
        
        // again, if we had error, the VM will take care to unroll up to the correct frame. 
        vm->retval( (Falcon::int64) cmp );
    }
    
    
    void _qpoint_manatthanLength( Falcon::VMachine *vm )
    {
        Falcon::CoreObject *self = vm->self().asObject();
    
        // get our data; we know we have X and Y
        Falcon::Item *i_x = self->getProperty( "x" );
        Falcon::Item *i_y = self->getProperty( "y" );
        
        Q_ASSERT( i_x != 0 && i_y != 0 );
        
        // Numeric?
        if ( i_x->isNumeric() || i_y->isNumeric() )
        {
            double x = i_x->forceNumeric();
            double y = i_y->forceNumeric();
            if ( x < 0 ) x = -x;
            if ( y < 0 ) y = -y;
            vm->retval( x + y );
        }
        else {
            // integer?
            Falcon::int64 x = i_x->forceInteger();
            Falcon::int64 y = i_y->forceInteger();
            if ( x < 0 ) x = -x;
            if ( y < 0 ) y = -y;
            vm->retval( x + y );
        }
    }

    //===================================================
    // Module initialization
    //===================================================

    Falcon::Module *CreateKrossModule()
    {
        Falcon::Module *self = new Falcon::Module();
        
        //TODO: QObject and QAction
        self->name( "falcon_rtl" );
        self->engineVersion( FALCON_VERSION_NUM );
        
        // TODO: integrate with kross versioning
        self->version( 0, 1, 0 );
   
        // QPoint
        Falcon::Symbol *qpoint_class = self->addClass( "QPoint", _qpoint_init );
        self->addClassProperty( qpoint_class, "x" );
        self->addClassProperty( qpoint_class, "y" );
        self->addClassMethod( qpoint_class, "compare", _qpoint_compare );
        self->addClassMethod( qpoint_class, "manatthanLength", _qpoint_manatthanLength );
        
        /* TODO Add those:
        self->addClassMethod( qpoint_class, "add", _qpoint_add );
        self->addClassMethod( qpoint_class, "multiply", _qpoint_multiply );
        self->addClassMethod( qpoint_class, "subtract", _qpoint_subtract );
        self->addClassMethod( qpoint_class, "divide", _qpoint_divide );
        */
        
    
        return self;
    }
}
