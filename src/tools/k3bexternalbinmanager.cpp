#include "k3bexternalbinmanager.h"

#include <kdebug.h>
#include <kprocess.h>
#include <kconfig.h>

#include <qstring.h>
#include <qregexp.h>
#include <qfile.h>


#define TRANSCODE_START 3
#define TRANSCODE_END 9

// binary program name without path
// to add a new one add the name at the end of the array and implement your own
// version of checkVersions and add a case in slotParseOutputVersion.

static const int NUM_BIN_PROGRAMS = 9;
static const int NUM_SEARCH_PATHS = 5;

static const char* binPrograms[] =  { "mkisofs",
				      "cdrecord",
				      "cdrdao",
				      "transcode",
				      "tcprobe",
				      "tccat",
				      "tcscan",
				      "tcextract",
				      "tcdecode" };
static const char* binVersions[] =  { "1.13",
				      "1.9",
				      "1.1.3",
				      "0.6.0pre3",
				      "0.6.0pre3",
				      "0.6.0pre3",
				      "0.6.0pre3",
				      "0.6.0pre3",
				      "0.6.0pre3" };
static const char* binVersionFlag[] =  { "--version",
					 "--version",
					 "--version",
					 "-version",
					 "-version",
					 "-version",
					 "-version",
					 "-version",
					 "-version" };

static const char* searchPaths[] = { "/usr/bin/",
				     "/usr/local/bin/",
				     "/usr/sbin/",
				     "/usr/local/sbin/",
				     "/opt/schily/bin/" };



K3bExternalBin::K3bExternalBin( const QString& name )
 : m_name( name )
{
}


bool K3bExternalBin::isEmpty() const
{
  return m_name.isEmpty();
}


const QString& K3bExternalBin::name() const
{
  return m_name;
}


bool K3bExternalBin::hasFeature( const QString& f ) const
{
  return m_features.contains( f );
}


void K3bExternalBin::addFeature( const QString& f )
{
  m_features.append( f );

  // cdrecord features:
  // -clone
  // gracetime
  // -overburn

  // readcd features:
  // -clone

  // mkisofs features:
  // -udf
}


void K3bExternalBin::addUserParameter( const QString& p )
{
  m_userParameters.append( p );
}




K3bExternalBinManager::K3bExternalBinManager( QObject* parent )
  : QObject( parent )
{
    m_process = new KProcess();

    for( int i=0; i< NUM_BIN_PROGRAMS; i++) {
        kdDebug() << "New bin object: " << binPrograms[ i ] << endl;
        m_binMap.insert( binPrograms[ i ], new K3bExternalBin( binPrograms[ i ] ));
    }
}


K3bExternalBinManager::~K3bExternalBinManager()
{
  delete m_process;

  QMap<QString, K3bExternalBin*>::Iterator it = m_binMap.begin();
  for( ; it != m_binMap.end(); ++it )
    delete it.data();
}

void K3bExternalBinManager::search() {
    for( int i=0; i<NUM_BIN_PROGRAMS; i++ ) {
        searchVersion( i );
    }
}

void K3bExternalBinManager::searchVersion( int programArrayIndex ){
    // search for program
    for( int i = 0; i < NUM_SEARCH_PATHS; i++ ) {
        QString bin = QString("%1%2").arg(searchPaths[i]).arg( binPrograms[ programArrayIndex ] );
        kdDebug() << "(K3bExternalBinManager) Check program " << bin << endl;
        if( QFile::exists( bin ) ) {
            // shit kde3, hangs up when parsing output each second time, fucking bullshit kde3,
            // seems to be working if deleting and creating new instance of kprocess.
            delete m_process;
            m_process = new KProcess();
            m_process->clearArguments();
            m_process->disconnect();
            m_process->setName( binPrograms[ programArrayIndex ] );
            *m_process << bin << binVersionFlag[ programArrayIndex ];
            connect( m_process, SIGNAL(receivedStdout(KProcess*, char*, int)),
	          this, SLOT(slotParseOutputVersion(KProcess*, char*, int)) );
            connect( m_process, SIGNAL(receivedStderr(KProcess*, char*, int)),
	          this, SLOT(slotParseOutputVersion(KProcess*, char*, int)) );
            m_process->start( KProcess::Block, KProcess::AllOutput );
            kdDebug() << "Process started, go on" << endl;
            if( m_binMap[  binPrograms[ programArrayIndex ] ]->version.isEmpty() ) {
                kdDebug() << "(K3bExternalBinManager) " << bin << " seems not to be "<<
                binPrograms[ programArrayIndex ]<<" version >= " << binVersions[ programArrayIndex ];
            } else {
                kdDebug() << "(K3bExternalBinManager) Set path " << bin << endl;
                m_binMap[ binPrograms[ programArrayIndex ] ]->path = bin;
                break;  // bin found
            }
        }
    }
    if( m_binMap[ binPrograms[ programArrayIndex ] ]->path.isEmpty() ) {
        kdDebug() << "(K3bExternalBinManager) Could not find " << binPrograms[ programArrayIndex ] << endl;
    }
}

