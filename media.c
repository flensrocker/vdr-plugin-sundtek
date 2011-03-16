#include "media.h"

#include <dlfcn.h>

#include <vdr/tools.h>


void *cSundtekMedia::_lib = NULL;

int (*cSundtekMedia::netConnect)(void);
int (*cSundtekMedia::netClose)(int);
struct media_device_enum *(*cSundtekMedia::netDeviceEnum)(int, int*, int);

bool cSundtekMedia::Init(void)
{
  if (_lib != NULL)
     return true;
  _lib = dlopen("/opt/lib/libmedia.so", RTLD_NOW);
  const char *error = dlerror();
  if (error) {
     esyslog("sundtek: can't load libmedia.so: %s", error);
     return false;
     }

  bool ret = true;
  netConnect = (int(*)(void))dlsym(_lib, "net_connect");
  error = dlerror();
  if (error) {
     esyslog("sundtek: can't find net_connect: %s", error);
     ret = false;
     }

  netClose = (int(*)(int))dlsym(_lib, "net_close");
  error = dlerror();
  if (error) {
     esyslog("sundtek: can't find net_close: %s", error);
     ret = false;
     }

  netDeviceEnum = (struct media_device_enum *(*)(int, int*, int))dlsym(_lib, "net_device_enum");
  error = dlerror();
  if (error) {
     esyslog("sundtek: can't find net_device_enum: %s", error);
     ret = false;
     }

  if (!ret) {
     netConnect = NULL;
     netClose = NULL;
     netDeviceEnum = NULL;
     dlclose(_lib);
     _lib = NULL;
     }
  return ret;
}
