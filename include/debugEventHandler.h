#ifndef __DEBUGEVENTHANDLER_H__
#define __DEBUGEVENTHANDLER_H__
#include <osgGA/GUIEventHandler>

namespace cube
{
  class DebugEventHandler
    : public osgGA::GUIEventHandler
  {
    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
  };
}

#endif // __DEBUGEVENTHANDLER_H__