void K3bExternalBinManager::checkVersions()
{
  // search for mkisofs
  K3bExternalBin* binO = binObject("mkisofs");
  if( binO ) {
    binO->version = QString::null;
    if( QFile::exists( binO->path ) ) {

      delete m_process;
      m_process = new KProcess();
      m_process->clearArguments();
      m_process->disconnect();
      *m_process << binO->path << "--version";
      connect( m_process, SIGNAL(receivedStdout(KProcess*, char*, int)),
	       this, SLOT(slotParseCdrtoolsVersion(KProcess*, char*, int)) );
      m_process->start( KProcess::Block, KProcess::Stdout );
      if( m_binMap["mkisofs"]->version.isEmpty() ) {
	kdDebug() << "(K3bExternalBinManager) " << binO->path << " seems not to be mkisofs." << endl;
      }
    }
  }

  binO = binObject("cdrecord");
  if( binO ) {
    binO->version = QString::null;
    if( QFile::exists( binO->path ) ) {

      delete m_process;
      m_process = new KProcess();
      m_process->clearArguments();
      m_process->disconnect();
      *m_process << binO->path << "--version";
      connect( m_process, SIGNAL(receivedStdout(KProcess*, char*, int)),
	       this, SLOT(slotParseCdrtoolsVersion(KProcess*, char*, int)) );
      m_process->start( KProcess::Block, KProcess::Stdout );
      if( m_binMap["cdrecord"]->version.isEmpty() ) {
	kdDebug() << "(K3bExternalBinManager) " << binO->path << " seems not to be cdrecord." << endl;
      }
    }
  }

  binO = binObject("cdrdao");
  if( binO ) {
    binO->version = QString::null;
    if( QFile::exists( binO->path ) ) {

      delete m_process;
      m_process = new KProcess();
      m_process->clearArguments();
      m_process->disconnect();
      *m_process << binO->path;
      connect( m_process, SIGNAL(receivedStderr(KProcess*, char*, int)),
	       this, SLOT(slotParseCdrdaoVersion(KProcess*, char*, int)) );
      m_process->start( KProcess::Block, KProcess::Stderr );
      if( m_binMap["cdrdao"]->version.isEmpty() ) {
	kdDebug() << "(K3bExternalBinManager) " << binO->path << " seems not to be cdrdao." << endl;
      }
    }
  }

  checkTranscodeVersion();
}

void K3bExternalBinManager::checkTranscodeVersion(){
    for( int i=TRANSCODE_START;  i < TRANSCODE_END; i++){
        K3bExternalBin* binO = binObject( binPrograms[ i ] );
        bool wrongVersion = false;
        if( binO ) {
            QString version = binO->version; //
            QString tmp = version.mid(0,1);
            int x = tmp.toInt();
            if(  x > 0 ){
                break; // 1.xx or greater is ok
            }
            version = version.mid(2);
            tmp = version.mid(0,1);
            if( tmp.toInt() > 6 ) {
                break; // 0.7 or greater is ok
            }
            if( tmp.toInt() > 5 ){
                if( version.mid(2,3) > 0)
                   break; // 0.6.1 or greater is ok
                version = version.mid(6);
                if( version.mid(0,1).toInt() < 3)
                    wrongVersion=true;
            } else {
                wrongVersion=true;
            }
            if( wrongVersion )
                kdDebug() << "(K3bExternalBinManager) " << binO->path << " seems not to be at least version 0.6.0pre3." << endl;
        } else {
            kdDebug() << "K3bExternalBinManager) Fatal Error: can't initialize " << binPrograms[i] << endl;
        }
    }
}

void K3bExternalBinManager::slotParseOutputVersion( KProcess* p, char* data, int len ) {
    QString task( p->name() );
    kdDebug() << "(K3bExternalBinManager) Check output of bin " << task << endl;
    int index = 0;
    for( int i=0; i< NUM_BIN_PROGRAMS; i++) {
        if( task == binPrograms[ i ] )
            break;
        index++;
    }
    switch( index ){
        case 0:
            slotParseCdrtoolsVersion(p, data, len);
            break;
        case 1:
            slotParseCdrtoolsVersion(p, data, len);
            break;
        case 2:
            slotParseCdrdaoVersion(p, data, len);
            break;
        case 3:
            slotParseTranscodeVersion(p, data, len);
            break;
        case 4:
            slotParseTranscodeVersion(p, data, len);
            break;
        case 5:
            slotParseTranscodeVersion(p, data, len);
            break;
        case 6:
            slotParseTranscodeVersion(p, data, len);
            break;
        case 7:
            slotParseTranscodeVersion(p, data, len);
            break;
        case 8:
            slotParseTranscodeVersion(p, data, len);
            break;
        default:
            kdDebug() << "(K3bExternalBinManager) BUG: Version check failed, add new case in K3bExternalBinManager." << endl;
            break;
    }
}


