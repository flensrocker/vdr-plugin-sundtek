#ifndef __SUNDTEK_DEVICE_H
#define __SUNDTEK_DEVICE_H

#include <vdr/tools.h>


class cSundtekDevice : public cListObject
{
private:
  static cList<cSundtekDevice> _devices;
  int     _deviceId;
  cString _frontend;

  cSundtekDevice(int DeviceId, const char *Frontend);

public:
  virtual ~cSundtekDevice(void);

  static void Enumerate(void);
  static void FreeAll(void);

  static void Attach(int DeviceId, const char *Frontend);
  static void Detach(int DeviceId);
};

#endif
