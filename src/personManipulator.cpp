#include <personManipulator.h>

using namespace cube;

bool PersonManipulator::handleKeyDown( const osgGA::GUIEventAdapter& ea, osgGA::GUIActionAdapter& us )
{
  switch(ea.getUnmodifiedKey())
    {
      case osgGA::GUIEventAdapter::KEY_F:
        // move forward
        moveForward( isAnimating() ? dynamic_cast< FirstPersonAnimationData* >( _animationData.get() )->_targetRot : _rotation,
                     -_wheelMovement * (getRelativeFlag( _wheelMovementFlagIndex ) ? _modelSize : 1. ));
        us.requestRedraw();
        us.requestContinuousUpdate( isAnimating() || _thrown );
        return true;
        break;
      case osgGA::GUIEventAdapter::KEY_R:
        // move backward
        moveForward( _wheelMovement * (getRelativeFlag( _wheelMovementFlagIndex ) ? _modelSize : 1. ));
        _thrown = false;
        us.requestRedraw();
        us.requestContinuousUpdate( isAnimating() || _thrown );
        return true;
        break;
    }

  return FirstPersonManipulator::handleKeyDown(ea, us);
}