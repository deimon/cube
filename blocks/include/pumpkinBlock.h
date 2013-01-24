#ifndef __PUMPKINBLOCK_H__
#define __PUMPKINBLOCK_H__

#include <baseBlock.h>

namespace cube
{
  class PumpkinBlock: public Block
  {
  public:

    PumpkinBlock();

    void Generate(Region* reg);

  protected:
  };
  REGISTER_BLOCK(Block::Pumpkin, PumpkinBlock);
}

#endif