/***************************************************************************
                          k3boptiondialog.cpp  -  description
                             -------------------
    begin                : Tue Apr 17 2001
    copyright            : (C) 2001 by Sebastian Trueg
    email                : trueg@informatik.uni-freiburg.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "k3boptiondialog.h"
#include "../k3b.h"
#include "k3bcddboptiontab.h"
#include "k3bcddblocaldbtab.h"
#include "k3bdeviceoptiontab.h"
#include "k3bburningoptiontab.h"
#include "k3bpatternoptiontab.h"
#include "k3bexternalbinoptiontab.h"
#include "k3bmiscoptiontab.h"

#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qtabwidget.h>

#include <klocale.h>
#include <kiconloader.h>
#include <kconfig.h>
#include <kedittoolbar.h>


// TODO: handle the default-settings

K3bOptionDialog::K3bOptionDialog(QWidget *parent, const char *name, bool modal )
  : KDialogBase( IconList, i18n("Options"), Apply|Ok|Cancel, Ok, parent,name, modal, true)
{
  setupBurningPage();
  setupDevicePage();	
  setupProgramsPage();
  setupCddbPage();
  setupPatternPage();
  setupMiscPage();
  setupToolBarTab();

  m_externalBinOptionTab->readSettings();
  m_cddbOptionTab->readSettings();
  //  m_cddbLocalTab->readSettings();
  m_deviceOptionTab->readDevices();
  m_burningOptionTab->readSettings();
  m_patternOptionTab->readSettings();
  m_miscOptionTab->readSettings();


  // if we don't do this the dialog start really huge
  // because of the label in the device-tab
  resize( 700, 500 );
}


K3bOptionDialog::~K3bOptionDialog()
{
}


void K3bOptionDialog::slotOk()
{
  if( saveSettings() )
    accept();
}

void K3bOptionDialog::slotApply()
{
  saveSettings();
}


bool K3bOptionDialog::saveSettings()
{
  // save all the shit!
  m_cddbOptionTab->apply();
  //  m_cddbLocalTab->apply();
  m_deviceOptionTab->saveDevices();
  m_burningOptionTab->saveSettings();
  m_patternOptionTab->apply();
  m_externalBinOptionTab->saveSettings();
  m_editToolbarWidget->save();

  if( !m_miscOptionTab->saveSettings() )
    return false;

  kapp->config()->sync();

  return true;
}


void K3bOptionDialog::slotDefault()
{
  switch( activePageIndex() )
    {
    case 0: // device page

      break;
    case 1: // programs page
      break;
    default: 
      break;
    }
}


void K3bOptionDialog::setupBurningPage()
{
  QFrame* frame = addPage( i18n("Writing"), i18n("Writing Settings"),
			   KGlobal::instance()->iconLoader()->loadIcon( "cdwriter_unmount", KIcon::NoGroup, KIcon::SizeMedium ) );
		
  QGridLayout* _frameLayout = new QGridLayout( frame );
  _frameLayout->setSpacing( 0 );
  _frameLayout->setMargin( 0 );

  m_burningOptionTab = new K3bBurningOptionTab( frame );
  _frameLayout->addWidget( m_burningOptionTab, 0, 0 );
}


void K3bOptionDialog::setupProgramsPage()
{
  QFrame* frame = addPage( i18n("Programs"), i18n("Setup External Programs"),
			   KGlobal::instance()->iconLoader()->loadIcon( "exec", KIcon::NoGroup, KIcon::SizeMedium ) );

  QGridLayout* _frameLayout = new QGridLayout( frame );
  _frameLayout->setSpacing( 0 );
  _frameLayout->setMargin( 0 );

  m_externalBinOptionTab = new K3bExternalBinOptionTab( k3bMain()->externalBinManager(), frame );
  _frameLayout->addWidget( m_externalBinOptionTab, 0, 0 );
}


void K3bOptionDialog::setupCddbPage()
{
  QFrame* frame = addPage( i18n("CDDB"), i18n("Setup the Cddb Server"),
			   KGlobal::instance()->iconLoader()->loadIcon( "connect_established", KIcon::NoGroup, KIcon::SizeMedium ) );

  QGridLayout* mainGrid = new QGridLayout( frame );
  mainGrid->setSpacing(0);
  mainGrid->setMargin(0);
  //  QTabWidget *_tab = new QTabWidget( frame );
  m_cddbOptionTab = new K3bCddbOptionTab( frame, "cddbremotepage");
  //  m_cddbLocalTab = new K3bCddbLocalDBTab( frame, "cddblocalpage");
//   _tab->addTab( m_cddbOptionTab, i18n("Remote") );
//   _tab->addTab( m_cddbLocalTab, i18n("Local") );
  //mainGrid->addWidget( m_cddbOptionTab, 0, 0 );
  mainGrid->addWidget( m_cddbOptionTab, 0, 0 );
}


void K3bOptionDialog::setupDevicePage()
{
  QFrame* frame = addPage( i18n("Devices"), i18n("Setup CD Devices"),
			   KGlobal::instance()->iconLoader()->loadIcon( "blockdevice", KIcon::NoGroup, KIcon::SizeMedium ) );

  QHBoxLayout* box = new QHBoxLayout( frame );
  box->setSpacing(0);
  box->setMargin(0);
  m_deviceOptionTab = new K3bDeviceOptionTab( frame, "deviceOptionTab" );
  box->addWidget( m_deviceOptionTab );
}


void K3bOptionDialog::setupPatternPage()
{
  QFrame* frame = addPage( i18n("CD Ripping"), i18n("Setup Audio CD Ripping Patterns"),
			   KGlobal::instance()->iconLoader()->loadIcon( "misc", KIcon::NoGroup, KIcon::SizeMedium ) );

  QVBoxLayout* box = new QVBoxLayout( frame );
  box->setSpacing( 0 );
  box->setMargin( 0 );

  m_patternOptionTab = new K3bPatternOptionTab( frame, "patternOptionTab" );
  box->addWidget( m_patternOptionTab );
}


void K3bOptionDialog::setupMiscPage()
{
  QFrame* frame = addPage( i18n("Misc"), i18n("Miscellaneous Settings"),
			   KGlobal::instance()->iconLoader()->loadIcon( "misc", KIcon::NoGroup, KIcon::SizeMedium ) );

  QVBoxLayout* box = new QVBoxLayout( frame );
  box->setSpacing( 0 );
  box->setMargin( 0 );

  m_miscOptionTab = new K3bMiscOptionTab( frame );
  box->addWidget( m_miscOptionTab );
}


void K3bOptionDialog::setupToolBarTab()
{
  QFrame* frame = addPage( i18n("Toolbars"), i18n("Configure Toolbars"),
			   KGlobal::instance()->iconLoader()->loadIcon( "configure_toolbars", KIcon::NoGroup, KIcon::SizeMedium ) );

  QHBoxLayout* box = new QHBoxLayout( frame );
  box->setSpacing(0);
  box->setMargin(0);
  m_editToolbarWidget = new KEditToolbarWidget( k3bMain()->actionCollection(), QString::null, true, frame );
  box->addWidget( m_editToolbarWidget );
}
#include "k3boptiondialog.moc"
