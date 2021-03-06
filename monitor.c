#include "monitor.h"
#include "device.h"

#include <sys/socket.h>

extern "C" {
#include <mediaclient.h>
}


cMutex cSundtekMonitor::_monitorMutex;
cSundtekMonitor *cSundtekMonitor::_monitor = NULL;


cSundtekMonitor::cSundtekMonitor(void)
{
}

cSundtekMonitor::~cSundtekMonitor(void)
{
}

void cSundtekMonitor::StartMonitor(void)
{
  cMutexLock lock(&_monitorMutex);
  if (_monitor != NULL)
     return;
  _monitor = new cSundtekMonitor;
  if (_monitor) {
     _monitor->Start();
     isyslog("sundtek: monitor started");
     }
}

void cSundtekMonitor::StopMonitor(void)
{
  cMutexLock lock(&_monitorMutex);
  if (_monitor == NULL)
     return;
  _monitor->Cancel(5);
  delete _monitor;
  _monitor = NULL;
  isyslog("sundtek: monitor stopped");
}

static char *GetFrontend(int devid)
{
  if (devid < 0)
     return NULL;
  int fd = net_connect(0);
  if (fd < 0)
     return NULL;
  char *frontend = NULL;
  int id = devid;
  struct media_device_enum *e = net_device_enum(fd, &id, 0);
  if (e) {
     frontend = strdup((const char*)e->frontend_node);
     free(e);
     }
  net_close(fd);
  return frontend;
}

void cSundtekMonitor::Action(void)
{
  int fd = net_register_notification();
  if (fd < 0) {
    esyslog("sundtek: can't connect monitor to mediasrv");
    return;
  }

  struct pollfd pfd;
  pfd.fd = fd;
  pfd.events = POLLIN | POLLHUP;
  while (true) {
        int count = poll(&pfd, 1, 1000);
        if (count == 0) {
           if (!Running())
               break;
           continue;
           }
        if (count < 0) {
           esyslog("sundtek: error on poll: %d", errno);
           if (!Running())
               break;
           continue;
           }
        if (pfd.revents & (POLLHUP | POLLERR | POLLNVAL)) {
           esyslog("sundtek: error on poll: %d", errno);
           break;
           }
        if (pfd.revents & POLLIN) {
           struct media_device_notification *mdn = net_read_notification(fd);
           if (mdn) {
              if (mdn->status == DEVICE_ATTACHED) {
                 char *frontend = GetFrontend(mdn->deviceid);
                 if (frontend) {
                    cSundtekDevice::Attach(mdn->deviceid, frontend);
                    free(frontend);
                    }
                 }
              else if (mdn->status == DEVICE_DETACHED)
                 cSundtekDevice::Detach(mdn->deviceid);
              free(mdn);
              }
           }
        }
  net_close(fd);
}
