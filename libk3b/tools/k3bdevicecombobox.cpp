/*
 *
 * $Id$
 * Copyright (C) 2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2007 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include "k3bdevicecombobox.h"
#include "k3bdevicelistmodel.h"

#include <k3bdevice.h>
#include <k3bdevicemanager.h>
#include <k3bcore.h>

#include <klocale.h>

#include <qmap.h>





class K3bDeviceComboBox::Private
{
public:
    K3bDeviceListModel* model;
};


K3bDeviceComboBox::K3bDeviceComboBox( QWidget* parent )
    : KComboBox( parent )
{
    d = new Private();
    d->model = new K3bDeviceListModel( this );
    setModel( d->model );

    connect( this, SIGNAL(activated(int)),
             this, SLOT(slotActivated(int)) );
}


K3bDeviceComboBox::~K3bDeviceComboBox()
{
    delete d;
}


K3bDevice::Device* K3bDeviceComboBox::selectedDevice() const
{
#ifdef __GNUC__
#warning FIXME: implement K3bDeviceComboBox::selectedDevice
#endif
//     if ( count() > 0 )
//         return d->devices[currentIndex()];
//     else
//         return 0;
}


void K3bDeviceComboBox::addDevice( K3bDevice::Device* dev )
{
    d->model->addDevice( dev );
}


void K3bDeviceComboBox::removeDevice( K3bDevice::Device* dev )
{
    d->model->removeDevice( dev );
}


void K3bDeviceComboBox::addDevices( const QList<K3bDevice::Device*>& list )
{
    d->model->addDevices( list );
}


void K3bDeviceComboBox::refreshDevices( const QList<K3bDevice::Device*>& list )
{
    d->model->setDevices( list );}


void K3bDeviceComboBox::setSelectedDevice( K3bDevice::Device* dev )
{
#ifdef __GNUC__
#warning FIXME: implement K3bDeviceComboBox::setSelectedDevice
#endif
//     if( dev ) {
//         if( d->deviceIndexMap.contains(dev->devicename()) ) {
//             setCurrentItem( d->deviceIndexMap[dev->devicename()] );
//             emit selectionChanged( dev );
//         }
//     }
}


void K3bDeviceComboBox::slotActivated( int i )
{
    emit selectionChanged( d->model->deviceForIndex( d->model->index( i, 0 ) ) );
}

#include "k3bdevicecombobox.moc"
