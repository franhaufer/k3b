/* 
 *
 * $Id: sourceheader 511311 2006-02-19 14:51:05Z trueg $
 * Copyright (C) 2006 Sebastian Trueg <trueg@k3b.org>
 *
 * This file is part of the K3b project.
 * Copyright (C) 1998-2006 Sebastian Trueg <trueg@k3b.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * See the file "COPYING" for the exact licensing terms.
 */

#include "k3bvideodvdvideostream.h"

unsigned int K3bVideoDVD::VideoStream::pictureWidth() const
{
  switch( pictureSize() ) {
  case VIDEO_PICTURE_SIZE_720:
    return 720;
  case VIDEO_PICTURE_SIZE_704:
    return 704;
  case VIDEO_PICTURE_SIZE_352:
  case VIDEO_PICTURE_SIZE_352_2:
    return 352;
  default:
    return 0;
  }
}


unsigned int K3bVideoDVD::VideoStream::pictureHeight() const
{
  int height = 480;
  if( format() != 0 )
    height = 576;
  if( pictureSize() == VIDEO_PICTURE_SIZE_352_2 )
    height /= 2;

  return height;
}
