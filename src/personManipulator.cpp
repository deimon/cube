#include <personManipulator.h>
#include <world.h>

using namespace cube;

void rotateYawPitch( osg::Quat& rotation, const double yaw, const double pitch,
                                           const osg::Vec3d& localUp )
{
  bool verticalAxisFixed = (localUp != osg::Vec3d( 0.,0.,0. ));

  // rotations
  osg::Quat rotateYaw( -yaw, verticalAxisFixed ? localUp : rotation * osg::Vec3d( 0.,1.,0. ) );
  osg::Quat rotatePitch;
  osg::Quat newRotation;
  osg::Vec3d cameraRight( rotation * osg::Vec3d( 1.,0.,0. ) );

  double my_dy = pitch;
  int i = 0;

  do
  {
    // rotations
    rotatePitch.makeRotate( my_dy, cameraRight );
    newRotation = rotation * rotateYaw * rotatePitch;

    // check for viewer's up vector to be more than 90 degrees from "up" axis
    osg::Vec3d newCameraUp = newRotation * osg::Vec3d( 0.,1.,0. );
    if( newCameraUp * localUp > 0. )
    {
      // apply new rotation
      rotation = newRotation;
      return;
    }

    my_dy /= 2.;
    if( ++i == 20 )
    {
      rotation = rotation * rotateYaw;
      return;
    }

  } while( true );
}

void fixVerticalAxis( osg::Quat& rotation, const osg::Vec3d& localUp, bool disallowFlipOver )
{
  // camera direction vectors
  osg::Vec3d cameraUp = rotation * osg::Vec3d( 0.,1.,0. );
  osg::Vec3d cameraRight = rotation * osg::Vec3d( 1.,0.,0. );
  osg::Vec3d cameraForward = rotation * osg::Vec3d( 0.,0.,-1. );

  // computed directions
  osg::Vec3d newCameraRight1 = cameraForward ^ localUp;
  osg::Vec3d newCameraRight2 = cameraUp ^ localUp;
  osg::Vec3d newCameraRight = (newCameraRight1.length2() > newCameraRight2.length2()) ?
			  newCameraRight1 : newCameraRight2;
  if( newCameraRight * cameraRight < 0. )
      newCameraRight = -newCameraRight;

  // vertical axis correction
  osg::Quat rotationVerticalAxisCorrection;
  rotationVerticalAxisCorrection.makeRotate( cameraRight, newCameraRight );

  // rotate camera
  rotation *= rotationVerticalAxisCorrection;

  if( disallowFlipOver )
  {
    // make viewer's up vector to be always less than 90 degrees from "up" axis
    osg::Vec3d newCameraUp = newCameraRight ^ cameraForward;
    if( newCameraUp * localUp < 0. )
	rotation = osg::Quat( osg::PI, osg::Vec3d( 0.,0.,1. ) ) * rotation;
  }
}

PersonManipulator::PersonManipulator()
  : osgGA::CameraManipulator()
{
  _autoComputeHomePosition = false;

  _moveBackward = false;
  _moveForward = false;
  _moveLeft = false;
  _moveRight = false;
  _jump = false;
}

void PersonManipulator::setByMatrix( const osg::Matrixd& matrix )
{
  // set variables
  _eye = matrix.getTrans();
  _rotation = matrix.getRotate();

  osg::CoordinateFrame coordinateFrame = getCoordinateFrame( _eye );
  osg::Vec3d localUp = getUpVector( coordinateFrame );
  fixVerticalAxis(_rotation, localUp, true );
}

void PersonManipulator::setByInverseMatrix( const osg::Matrixd& matrix )
{
   setByMatrix( osg::Matrixd::inverse( matrix ) );
}

osg::Matrixd PersonManipulator::getMatrix() const
{
   return osg::Matrixd::rotate( _rotation ) * osg::Matrixd::translate( _eye );
}

osg::Matrixd PersonManipulator::getInverseMatrix() const
{
   return osg::Matrixd::translate( -_eye ) * osg::Matrixd::rotate( _rotation.inverse() );
}

