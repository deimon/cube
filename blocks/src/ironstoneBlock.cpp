#include <ironstoneBlock.h>
#include <region.h>
#include <mathUtils.h>

using namespace cube;

IronstoneBlock::IronstoneBlock()
: Block(Ironstone)
{
}

void IronstoneBlock::Generate(Region* reg)
{
  for(int n = 0; n < 1000; n++)
  {
    int i = MathUtils::random(0, REGION_WIDTH - 1);
    int j = MathUtils::random(0, REGION_WIDTH - 1);
    int k = MathUtils::random(0, reg->GetHeight(i, j) - 2);

    cube::CubRegion cubReg = reg->GetCub(i, j, k);

    if(cubReg.GetCubType() != cube::Block::Air)
      cubReg.SetCubType(cube::Block::Ironstone);
  }
}
