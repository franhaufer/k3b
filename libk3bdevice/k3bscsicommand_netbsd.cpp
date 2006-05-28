/*
 *
 */

#include "k3bscsicommand.h"
#include "k3bdevice.h"

#include <kdebug.h>

#include <sys/ioctl.h>
#include <sys/scsiio.h>
// #include <sys/cdio.h>
// #include <sys/dkio.h>

#include <unistd.h>
#include <sys/types.h>


class K3bDevice::ScsiCommand::Private
{
public:
  struct scsireq cmd;
};


void K3bDevice::ScsiCommand::clear()
{
  ::memset( &d->cmd, 0, sizeof(struct scsireq ) );
}


unsigned char& K3bDevice::ScsiCommand::operator[]( size_t i )
{
  return d->cmd.cmd[i];
}


int K3bDevice::ScsiCommand::transport( TransportDirection dir,
				       void* data,
				       size_t len )
{
  bool needToClose = false;
  if( m_device ) {
    if( !m_device->isOpen() ) {
      needToClose = true;
    }
    m_device->open( dir == TR_DIR_WRITE );
    m_deviceHandle = m_device->handle();
  }

  if( m_deviceHandle == -1 )
    return -1;

  d->cmd.cmdlen = 12;
  d->cmd.timeout = 10000;
  d->cmd.databuf = (caddr_t) data;
  d->cmd.datalen = len;
  //  d->cmd.datalen_used = len;
  d->cmd.senselen = SENSEBUFLEN;
  switch (dir)
  {
    case TR_DIR_READ:
      d->cmd.flags = SCCMD_READ;
      break;
    case TR_DIR_WRITE:
      d->cmd.flags = SCCMD_WRITE;
      break;
    default:
      d->cmd.flags = SCCMD_READ;
      break;
  }

  int i = ::ioctl( m_deviceHandle, SCIOCCOMMAND, &d->cmd );

  if( needToClose )
    m_device->close();

  if( i || (d->cmd.retsts != SCCMD_OK)) {
    debugError( d->cmd.cmd[0],
		d->cmd.retsts,
		d->cmd.sense[2],
		d->cmd.sense[12],
		d->cmd.sense[13] );

     return 1;
  }
  else
    return 0;
}
