/*
 *
 * Copyright (C) 2007 Sebastian Trueg <trueg@k3b.org>
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

#ifndef _K3B_DEVICE_MODEL_H_
#define _K3B_DEVICE_MODEL_H_

#include <QAbstractItemModel>

namespace K3bDevice {
    class Device;
}

class K3bDeviceModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    K3bDeviceModel( QObject* parent = 0 );
    ~K3bDeviceModel();

    QList<K3bDevice::Device*> devices() const;

    K3bDevice::Device* deviceForIndex( const QModelIndex& index ) const;
    QModelIndex indexForDevice( K3bDevice::Device* dev ) const;

    int columnCount( const QModelIndex& parent = QModelIndex() ) const;
    QVariant data( const QModelIndex& index, int role = Qt::DisplayRole ) const;
    QModelIndex index( int row, int column, const QModelIndex& parent = QModelIndex() ) const;
    QModelIndex parent( const QModelIndex& index ) const;
    int rowCount( const QModelIndex& parent = QModelIndex() ) const;

public Q_SLOTS:
    void setDevices( const QList<K3bDevice::Device*>& devices );

private:
    class Private;
    Private* const d;
};

#endif