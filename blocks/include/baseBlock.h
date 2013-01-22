#ifndef __BASEBLOCK_H__
#define __BASEBLOCK_H__

#include <vector>
#include "singleton.h"

namespace cube
{
  class Region;

  class Block
  {
  public:

    enum BlockType
    {
      Air = 0,
      Ground = 1,
      Grass = 2,
      Stone = 3,
      TruncWood = 4,
      LeavesWood = 5,
      Pumpkin = 6,
      Water = 7,
      Ironstone = 8,
      ObjGrass = 9
    };

    Block(BlockType type)
      : _type(type)
    {
    }

    virtual void Generate(Region* reg) = 0;

    protected:
      BlockType _type;
  };

  class BlockProducer: public utils::Singleton<BlockProducer>
  {
  public:
    
    ~BlockProducer()
    {
      for(int i = 0; i < _bloks.size(); i++)
        delete _bloks[i];

      _bloks.clear();
    }

    void AddBlock(Block* block) { _bloks.push_back(block); }

    void Generate(Region* reg)
    {
      for(int i = 0; i < _bloks.size(); i++)
        _bloks[i]->Generate(reg);
    }

  private:
    std::vector<Block*> _bloks;
  };
}

#endif
