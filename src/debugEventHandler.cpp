#include <debugEventHandler.h>
#include <world.h>

namespace cube
{

bool DebugEventHandler::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  switch( ea.getEventType() )
  {
  case osgGA::GUIEventAdapter::KEYDOWN:
    {
      switch(ea.getUnmodifiedKey())
      {
      case osgGA::GUIEventAdapter::KEY_Home:
        {
          //float sun;
          //cube::World::Instance()._worldLight->get(sun);
          //cube::World::Instance()._worldLight->set(sun + 0.05f);
          return true;
        }
        break;

      case osgGA::GUIEventAdapter::KEY_End:
        {
          //float sun;
          //cube::World::Instance()._worldLight->get(sun);
          //cube::World::Instance()._worldLight->set(sun - 0.05f);
          return true;
        }
        break;
      }
    }
  }
  return false;
}


}