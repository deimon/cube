#ifndef __OBJGRASSBLOCK_H__
#define __OBJGRASSBLOCK_H__

#include <baseBlock.h>

namespace cube
{
  class ObjGrassBlock: public Block
  {
  public:

    ObjGrassBlock();

    void Generate(Region* reg);

  protected:
  };
}

#endif