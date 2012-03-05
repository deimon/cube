#ifndef __PERSONMANIPULATOR_H__
#define __PERSONMANIPULATOR_H__
#include <osgGA/CameraManipulator>

namespace cube
{
  static int homeCursorPositionX = 0;
  static int homeCursorPositionY = 0;
  static double movementSpeed = 5;

  class PersonManipulator : public osgGA::CameraManipulator
  {
  public:
    PersonManipulator();
    void setByMatrix( const osg::Matrixd& matrix );
    void setByInverseMatrix( const osg::Matrixd& matrix );
    osg::Matrixd getMatrix() const;
    osg::Matrixd getInverseMatrix() const;
    void setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation );

    void home( double /*currentTime*/ );
    void home(const osgGA::GUIEventAdapter& ,osgGA::GUIActionAdapter&) {home(0);}

    void setNode( osg::Node* );
    const osg::Node* getNode() const;
    osg::Node* getNode();

  protected:
    void calcStep(osg::Vec3d vDir, osgGA::GUIActionAdapter& us);

    bool handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
    bool handleFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
    bool handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
    bool handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );
    bool handleMouseMove( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us );

    bool _moveBackward, _moveForward;
    bool _moveLeft, _moveRight;
    bool _jump;
    float _startJump;

    osg::Vec3d _eye;
    osg::Quat  _rotation;

    double _delta_frame_time;
    double _last_frame_time;

    osg::ref_ptr< osg::Node > _node;

    int _cubType;
  };
}

#endif