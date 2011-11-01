#include <personManipulator.h>
#include <world.h>

using namespace cube;

PersonManipulator::PersonManipulator()
  :osgGA::FirstPersonManipulator()
{
  _autoComputeHomePosition = false;
  setWheelMovement( 3.0, true );

  _moveBackward = false;
  _moveForward = false;
}

bool PersonManipulator::handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  switch(ea.getUnmodifiedKey())
    {
      case osgGA::GUIEventAdapter::KEY_F:
        {
          _moveBackward = false;
          return true;
        }
        break;
      case osgGA::GUIEventAdapter::KEY_R:
        {
          _moveForward = false;
          return true;
        }
        break;

      case osgGA::GUIEventAdapter::KEY_T:
        {
          _eye += osg::Vec3d(.0, .0, 1.2);
          us.requestRedraw();
          return true;
        }
        break;
    }

  return FirstPersonManipulator::handleKeyDown(ea, us);
}

bool PersonManipulator::handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  switch(ea.getUnmodifiedKey())
    {
      case osgGA::GUIEventAdapter::KEY_F:
        {
          _moveBackward = true;
          return true;
        }
        break;
      case osgGA::GUIEventAdapter::KEY_R:
        {
          _moveForward = true;
          return true;
        }
        break;
    }

  return FirstPersonManipulator::handleKeyDown(ea, us);
}

bool PersonManipulator::handleFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  FirstPersonManipulator::handleFrame(ea, us);

  cube::World& world = cube::World::Instance();

  if(_moveForward)
  {
    osg::Vec3d vDir = _rotation * osg::Vec3d(0., 0., -_wheelMovement) * _delta_frame_time;
    vDir.z() = 0;

    osg::Vec3d newEye = _eye + vDir;
    const cube::Cub& cub = world.GetCub(newEye.x(), newEye.y(), newEye.z() - (PERSON_HEIGHT-1));

    if(cub._type == cube::Cub::Air)
      _eye = newEye;
    us.requestRedraw();
  }

  if(_moveBackward)
  {
    osg::Vec3d vDir = _rotation * osg::Vec3d(0., 0., _wheelMovement) * _delta_frame_time;
    vDir.z() = 0;

    osg::Vec3d newEye = _eye + vDir;
    const cube::Cub& cub = world.GetCub(newEye.x(), newEye.y(), newEye.z() - (PERSON_HEIGHT-1));

    if(cub._type == cube::Cub::Air)
      _eye = newEye;
    us.requestRedraw();
  }

  osg::Vec3d prevEye = _eye;

  const cube::Cub& cub = world.GetCub(prevEye.x(), prevEye.y(), prevEye.z() - PERSON_HEIGHT);

  if(cub._type == cube::Cub::Air)
  {
    prevEye += osg::Vec3d(0.0, 0.0, -9.0) * _delta_frame_time;

    const cube::Cub& newCub = world.GetCub(prevEye.x(), prevEye.y(), prevEye.z() - PERSON_HEIGHT);

    if(cub._type == cube::Cub::Air)
      setTransformation(prevEye, _rotation);
    else
    {

    }
  }
  //else if(cub._type == cube::Cub::Ground)
  //{
  //  const cube::Cub& cub2 = rg->GetCub(rPos.x(), rPos.y(), rPos.z() - 2);

  //  if(cub2._type != cube::Cub::Air)
  //  {
  //    //prevEye += osg::Vec3d(0.0, 0.0, 1.0) * _delta_frame_time;
  //    prevEye.z() = (int)(rPos.z() + 1);
  //    setTransformation(prevEye, prevRot);
  //  }
  //}

  //dx * _delta_frame_time

  return false;
}
