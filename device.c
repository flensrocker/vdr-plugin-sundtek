#include "device.h"

extern "C" {
#include <mediaclient.h>
}

#include <vdr/plugin.h>


cMutex cSundtekDevice::_devicesMutex;
cList<cSundtekDevice> cSundtekDevice::_devices;


cSundtekDevice::cSundtekDevice(int DeviceId, const char *Frontend)
:_deviceId(DeviceId)
,_frontend(Frontend)
{
  cMutexLock lock(&_devicesMutex);
  _devices.Add(this);
  isyslog("sundtek: new Sundtek device, id = %d, frontend = %s", _deviceId, *_frontend);
}

cSundtekDevice::~cSundtekDevice(void)
{
  cMutexLock lock(&_devicesMutex);
  _devices.Del(this, false);
  isyslog("sundtek: delete Sundtek device, id = %d, frontend = %s", _deviceId, *_frontend);
}

void cSundtekDevice::Enumerate(const char *Frontend)
{
  int fd = net_connect(0);
  if (fd < 0) {
     esyslog("sundtek: can't connect to mediasrv for enumeration of devices");
     return;
     }
  int id = 0;
  struct media_device_enum *e = NULL;
  while (true) {
        int subid = 0;
        while (true) {
              e = net_device_enum(fd, &id, subid);
              if (!e)
                 break;
              if (((Frontend == NULL) || (strcmp((const char*)e->frontend_node, Frontend) == 0))
                 && (GetDeviceId((const char*)e->frontend_node) < 0)) {
                 isyslog("sundtek: enumerate devices: found id %d, frontend %s", id, (const char*)e->frontend_node);
                 Attach(id, (const char*)e->frontend_node);
                 if (Frontend != NULL)
                    goto close;
                 }
              subid++;
              free(e);
              e = NULL;
              }
        if (subid == 0)
           break;
        id++;
        }
close:
  if (e)
     free(e);
  net_close(fd);
}

void cSundtekDevice::FreeAll(void)
{
  cMutexLock lock(&_devicesMutex);
  _devices.Clear();
}

int  cSundtekDevice::GetDeviceId(const char *Frontend)
{
  cMutexLock lock(&_devicesMutex);
  for (cSundtekDevice *d = _devices.First(); d; d = _devices.Next(d)) {
      if (strcmp(*d->_frontend, Frontend) == 0)
         return d->_deviceId;
      }
  return -1;
}

void cSundtekDevice::Attach(int DeviceId, const char *Frontend)
{
  cMutexLock lock(&_devicesMutex);
  for (cSundtekDevice *d = _devices.First(); d; d = _devices.Next(d)) {
      if (strcmp(*d->_frontend, Frontend) == 0) {
         if (d->_deviceId != DeviceId) {
            esyslog("sundtek: device id mismatch at frontend %s -> assign new id %d", Frontend, DeviceId);
            d->_deviceId = DeviceId;
            }
         return;
         }
      }
  new cSundtekDevice(DeviceId, Frontend);
#ifdef __DYNAMIC_DEVICE_PROBE
  cDynamicDeviceProbe::QueueDynamicDeviceCommand(ddpcAttach, Frontend);
#endif
}

void cSundtekDevice::Detach(int DeviceId)
{
  cMutexLock lock(&_devicesMutex);
  for (cSundtekDevice *d = _devices.First(); d; d = _devices.Next(d)) {
      if (d->_deviceId == DeviceId) {
         char *frontend = strdup(*d->_frontend);
         delete d;
         cPluginManager::CallFirstService("dynamite-ForceDetachDevice-v0.1", (void*)frontend);
         free(frontend);
         return;
         }
      }
}
