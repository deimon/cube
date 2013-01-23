#include <screamerBlock.h>
#include <region.h>
#include <mathUtils.h>

using namespace cube;

ScreamerBlock::ScreamerBlock()
: Block(Screamer)
{
}

void ScreamerBlock::Generate(Region* reg)
{
  for(int n = 0; n < 3; n++)
  {
    int i = MathUtils::random(0, REGION_WIDTH - 1);
    int j = MathUtils::random(0, REGION_WIDTH - 1);

    cube::CubRegion cubReg = reg->GetCub(i, j, reg->GetHeight(i, j) + 1);

    if(cubReg.GetCubType() == cube::Block::Air)
      cubReg.SetCubType(cube::Block::Screamer);
  }
}
