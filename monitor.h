#ifndef __SUNDTEK_MONITOR_H
#define __SUNDTEK_MONITOR_H

#include <vdr/thread.h>


class cSundtekMonitor : public cThread
{
private:
  static cMutex _monitorMutex;
  static cSundtekMonitor *_monitor;

  cSundtekMonitor(void);

public:
  virtual ~cSundtekMonitor(void);

  static void StartMonitor(void);
  static void StopMonitor(void);

protected:
  virtual void Action(void);
};

#endif