void K3bExternalBinManager::slotParseCdrtoolsVersion( KProcess*p, char* data, int len ){
    QString buffer = QString::fromLocal8Bit( data, len );
    int start = buffer.find( QRegExp("[0-9]") );
    int findStart = ( start > -1 ? start : 0 );
    int end   = buffer.find( ' ', findStart );
    kdDebug() << "Check cdrtools: " << buffer << endl;
    if( start > -1 && end > -1 ) {
        if( buffer.contains( "mkisofs" ) ) {
            if( m_binMap.contains( "mkisofs" ) ) {
                m_binMap["mkisofs"]->version = buffer.mid( start, end-start );
            }
        } else if( buffer.contains( "Cdrecord" ) ) {
            if( m_binMap.contains( "cdrecord" ) ) {
                m_binMap["cdrecord"]->version = buffer.mid( start, end-start );
            }
        }
    }
}




void K3bExternalBinManager::slotParseCdrdaoVersion( KProcess*, char* data, int len )
{
  if( m_binMap.contains( "cdrdao" ) ) {
    QString buffer = QString::fromLocal8Bit( data, len );
    QStringList lines = QStringList::split( "\n", buffer );

    for( QStringList::Iterator str = lines.begin(); str != lines.end(); str++ ) {
      if( (*str).startsWith( "Cdrdao version" ) ) {
	int start = (*str).find( "version" ) + 8;
	int findStart = ( start > -1 ? start : 0 );
	int end   = (*str).find( ' ', findStart );

	if( start > -1 && end > -1 ) {
	  m_binMap["cdrdao"]->version = (*str).mid( start, end-start );
	  break;   // version found
	}
      }
    }
  }
}


void K3bExternalBinManager::slotParseTranscodeVersion( KProcess* p, char* data, int len ) {
    QString name( p->name() );
    kdDebug() << "version parsing for " << p->name() << endl;
    if( m_binMap.contains( name ) ) {
        kdDebug() << "version parsing" << endl;
        QString buffer = QString::fromLocal8Bit( data, len );
        QStringList lines = QStringList::split( "\n", buffer );

        for( QStringList::Iterator str = lines.begin(); str != lines.end(); str++ ) {
            if( (*str).contains( "transcode v" ) ) {
                int start = (*str).find( "transcode v" ) + 11;
                if( start > -1 ) {
                    kdDebug() << "version found" << endl;
                    int end = (*str).find( ")", start );
                    m_binMap[ name ]->version = (*str).mid( start, end-start ); // 0.6.0pre3-20010101
                    break;   // version found
                }
            }
        }
    }
}

bool K3bExternalBinManager::readConfig( KConfig* c )
{
  for( int i=0; i< NUM_BIN_PROGRAMS; i++) {
    if( c->hasKey( QString(binPrograms[ i ]) + " path" ) ) {
        QString path = c->readEntry( QString( binPrograms[ i ] )+ " path" );
        if( QFile::exists( path ) ) {
            m_binMap[ binPrograms[ i ] ]->path = path;
        } else {
            kdDebug() << "(K3bExternalBinManager) config contains invalid " << binPrograms[ i ] << " path" << endl;
        }
    }
  }
  return true;
}

bool K3bExternalBinManager::saveConfig( KConfig* c )
{
    for( int i=0; i< NUM_BIN_PROGRAMS; i++) {
      if( QFile::exists( m_binMap[ binPrograms[ i ] ]->path ) )
            c->writeEntry( QString( binPrograms[ i ] ) + " path", m_binMap[ binPrograms[ i ] ]->path );
    }
  return true;
}


bool K3bExternalBinManager::foundBin( const QString& name )
{
  if( !binObject( name ) )
    return false;
  else
    return ( !binObject( name )->version.isEmpty() );
}


const QString& K3bExternalBinManager::binPath( const QString& name )
{
  K3bExternalBin* extBin = binObject( name );
  if( extBin != 0 )
    return extBin->path;

  return m_noPath;
}


K3bExternalBin* K3bExternalBinManager::binObject( const QString& name )
{
  if( m_binMap.contains( name ) )
    return m_binMap[name];
  else
    return 0;
}


QPtrList<K3bExternalBin> K3bExternalBinManager::list() const
{
  QList<K3bExternalBin> l;

  QMap<QString, K3bExternalBin*>::const_iterator it = m_binMap.begin();
  for( ; it != m_binMap.end(); ++it )
    l.append( it.data() );

  return l;
}


#include "k3bexternalbinmanager.moc"

