/*
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

#include "k3baudiometainforenamerplugin.h"

#include <config-k3b.h>

// the k3b stuff we need
#include <k3bcore.h>
#include <k3bdatadoc.h>
#include <k3bdiritem.h>
#include <k3bfileitem.h>
#include <k3blistview.h>

#include <kdebug.h>
#include <kcomponentdata.h>
#include <kmessagebox.h>
#include <klocale.h>
#include <kfilemetainfo.h>
#include <kcombobox.h>
#include <kconfig.h>
#include <kmimetype.h>
#include <kdialog.h>
#include <KIcon>

#include <qstring.h>
#include <qfile.h>
#include <qcheckbox.h>
#include <qradiobutton.h>
#include <qpushbutton.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qtooltip.h>

#include <qpair.h>
#include <qlayout.h>
#include <QVBoxLayout>
#include <QtCore/QHash>
#include <QtCore/QLatin1String>
#include <QtGui/QTreeWidget>

#include <taglib/tag.h>
#include <taglib/fileref.h>
#include <taglib/audioproperties.h>
#include <taglib/mpegfile.h>
#include <taglib/vorbisfile.h>
#include <taglib/oggflacfile.h>


K3B_EXPORT_PLUGIN( k3baudiometainforenamerplugin, K3bAudioMetainfoRenamerPlugin )

namespace {
    class KMimeTypeResolver : public TagLib::FileRef::FileTypeResolver
    {
    public:
        // to make gcc shut up
        virtual ~KMimeTypeResolver() {}

        TagLib::File* createFile( TagLib::FileName fileName, bool, TagLib::AudioProperties::ReadStyle ) const
        {
            KMimeType::Ptr mimetype = KMimeType::findByPath( QFile::decodeName( fileName ) );
            if ( mimetype ) {
                if ( mimetype->name() == QLatin1String( "audio/mpeg" ) )
                    return new TagLib::MPEG::File(fileName);
                else if ( mimetype->name() == QLatin1String( "application/ogg" ) )
                    return new TagLib::Ogg::Vorbis::File(fileName);
                else if ( mimetype->name() == QLatin1String( "application/x-flac" ) )
                    return new TagLib::Ogg::FLAC::File(fileName);
            }

            return 0;
        }
    };
}


class K3bAudioMetainfoRenamerPluginWidget::Private
{
public:
    K3bDataDoc* doc;
    QString pattern;

    KComboBox* comboPattern;
    QTreeWidget* viewFiles;
    //  KProgressDialog* progressDialog;
    QPushButton* scanButton;

    QList< QPair<K3bFileItem*, QTreeWidgetItem*> > renamableItems;
    QHash<K3bDirItem*, QTreeWidgetItem*> dirItemHash;

//   long long scannedSize;
//   int progressCounter;
};


K3bAudioMetainfoRenamerPluginWidget::K3bAudioMetainfoRenamerPluginWidget( K3bDoc* doc,
                                                                          QWidget* parent )
    : QWidget( parent )
{
    d = new Private();
    d->doc = static_cast<K3bDataDoc*>(doc);
    //  d->progressDialog = 0;

    // pattern group
    QGroupBox* patternGroup = new QGroupBox( i18n("Rename Pattern"), this );
    QHBoxLayout* patternGroupLayout = new QHBoxLayout( patternGroup );
    patternGroupLayout->setMargin( KDialog::marginHint() );
    patternGroupLayout->setSpacing( KDialog::spacingHint() );

    d->comboPattern = new KComboBox( patternGroup );
    d->comboPattern->setEditable( true );

    d->scanButton = new QPushButton( i18n("Scan"), patternGroup );
    patternGroupLayout->addWidget( d->comboPattern );
    patternGroupLayout->addWidget( d->scanButton );

    // the files view
    QGroupBox* filesGroup = new QGroupBox( i18n("Found Files"), this );
    QHBoxLayout* filesGroupLayout = new QHBoxLayout( filesGroup );
    filesGroupLayout->setMargin( KDialog::marginHint() );
    filesGroupLayout->setSpacing( KDialog::spacingHint() );

    d->viewFiles = new QTreeWidget( filesGroup );
    d->viewFiles->setHeaderLabels( QStringList() << i18n("New Name") << i18n("Old Name") );
    // FIXME: how about a class that installs an event filter to paint the no item text?
//    d->viewFiles->setNoItemText( i18n("Please click the Scan button to search for renameable files.") );

    filesGroupLayout->addWidget( d->viewFiles );

    // layout
    QVBoxLayout* box = new QVBoxLayout( this );
    box->setMargin( 0 );
    box->setSpacing( KDialog::spacingHint() );

    box->addWidget( patternGroup );
    box->addWidget( filesGroup );

    connect( d->scanButton, SIGNAL(clicked()), this, SLOT(slotScanClicked()) );

    d->scanButton->setToolTip( i18n("Scan for renamable files") );
    d->comboPattern->setWhatsThis( i18n("<qt>This specifies how the files should be renamed. "
                                        "Currently only the special strings <em>%a</em> (Artist), "
                                        "<em>%n</em> (Track number), and <em>%t</em> (Title) ,"
                                        "are supported.") );

    TagLib::FileRef::addFileTypeResolver( new KMimeTypeResolver() );
}


K3bAudioMetainfoRenamerPluginWidget::~K3bAudioMetainfoRenamerPluginWidget()
{
    delete d;
}


QString K3bAudioMetainfoRenamerPluginWidget::title() const
{
    return i18n("Rename Audio Files");
}


QString K3bAudioMetainfoRenamerPluginWidget::subTitle() const
{
    return i18n("Based on meta info");
}


void K3bAudioMetainfoRenamerPluginWidget::loadDefaults()
{
    d->comboPattern->setEditText( "%a - %t" );
}


void K3bAudioMetainfoRenamerPluginWidget::readSettings( const KConfigGroup& grp )
{
    d->comboPattern->setEditText( grp.readEntry( "rename pattern", "%a - %t" ) );
}


void K3bAudioMetainfoRenamerPluginWidget::saveSettings( KConfigGroup grp )
{
    grp.writeEntry( "rename pattern", d->comboPattern->currentText() );
}


void K3bAudioMetainfoRenamerPluginWidget::slotScanClicked()
{
    d->pattern = d->comboPattern->currentText();
    if( d->pattern.isEmpty() ) {
        KMessageBox::error( this, i18n("Please specify a valid pattern.") );
    }
    else {
//     if( d->progressDialog == 0 ) {
//       d->progressDialog = new KProgressDialog( this, "scanning_progress",
// 					       i18n("Scanning..."),
// 					       i18n("Scanning for renameable files."),
// 					       true );
//       d->progressDialog->setAllowCancel(false);
//     }

        K3bDirItem* dir = d->doc->root();

        // clear old searches
        d->viewFiles->clear();
        d->renamableItems.clear();
        d->dirItemHash.clear();
//     d->scannedSize = 0;
//     d->progressCounter = 0;

        // create root item
        QTreeWidgetItem* rootItem = new QTreeWidgetItem( d->viewFiles, QStringList() << QLatin1String( "/" ) );
        rootItem->setIcon( 0, KIcon( "folder" ) );

        //  d->progressDialog->show();
        scanDir( dir, rootItem );
        //    d->progressDialog->close();

        rootItem->setExpanded(true);

        if( d->renamableItems.isEmpty() )
            KMessageBox::sorry( this, i18n("No renameable files found.") );
    }
}


void K3bAudioMetainfoRenamerPluginWidget::scanDir( K3bDirItem* dir, QTreeWidgetItem* viewRoot )
{
    kDebug() << "(K3bAudioMetainfoRenamerPluginWidget) scanning dir " << dir->k3bName();

    d->dirItemHash.insert( dir, viewRoot );

    foreach( K3bDataItem* item, dir->children() ) {
        if( item->isFile() ) {
            if( item->isRenameable() ) {
                QString newName = createNewName( (K3bFileItem*)item );
                if( !newName.isEmpty() ) {
                    QTreeWidgetItem* fileViewItem = new QTreeWidgetItem( viewRoot, QStringList() << newName << item->k3bName() );
                    fileViewItem->setCheckState( 0, Qt::Checked );
                    fileViewItem->setIcon( 0, KIcon( item->mimeType()->iconName() ) );
                    d->renamableItems.append( qMakePair( (K3bFileItem*)item, fileViewItem ) );
                }
            }

//       d->scannedSize += item->k3bSize();
//       d->progressCounter++;
//       if( d->progressCounter > 50 ) {
// 	d->progressCounter = 0;
// 	d->progressDialog->progressBar()->setProgress( 100*d->scannedSize/d->doc->root()->k3bSize() );
// 	qApp->processEvents();
//       }
        }
        else if( item->isDir() ) {
            // create dir item
            K3bDirItem* dirItem = static_cast<K3bDirItem*>( item );
            if ( !dirItem->children().isEmpty() ) {
                QTreeWidgetItem* dirViewItem = new QTreeWidgetItem( viewRoot, QStringList() << item->k3bName() );
                dirViewItem->setIcon( 0, KIcon( "folder" ) );
                scanDir( dirItem, dirViewItem );
                dirViewItem->setExpanded(true);
            }
        }
    }
}


void K3bAudioMetainfoRenamerPluginWidget::activate()
{
    if( d->renamableItems.isEmpty() ) {
        KMessageBox::sorry( this, i18n("Please click the Scan button to search for renameable files.") );
    }
    else {
        for( QList< QPair<K3bFileItem*, QTreeWidgetItem*> >::iterator it = d->renamableItems.begin();
             it != d->renamableItems.end(); ++it ) {
            QPair<K3bFileItem*, QTreeWidgetItem*>& item = *it;

            if( item.second->checkState(0) == Qt::Checked )
                item.first->setK3bName( item.second->text(0) );
        }

        d->viewFiles->clear();
        d->renamableItems.clear();

        KMessageBox::information( this, i18n("Done.") );
    }
}


QString K3bAudioMetainfoRenamerPluginWidget::createNewName( K3bFileItem* item )
{
    TagLib::FileRef file( QFile::encodeName( item->localPath() ).data() );

    if ( !file.isNull() && file.tag() ) {
        QString artist, title, track;
        artist = TStringToQString( file.tag()->artist() );
        title = TStringToQString( file.tag()->title() );
        if ( file.tag()->track() > 0 )
            track = QString::number( file.tag()->track() );

        QString newName;
        for( int i = 0; i < d->pattern.length(); ++i ) {

            if( d->pattern[i] == '%' ) {
                ++i;

                if( i < d->pattern.length() ) {
                    if( d->pattern[i] == 'a' ) {
                        if( artist.isEmpty() )
                            return QString();
                        newName.append(artist);
                    }
                    else if( d->pattern[i] == 'n' ) {
                        if( title.isEmpty() )
                            return QString();
                        newName.append(track);
                    }
                    else if( d->pattern[i] == 't' ) {
                        if( title.isEmpty() )
                            return QString();
                        newName.append(title);
                    }
                    else {
                        newName.append( "%" );
                        newName.append( d->pattern[i] );
                    }
                }
                else {  // end of pattern
                    newName.append( "%" );
                }
            }
            else {
                newName.append( d->pattern[i] );
            }
        }

        // remove white spaces from end and beginning
        newName = newName.trimmed();

        QString extension = item->k3bName().mid( item->k3bName().lastIndexOf('.') );

        if( !newName.isEmpty() ) {
            //
            // Check if files with that name exists and if so append number
            //
            if( existsOtherItemWithSameName( item, newName + extension ) ) {
                kDebug() << "(K3bAudioMetainfoRenamerPluginWidget) file with name "
                         << newName << extension << " already exists" << endl;
                int i = 1;
                while( existsOtherItemWithSameName( item, newName + QString( " (%1)").arg(i) + extension ) )
                    i++;
                newName.append( QString( " (%1)").arg(i) );
            }

            // append extension
            newName.append( extension );
        }

        return newName;
    }
    else
        return QString();
}


bool K3bAudioMetainfoRenamerPluginWidget::existsOtherItemWithSameName( K3bFileItem* item, const QString& name )
{
    K3bDirItem* dir = item->parent();
    K3bDataItem* otherItem = dir->find( name );
    if( otherItem && otherItem != item )
        return true;

    QTreeWidgetItem* dirViewItem = d->dirItemHash[dir];
    for ( int i = 0; i < dirViewItem->childCount(); ++i ) {
        QTreeWidgetItem* current = dirViewItem->child( i );
        if( current->text(0) == name )
            return true;
    }

    return false;
}



K3bAudioMetainfoRenamerPlugin::K3bAudioMetainfoRenamerPlugin( QObject* parent, const QVariantList& )
    : K3bProjectPlugin( DATA_PROJECTS, true, parent )
{
    setText( i18n("Rename Audio Files") );
    setToolTip( i18n("Rename audio files based on their meta info.") );
}


K3bAudioMetainfoRenamerPlugin::~K3bAudioMetainfoRenamerPlugin()
{
}


K3bProjectPluginGUIBase* K3bAudioMetainfoRenamerPlugin::createGUI( K3bDoc* doc, QWidget* parent )
{
    return new K3bAudioMetainfoRenamerPluginWidget( doc, parent );
}

#include "k3baudiometainforenamerplugin.moc"
