#ifndef __IRONSTONEBLOCK_H__
#define __IRONSTONEBLOCK_H__

#include <baseBlock.h>

namespace cube
{
  class IronstoneBlock: public Block
  {
  public:

    IronstoneBlock();

    void Generate(Region* reg);

  protected:
  };
}

#endif