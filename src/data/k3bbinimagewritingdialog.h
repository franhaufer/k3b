/* 
 *
 * $Id$
 * Copyright (C) 2003 Klaus-Dieter Krannich <kd@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2003 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */


#ifndef K3BBINIMAGEWRITINGDIALOG_H
#define K3BBINIMAGEWRITINGDIALOG_H

#include <k3binteractiondialog.h>
#include <kurl.h>
#include <klineedit.h>
#include <qcheckbox.h>
#include <qtoolbutton.h>
#include <qspinbox.h>
#include "k3bwriterselectionwidget.h"
#include "k3bbinimagewritingjob.h"


/**
  *@author Klaus-Dieter Krannich
  */
class K3bBinImageWritingDialog : public K3bInteractionDialog
{
Q_OBJECT

 public: 
  K3bBinImageWritingDialog( QWidget* = 0, const char* = 0, bool = true );
  ~K3bBinImageWritingDialog();

  void setTocFile( const KURL& url );

 protected slots:
  void slotStartClicked();

  void slotFindTocFile();
  void slotWriterChanged();

  void slotLoadUserDefaults();
  void slotSaveUserDefaults();
  void slotLoadK3bDefaults();

 private:
  void setupGui();

  K3bBinImageWritingJob* m_job;

  K3bWriterSelectionWidget* m_writerSelectionWidget;
  QCheckBox* m_checkSimulate;
  QCheckBox* m_checkMulti;
  QCheckBox* m_checkForce;
  QSpinBox*  m_spinCopies;
  KLineEdit* m_editTocPath;
  QToolButton* m_buttonFindTocFile;
  QString m_tocPath;
};

#endif
