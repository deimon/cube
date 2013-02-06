#ifndef __WATERBLOCK_H__
#define __WATERBLOCK_H__

#include <baseBlock.h>

namespace cube
{
  class WaterBlock: public Block
  {
  public:

    WaterBlock();

    void Generate(Region* reg);
    
    void Update(double updateTime, double curTime, cube::CubRegion& cubReg, osg::Vec3d wcpos, RenderGroup::DataUpdateContainer* dataUpdate);

  protected:
  };
  REGISTER_BLOCK(Block::Water, WaterBlock);
}

#endif