void PersonManipulator::setTransformation( const osg::Vec3d& eye, const osg::Quat& rotation )
{
  // set variables
  _eye = eye;
  _rotation = rotation;

  osg::CoordinateFrame coordinateFrame = getCoordinateFrame( _eye );
  osg::Vec3d localUp = getUpVector( coordinateFrame );
  fixVerticalAxis(_rotation, localUp, true );
}

void PersonManipulator::home( double /*currentTime*/ )
{
  osg::Matrixd m( osg::Matrixd::lookAt( _homeEye, _homeCenter, _homeUp ) );
  setTransformation(_homeEye, m.getRotate().inverse());
}

void PersonManipulator::setNode( osg::Node* node )
{
  _node = node;
}

const osg::Node* PersonManipulator::getNode() const
{
  return _node.get();
}

osg::Node* PersonManipulator::getNode()
{
  return _node.get();
}


bool PersonManipulator::handleKeyUp( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  switch(ea.getUnmodifiedKey())
    {
      case osgGA::GUIEventAdapter::KEY_S:
        {
          _moveBackward = false;
          return true;
        }
        break;
      case osgGA::GUIEventAdapter::KEY_W:
        {
          _moveForward = false;
          return true;
        }
        break;

      case osgGA::GUIEventAdapter::KEY_A:
        {
          _moveLeft = false;
          return true;
        }
        break;
      case osgGA::GUIEventAdapter::KEY_D:
        {
          _moveRight = false;
          return true;
        }
        break;

      case osgGA::GUIEventAdapter::KEY_Space:
        {
          if(!_jump)
          {
            _jump = true;
            _startJump = _eye.z();
          }
          return true;
        }
        break;
    }

  return false;
}

bool PersonManipulator::handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  switch(ea.getUnmodifiedKey())
    {
      case osgGA::GUIEventAdapter::KEY_S:
        {
          _moveBackward = true;
          return true;
        }
        break;
      case osgGA::GUIEventAdapter::KEY_W:
        {
          _moveForward = true;
          return true;
        }
        break;

      case osgGA::GUIEventAdapter::KEY_A:
        {
          _moveLeft = true;
          return true;
        }
        break;
      case osgGA::GUIEventAdapter::KEY_D:
        {
          _moveRight = true;
          return true;
        }
        break;
    }

  return false;
}

void PersonManipulator::calcStep(osg::Vec3d vDir, osgGA::GUIActionAdapter& us)
{
  float dx = vDir.x() > 0 ? 0.2 : -0.2;
  float dy = vDir.y() > 0 ? 0.2 : -0.2;
  vDir.x() += dx;
  vDir.y() += dy;

  cube::World& world = cube::World::Instance();
  osg::Vec3d newEye = _eye;

  if(world.GetCub(_eye.x() + vDir.x(), _eye.y(), _eye.z() - (PERSON_HEIGHT-1))._type == cube::Cub::Air)
    newEye.x() += vDir.x() - dx;

  if(world.GetCub(_eye.x(), _eye.y() + vDir.y(), _eye.z() - (PERSON_HEIGHT-1))._type == cube::Cub::Air)
    newEye.y() += vDir.y() - dy;

  _eye = newEye;

  world._you = _eye;

  us.requestRedraw();
}

