#include "device.h"

#include <sundtek/mediaclient.h>

#include <vdr/plugin.h>


cList<cSundtekDevice> cSundtekDevice::_devices;


cSundtekDevice::cSundtekDevice(int DeviceId, const char *Frontend)
:_deviceId(DeviceId)
,_frontend(Frontend)
{
  _devices.Add(this);
  isyslog("sundtek: new Sundtek device, id = %d, frontend = %s", _deviceId, *_frontend);
}

cSundtekDevice::~cSundtekDevice(void)
{
  _devices.Del(this, false);
  isyslog("sundtek: delete Sundtek device, id = %d, frontend = %s", _deviceId, *_frontend);
}

void cSundtekDevice::Enumerate(void)
{
  int fd = net_connect();
  if (fd < 0) {
     esyslog("sundtek: can't connect to mediasrv for enumeration of devices");
     return;
     }
  int id = 0;
  while (true) {
        int subid = 0;
        while (true) {
              struct media_device_enum *e = net_device_enum(fd, &id, subid);
              if (!e)
                 break;
              new cSundtekDevice(id, (const char*)e->frontend_node);
              subid++;
              free(e);
              }
        if (subid == 0)
           break;
        id++;
        }
  net_close(fd);
}

void cSundtekDevice::FreeAll(void)
{
  _devices.Clear();
}

void cSundtekDevice::Attach(int DeviceId, const char *Frontend)
{
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
  cPluginManager::CallFirstService("dynamite-AttachDevice-v0.1", (void*)Frontend);
}

void cSundtekDevice::Detach(int DeviceId)
{
  for (cSundtekDevice *d = _devices.First(); d; d = _devices.Next(d)) {
      if (d->_deviceId == DeviceId) {
         cPluginManager::CallFirstService("dynamite-DetachDevice-v0.1", (void*)*d->_frontend);
         delete d;
         return;
         }
      }
}