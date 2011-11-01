#ifndef __PERSONMANIPULATOR_H__
#define __PERSONMANIPULATOR_H__
#include <osgGA/FirstPersonManipulator>

namespace cube
{
  class PersonManipulator : public osgGA::FirstPersonManipulator
  {
  public:
    PersonManipulator();
    virtual bool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
    virtual bool handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

    protected:
      virtual bool handleFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

      bool _moveBackward, _moveForward;
      bool _moveLeft, _moveRight;
      bool _jump;
      float _startJump;
  };
}

#endif