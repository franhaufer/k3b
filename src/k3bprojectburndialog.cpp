/***************************************************************************
                          k3bprojectburndialog.cpp  -  description
                             -------------------
    begin                : Thu May 17 2001
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

#include "k3bprojectburndialog.h"
#include "k3b.h"
#include "k3bdoc.h"
#include "k3bburnprogressdialog.h"
#include "k3bjob.h"

#include <qstring.h>
#include <qpushbutton.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <qlayout.h>
#include <qvbox.h>

#include <klocale.h>
#include <kconfig.h>
#include <kmessagebox.h>
#include <kguiitem.h>
#include <kstdguiitem.h>



K3bProjectBurnDialog::K3bProjectBurnDialog(K3bDoc* doc, QWidget *parent, const char *name, bool modal )
  : KDialogBase( parent, name, modal, i18n("Write CD"), Ok|User1|User2, User1, false,
                 KGuiItem( i18n("Save"), "filesave", i18n("Save Settings and close"),
                           i18n("Saves the settings to the project and closes the burn dialog.") ),
                 KStdGuiItem::cancel() )
{
  m_doc = doc;

  setButtonBoxOrientation( Vertical );
  setButtonText( Ok, i18n("Write") );
  m_job = 0;

  QWidget* box = new QWidget( this );
  setMainWidget( box );

  m_k3bMainWidget = new QVBox( box );

  QGridLayout* grid = new QGridLayout( box );
  grid->setSpacing( spacingHint() );
  grid->setMargin( 0 );

  m_buttonLoadDefaults = new QPushButton( i18n("Defaults"), box );
  m_buttonLoadUserDefaults = new QPushButton( i18n("User Defaults"), box );
  m_buttonSaveUserDefaults = new QPushButton( i18n("Save User Defaults"), box );

  grid->addMultiCellWidget( m_k3bMainWidget, 0, 0, 0, 3 );
  grid->addWidget( m_buttonLoadDefaults, 1, 0 );
  grid->addWidget( m_buttonLoadUserDefaults, 1, 2 );
  grid->addWidget( m_buttonSaveUserDefaults, 1, 3 );
  grid->setRowStretch( 0, 1 );
  grid->setColStretch( 1, 1 );

  connect( m_buttonLoadDefaults, SIGNAL(clicked()), this, SLOT(loadDefaults()) );
  connect( m_buttonLoadUserDefaults, SIGNAL(clicked()), this, SLOT(loadUserDefaults()) );
  connect( m_buttonSaveUserDefaults, SIGNAL(clicked()), this, SLOT(saveUserDefaults()) );


  // ToolTips
  // -------------------------------------------------------------------------
  QToolTip::add( m_buttonLoadDefaults, i18n("Load K3b default settings") );
  QToolTip::add( m_buttonLoadUserDefaults, i18n("Load user default settings") );
  QToolTip::add( m_buttonSaveUserDefaults, i18n("Save user default settings for new projects") );

  // What's This info
  // -------------------------------------------------------------------------
  QWhatsThis::add( m_buttonLoadDefaults, i18n("<p>This sets all options back to K3b defaults.") );
  QWhatsThis::add( m_buttonLoadUserDefaults, i18n("<p>This loads the settings saved with the <em>Save User Defaults</em> "
						  "button.") );
  QWhatsThis::add( m_buttonSaveUserDefaults, i18n("<p>Saves the current settings as the default for all new projects."
						  "<p>These settings can also be loaded with the <em>User Defaults</em> "
						  "button."
						  "<p><b>The K3b defaults are not overwritten by this!</b>") );
}


K3bProjectBurnDialog::~K3bProjectBurnDialog(){
}


int K3bProjectBurnDialog::exec( bool burn )
{
  if( burn && m_job == 0 ) {
    showButton(Ok, true );
    actionButton(Ok)->setDefault(true);
    actionButton(User1)->setDefault(false);
    actionButton(User1)->clearFocus();
  }
  else {
    showButton(Ok, false );
    actionButton(User1)->setDefault(false);
    actionButton(Ok)->setDefault(true);
    actionButton(Ok)->clearFocus();
  }

  readSettings();

  return KDialogBase::exec();
}


void K3bProjectBurnDialog::slotUser1()
{
  saveSettings();
  m_doc->updateAllViews();
  done( Saved );
}


void K3bProjectBurnDialog::slotUser2()
{
  done( Canceled );
}


void K3bProjectBurnDialog::slotCancel()
{
  done( Canceled );
}

void K3bProjectBurnDialog::slotOk()
{
  if( m_job ) {
    KMessageBox::sorry( k3bMain(), i18n("K3b is already working on this project!"), i18n("Error") );
    return;
  }

  saveSettings();

  m_job = m_doc->newBurnJob();
  //  m_job->setWritingApp( m_writerSelectionWidget->writingApp() );

  K3bBurnProgressDialog d( k3bMain() );
  d.setJob( m_job );

  hide();

  m_job->start();
  d.exec();


  delete m_job;

  done( Burn );
}


#include "k3bprojectburndialog.moc"
