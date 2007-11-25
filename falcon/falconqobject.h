/***************************************************************************
 * falconqobject.h
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

#ifndef KROSS_FALCONQOBJECT_H
#define KROSS_FALCONQOBJECT_H

#include <falcon/userdata.h>
#include <QObject>

namespace Falcon {
    /// Forward declaration for Falcon VM class;
    class VMachine;
}

namespace Kross {
    
    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * This implements object initialization.
    */
    void _falcon_qobject_init( Falcon::VMachine *vm );
    
    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::blockSignal() accessor.
    */
    void _falcon_qobject_blockSignals( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::children() accessor.
    * The calling script will receive an array of Falcon objects, each
    * being an implementation of a QObject.
    */
    // TODO: implement children control
    //void _falcon_qobject_children( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::connect() method.
    */
    void _falcon_qobject_connect( Falcon::VMachine *vm );
    
    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::connect() method.
    */
    void _falcon_qobject_disconnect( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::dumpObjectInfo() method.
    */
    void _falcon_qobject_dumpObjectInfo( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::dumpObjectTree() method.
    */
    void _falcon_qobject_dumpObjectTree( Falcon::VMachine *vm );
    
    //TODO: QEvent
    //virtual bool event ( QEvent * e )
    //virtual bool eventFilter ( QObject * watched, QEvent * event )

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::findChild() method.
    */
    void _falcon_qobject_findChild( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::findChildren() method.
    * Can be provided with nothing, a string or a regular expression.
    * Returns a list of objects
    */
    void _falcon_qobject_findChildren ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::inherits() method.
    */
    void _falcon_qobject_inherits ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::installEventFilter() method.
    */
    void _falcon_qobject_installEventFilter ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::isWidgetType() method.
    */
    void _falcon_qobject_isWidgetType ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::killTimer() method.
    */
    void _falcon_qobject_killTimer ( Falcon::VMachine *vm );

    //TODO - MetaObject
    //virtual const QMetaObject * metaObject () const

    //TODO - QThread
    //void moveToThread ( QThread * targetThread )

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::killTimer() method.
    */
    void _falcon_qobject_killTimer ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::objectName() method.
    */
    void _falcon_qobject_objectName ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::parent() method.
    */
    // TODO: implement children control
    //void _falcon_qobject_parent ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::propertyproperty() method.
    */
    void _falcon_qobject_property ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::removeEventFilter() method.
    */
    // TODO: event
    //void _falcon_qobject_removeEventFilter ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::setObjectName() method.
    */
    void _falcon_qobject_setObjectName ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::setParentsetParent() method.
    */
    // TODO: children control
    //void _falcon_qobject_setParent ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::setProperty() method.
    */
    void _falcon_qobject_setProperty( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::signalsBlocked() method.
    */
    void _falcon_qobject_signalsBlocked ( Falcon::VMachine *vm );

    /**
    * Interfaces the C++ QObject method as a falcon object call.
    * Reimplmentation of the QObject::startTimer() method.
    */
    void _falcon_qobject_startTimer ( Falcon::VMachine *vm );

    // TODO: Qthread
    //QThread * thread () const
    
    class FQOCarrier;
    
    /** 
    * QObject destruction sensitive class. 
    * This class has a slot to receive the destruction of the carried object.
    * When this happens, the carrier is invalidated and access to carried
    * data will cause VM exception in Falcon.
    */
    class FQOCarrier_Sensitive: public QObject
    {
        Q_OBJECT
        
        FQOCarrier *m_owner;
    public:
        FQOCarrier_Sensitive( FQOCarrier *owner  );

    public slots:
        void onCarriedGone();
    };
    
    /**
    * Carrier class for QObjects.
    * This class is needed to provide storage and reflectivity for
    * QObjects stored in Falcon objects for script manipulation.
    *
    * This class provides safe access to the stored QObject. In case
    * it gets destroyed while in the carrier, the object is zeroed
    * and access will generate an exception in the calling VM.
    *
    * This class hasn't a d-> pointer as it IS the d pointer for the
    * CoreObject class...
    */
    class FQOCarrier: public Falcon::UserData
    {
        QObject *m_carried;
    
        
        FQOCarrier_Sensitive m_sensible;
        
        // also, we need friendship of the internal class to manipulate the carreid object.
        friend class FQOCarrier_Sensitive;
        
    public:
        FQOCarrier( QObject *data );
        
        /**
        * Unguarded version of the accessor. Use with care.
        */
        
        QObject *qobject() const;
        
        /**
        * Guarded version of the accessor.
        * If the accessor returns zero, the VM has already been notified with an exception.
        */
        QObject *qobject( Falcon::VMachine *vm ) const;
        
        
        /**
        * Setting a new accessor.
        */
        void qobject( QObject *qo );
    };
}



#endif
