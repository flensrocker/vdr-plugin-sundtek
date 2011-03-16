#ifndef __SUNDTEK_MEDIA_H
#define __SUNDTEK_MEDIA_H

#include <sundtek/mediaclient.h>


class cSundtekMedia
{
private:
  static void *_lib;

public:
  static bool Init(void);

  static int (*netConnect)(void);
  static int (*netClose)(int);
  static struct media_device_enum *(*netDeviceEnum)(int, int*, int);
};

#endif
