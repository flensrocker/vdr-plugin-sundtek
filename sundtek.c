/*
 * sundtek.c: A plugin for the Video Disk Recorder
 *
 * See the README file for copyright information and how to reach the author.
 *
 * $Id$
 */

#include "device.h"
#include "monitor.h"

#include <vdr/plugin.h>

static const char *VERSION        = "0.0.3a";
static const char *DESCRIPTION    = "support for special Sundtek device features";
static const char *MAINMENUENTRY  = NULL;

class cPluginSundtek : public cPlugin {
private:
  // Add any member variables or functions you may need here.
public:
  cPluginSundtek(void);
  virtual ~cPluginSundtek();
  virtual const char *Version(void) { return VERSION; }
  virtual const char *Description(void) { return DESCRIPTION; }
  virtual const char *CommandLineHelp(void);
  virtual bool ProcessArgs(int argc, char *argv[]);
  virtual bool Initialize(void);
  virtual bool Start(void);
  virtual void Stop(void);
  virtual void Housekeeping(void);
  virtual void MainThreadHook(void);
  virtual cString Active(void);
  virtual time_t WakeupTime(void);
  virtual const char *MainMenuEntry(void) { return MAINMENUENTRY; }
  virtual cOsdObject *MainMenuAction(void);
  virtual cMenuSetupPage *SetupMenu(void);
  virtual bool SetupParse(const char *Name, const char *Value);
  virtual bool Service(const char *Id, void *Data = NULL);
  virtual const char **SVDRPHelpPages(void);
  virtual cString SVDRPCommand(const char *Command, const char *Option, int &ReplyCode);
  };

cPluginSundtek::cPluginSundtek(void)
{
  // Initialize any member variables here.
  // DON'T DO ANYTHING ELSE THAT MAY HAVE SIDE EFFECTS, REQUIRE GLOBAL
  // VDR OBJECTS TO EXIST OR PRODUCE ANY OUTPUT!
  cSundtekDevice::Enumerate(NULL);
  cSundtekMonitor::StartMonitor();
}

cPluginSundtek::~cPluginSundtek()
{
  // Clean up after yourself!
  cSundtekMonitor::StopMonitor();
  cSundtekDevice::FreeAll();
}

const char *cPluginSundtek::CommandLineHelp(void)
{
  // Return a string that describes all known command line options.
  return NULL;
}

bool cPluginSundtek::ProcessArgs(int argc, char *argv[])
{
  // Implement command line argument processing here if applicable.
  return true;
}

bool cPluginSundtek::Initialize(void)
{
  // Initialize any background activities the plugin shall perform.
  return true;
}

bool cPluginSundtek::Start(void)
{
  // Start any background activities the plugin shall perform.
  return true;
}

void cPluginSundtek::Stop(void)
{
  // Stop any background activities the plugin is performing.
}

void cPluginSundtek::Housekeeping(void)
{
  // Perform any cleanup or other regular tasks.
}

void cPluginSundtek::MainThreadHook(void)
{
  // Perform actions in the context of the main program thread.
  // WARNING: Use with great care - see PLUGINS.html!
}

cString cPluginSundtek::Active(void)
{
  // Return a message string if shutdown should be postponed
  return NULL;
}

time_t cPluginSundtek::WakeupTime(void)
{
  // Return custom wakeup time for shutdown script
  return 0;
}

cOsdObject *cPluginSundtek::MainMenuAction(void)
{
  // Perform the action when selected from the main VDR menu.
  return NULL;
}

cMenuSetupPage *cPluginSundtek::SetupMenu(void)
{
  // Return a setup menu in case the plugin supports one.
  return NULL;
}

bool cPluginSundtek::SetupParse(const char *Name, const char *Value)
{
  // Parse your own setup parameters and store their values.
  return false;
}

bool cPluginSundtek::Service(const char *Id, void *Data)
{
  if ((strcmp(Id, "dynamite-event-DeviceAttached-v0.1") == 0) && (Data != NULL)) {
     int deviceId = cSundtekDevice::GetDeviceId((const char*)Data);
     if (deviceId < 0)
        cSundtekDevice::Enumerate((const char*)Data);
     return true;
     }
  else if ((strcmp(Id, "dynamite-event-DeviceDetached-v0.1") == 0) && (Data != NULL)) {
     int deviceId = cSundtekDevice::GetDeviceId((const char*)Data);
     if (deviceId >= 0)
        cSundtekDevice::Detach(deviceId);
     return true;
     }
  else if ((strcmp(Id, "sundtek-Monitor-v0.1") == 0) && (Data != NULL)) {
     if (strcmp((const char*)Data, "start") == 0) {
        cSundtekDevice::Enumerate(NULL);
        cSundtekMonitor::StartMonitor();
        }
     else if (strcmp((const char*)Data, "stop") == 0)
        cSundtekMonitor::StopMonitor();
     return true;
     }
  else if (strcmp(Id, "sundtek-Enumerate-v0.1") == 0) {
     cSundtekDevice::Enumerate(NULL);
     return true;
     }
  return false;
}

const char **cPluginSundtek::SVDRPHelpPages(void)
{
  // Return help text for SVDRP commands this plugin implements
  return NULL;
}

cString cPluginSundtek::SVDRPCommand(const char *Command, const char *Option, int &ReplyCode)
{
  // Process SVDRP commands this plugin implements
  return NULL;
}

VDRPLUGINCREATOR(cPluginSundtek); // Don't touch this!
