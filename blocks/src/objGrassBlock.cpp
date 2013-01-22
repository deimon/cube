#include <objGrassBlock.h>
#include <region.h>
#include <mathUtils.h>

using namespace cube;

ObjGrassBlock::ObjGrassBlock()
: Block(ObjGrass)
{
}

void ObjGrassBlock::Generate(Region* reg)
{
  for(int n = 0; n < 100; n++)
  {
    if(MathUtils::random(0, 10) < 5)
      continue;

    int i = MathUtils::random(0, REGION_WIDTH - 1);
    int j = MathUtils::random(0, REGION_WIDTH - 1);

    if(reg->GetCub(i, j, reg->GetHeight(i, j)).GetCubType() == cube::Block::Grass)
    {
      cube::CubRegion cubReg = reg->GetCub(i, j, reg->GetHeight(i, j) + 1);

      if(cubReg.GetCubType() == cube::Block::Air)
        cubReg.SetCubType(cube::Block::ObjGrass);
    }
  }
}
