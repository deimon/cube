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
          float sun;
          cube::World::Instance().GetSunUniform()->get(sun);
          cube::World::Instance().GetSunUniform()->set(osg::minimum(sun + 0.01f, 1.0f));
          return true;
        }
        break;

      case osgGA::GUIEventAdapter::KEY_End:
        {
          float sun;
          cube::World::Instance().GetSunUniform()->get(sun);
          cube::World::Instance().GetSunUniform()->set(osg::maximum(sun - 0.01f, 0.0f));
          return true;
        }
        break;
      }
    }
  }
  return false;
}


}