#ifndef __SCREAMERBLOCK_H__
#define __SCREAMERBLOCK_H__

#include <baseBlock.h>

namespace cube
{
  class ScreamerBlock: public Block
  {
  public:

    ScreamerBlock();

    void Generate(Region* reg);
    
    void Update(double updateTime, double curTime, cube::CubRegion& cubReg, osg::Vec3d wcpos, RenderGroup::DataUpdateContainer* dataUpdate);

  protected:
  };
  REGISTER_BLOCK(Block::Screamer, ScreamerBlock);
}

#endif