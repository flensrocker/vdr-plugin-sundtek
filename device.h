#ifndef __SUNDTEK_DEVICE_H
#define __SUNDTEK_DEVICE_H

#include <vdr/tools.h>
#include <vdr/thread.h>


class cSundtekDevice : public cListObject
{
private:
  static cMutex _devicesMutex;
  static cList<cSundtekDevice> _devices;
  int     _deviceId;
  cString _frontend;

  cSundtekDevice(int DeviceId, const char *Frontend);

public:
  virtual ~cSundtekDevice(void);

  static void Enumerate(const char *Frontend);
  static void FreeAll(void);

  static int  GetDeviceId(const char *Frontend);

  static void Attach(int DeviceId, const char *Frontend);
  static void Detach(int DeviceId);
};

#endif
