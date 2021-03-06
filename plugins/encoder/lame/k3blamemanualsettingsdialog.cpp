/*
 *
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

#include "k3blamemanualsettingsdialog.h"

#include <KLocale>

K3bLameManualSettingsDialog::K3bLameManualSettingsDialog( QWidget* parent )
    : KDialog( parent )
{
    setCaption( i18n("(Lame) Manual Quality Settings") );
    setButtons( Ok|Cancel );

    setupUi( mainWidget() );
}


K3bLameManualSettingsDialog::~K3bLameManualSettingsDialog()
{
}
