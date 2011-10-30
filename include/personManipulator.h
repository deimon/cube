#ifndef __PERSONMANIPULATOR_H__
#define __PERSONMANIPULATOR_H__
#include <osgGA/FirstPersonManipulator>

namespace cube
{
  class PersonManipulator : public osgGA::FirstPersonManipulator
  {
  public:
    virtual bool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
  };
}

#endif