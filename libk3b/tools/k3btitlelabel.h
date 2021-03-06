/*
 *
 * Copyright (C) 2003-2009 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2009 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */


#ifndef _K3B_TITLE_LABEL_H_
#define _K3B_TITLE_LABEL_H_

#include <QFrame>
#include <QResizeEvent>

#include "k3b_export.h"

class QResizeEvent;

namespace K3b {
    class LIBK3B_EXPORT TitleLabel : public QFrame
    {
        Q_OBJECT

    public:
        TitleLabel( QWidget* parent = 0 );
        ~TitleLabel();

        QSize sizeHint() const;
        QSize minimumSizeHint() const;

        bool event( QEvent* event );

    public Q_SLOTS:
        /**
         * default: 2
         */
        void setMargin( int );

        void setTitle( const QString& title, const QString& subTitle = QString() );
        void setSubTitle( const QString& subTitle );

        /**
         * The title label only supports alignments left, hcenter, and right
         *
         * Default alignment is left.
         */
        void setAlignment( Qt::Alignment alignment );

    protected:
        void resizeEvent( QResizeEvent* );
        void paintEvent( QPaintEvent* );

    private:
        void updatePositioning();

        class Private;
        Private* d;
    };
}

#endif
