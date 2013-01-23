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

  protected:
  };
  REGISTER_BLOCK(ScreamerBlock);
}

#endif