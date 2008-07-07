/***************************************************************************
* falconkopaque.cpp
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

#include "falconkopaque.h"
#include "falconobjman.h"
#include <QMetaType>
#include <kross/core/metatype.h>

namespace Kross {

static void KrossOpaque_typeId( Falcon::VMachine *vm )
{
    Falcon::CoreObject *self = vm->self().asObject();
    MetaType *md = static_cast<MetaType *>( self->getUserData() );
    vm->retval( (Falcon::int64) md->typeId() );
}

static void KrossOpaque_typeName( Falcon::VMachine *vm )
{
    Falcon::CoreObject *self = vm->self().asObject();
    MetaType *md = static_cast<MetaType *>( self->getUserData() );
    // we have a valid conversion between String & and items,
    // and between char * and String &
    vm->retval( QMetaType::typeName( md->typeId() ) );
}

void DeclareKrossOpaque( Falcon::Module *module )
{
    Falcon::Symbol *kosym = module->addClass( "KrossOpaque" );
    kosym->setWKS( true );
    kosym->getClassDef()->setObjectManager( &ko_ptr_manager );
    module->addClassMethod( kosym, "typeId", KrossOpaque_typeId );
    module->addClassMethod( kosym, "typeName", KrossOpaque_typeName );
}

}