bool PersonManipulator::handle( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  switch( ea.getEventType() )
  {
    case osgGA::GUIEventAdapter::FRAME:
      return handleFrame( ea, us );

    case osgGA::GUIEventAdapter::RESIZE:
    default:
      break;
  }

  if( ea.getHandled() )
    return false;

  switch( ea.getEventType() )
  {
    case osgGA::GUIEventAdapter::MOVE:
    case osgGA::GUIEventAdapter::DRAG:
      return handleMouseMove( ea, us );

    case osgGA::GUIEventAdapter::KEYDOWN:
      return handleKeyDown( ea, us );

    case osgGA::GUIEventAdapter::KEYUP:
      return handleKeyUp( ea, us );

    case osgGA::GUIEventAdapter::PUSH:
      {
	cube::World& world = cube::World::Instance();

	osg::Vec3d vDir = _rotation * osg::Vec3d(0., 0., -1.0);
	osg::Vec3d newEye = _eye + vDir;

	cube::Cub& cub = (cube::Cub&)world.GetCub(newEye.x(), newEye.y(), newEye.z());

	if(cub._type != cube::Cub::Air)
	{
	  world.RemoveCub(newEye);
	}
	else
	{
	  newEye = _eye + vDir * 2.0;

	  cube::Cub& cub = (cube::Cub&)world.GetCub(newEye.x(), newEye.y(), newEye.z());
	  if(cub._type != cube::Cub::Air)
	  {
	    world.RemoveCub(newEye);
	  }
	}


	return true;
      }
    case osgGA::GUIEventAdapter::RELEASE:
    case osgGA::GUIEventAdapter::SCROLL:
    default:
      return false;
  }
}

bool PersonManipulator::handleFrame( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  double current_frame_time = ea.getTime();

  _delta_frame_time = current_frame_time - _last_frame_time;
  _last_frame_time = current_frame_time;

  cube::World& world = cube::World::Instance();

  osg::Vec3d currentRotation = _rotation * osg::Vec3d(0., 0., -1);
  currentRotation.z() = 0;
  currentRotation.normalize();

  osg::Vec3d moveDirection;

  if(_moveForward)
    moveDirection += currentRotation;

  if(_moveBackward)
    moveDirection -= currentRotation;

  if(_moveLeft)
  {
    osg::Vec3d vDir = currentRotation;
    double x = vDir.x();
    vDir.x() = vDir.y();
    vDir.y() = -x;
    moveDirection -= vDir;
  }

  if(_moveRight)
  {
    osg::Vec3d vDir = currentRotation;
    double x = vDir.x();
    vDir.x() = vDir.y();
    vDir.y() = -x;
    moveDirection += vDir;
  }

  if(moveDirection.length() > 0.01)
  {
    moveDirection.normalize();
    calcStep(moveDirection * movementSpeed * _delta_frame_time, us);
  }

  if(_jump)
  {
    _eye += osg::Vec3d(0.0, 0.0, 5.0) * _delta_frame_time;

    if((_eye.z() - _startJump) > 1.2f)
      _jump = false;

    us.requestRedraw();

    osg::CoordinateFrame coordinateFrame = getCoordinateFrame( _eye );
    osg::Vec3d localUp = getUpVector( coordinateFrame );
    fixVerticalAxis(_rotation, localUp, true );
  }
  else
  {
    osg::Vec3d prevEye = _eye;
    const cube::Cub& cub = world.GetCub(prevEye.x(), prevEye.y(), prevEye.z() - PERSON_HEIGHT);

    if(cub._type == cube::Cub::Air)
    {
      prevEye += osg::Vec3d(0.0, 0.0, -5.0) * _delta_frame_time;

      const cube::Cub& newCub = world.GetCub(prevEye.x(), prevEye.y(), prevEye.z() - PERSON_HEIGHT);

      if(cub._type == cube::Cub::Air)
        setTransformation(prevEye, _rotation);
      else
      {

      }
    }
  }

  homeCursorPositionX = (ea.getXmin() + ea.getXmax()) / 2.0f;
  homeCursorPositionY = (ea.getYmin() + ea.getYmax()) / 2.0f;
  us.requestWarpPointer(homeCursorPositionX, homeCursorPositionY);

  return false;
}

bool PersonManipulator::handleMouseMove( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  float dx = ea.getX() - homeCursorPositionX;
  float dy = ea.getY() - homeCursorPositionY;

  // return if there is no movement.
  if( dx == 0. && dy == 0. )
      return false;

  // world up vector
  osg::CoordinateFrame coordinateFrame = getCoordinateFrame( _eye );
  osg::Vec3d localUp = getUpVector( coordinateFrame );

  rotateYawPitch( _rotation, ea.getXnormalized(), ea.getYnormalized(), localUp );
  fixVerticalAxis(_rotation, localUp, true);

  return true;
}
