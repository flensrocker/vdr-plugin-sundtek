#include "monitor.h"
#include "device.h"

#include <sys/socket.h>

#include <sundtek/mediaclient.h>


cSundtekMonitor *cSundtekMonitor::_monitor = NULL;


cSundtekMonitor::cSundtekMonitor(void)
{
}

cSundtekMonitor::~cSundtekMonitor(void)
{
}

void cSundtekMonitor::StartMonitor(void)
{
  if (_monitor != NULL)
     return;
  _monitor = new cSundtekMonitor;
  if (_monitor)
     _monitor->Start();
}

void cSundtekMonitor::StopMonitor(void)
{
  if (_monitor == NULL)
     return;
  _monitor->Cancel(5);
  delete _monitor;
  _monitor = NULL;
}

static char *GetFrontend(int devid)
{
  if (devid < 0)
     return NULL;
  int fd = net_connect();
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
  int fd = net_connect();
  if (fd < 0) {
    esyslog("sundtek: can't connect monitor to mediasrv");
    return;
  }
  char cmd = MEDIA_CMD_NOTIFICATION;
  if (send(fd, &cmd, 1, MSG_NOSIGNAL) < 0)
    esyslog("sundtek: error on send: %d", errno);
  else {
    char buffer[1024];
    struct pollfd pfd;
    pfd.fd = fd;
    pfd.events = POLLIN | POLLHUP;
    while (true) {
          int count = poll(&pfd, 1, 1000);
          if (!Running())
              break;
          if (count == 0)
             continue;
          if (count < 0) {
             esyslog("sundtek: error on poll: %d", errno);
             continue;
             }
          if (pfd.revents & (POLLHUP | POLLERR | POLLNVAL)) {
             esyslog("sundtek: error on poll: %d", errno);
             break;
             }
          if (pfd.revents & POLLIN) {
             int nread = recv(fd, buffer, 1024, MSG_DONTWAIT);
             if (nread == sizeof(struct media_device_notification)) {
                struct media_device_notification *mdn = (struct media_device_notification*)buffer;
                if (mdn->status == DEVICE_ATTACHED) {
                   char *frontend = GetFrontend(mdn->deviceid);
                   if (frontend) {
                      cSundtekDevice::Attach(mdn->deviceid, frontend);
                      free(frontend);
                      }
                   }
                else if (mdn->status == DEVICE_DETACHED)
                   cSundtekDevice::Detach(mdn->deviceid);
                }
             }
          }
    }
  net_close(fd);
}
