#ifndef __BASEBLOCK_H__
#define __BASEBLOCK_H__

#include <map>
#include "singleton.h"
#include "RenderGroup.h"

namespace osg
{
  class Vec3d;
}

namespace cube
{
  class Region;
  class CubRegion;

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
      ObjGrass = 9,
      Screamer = 10
    };

    Block(BlockType type)
      : _type(type)
    {
    }

    virtual void Generate(Region* reg) = 0;

    virtual void Update(double curTime, cube::CubRegion& cubReg, osg::Vec3d wcpos, RenderGroup::DataUpdateContainer* dataUpdate) {};

    protected:
      BlockType _type;
  };

  class BlockProducer
  {
  public:
    
    ~BlockProducer()
    {
      std::map<Block::BlockType, Block*>::iterator it = _bloks.begin();
      while(_bloks.begin() != _bloks.end())
      {
        delete it->second;
        _bloks.erase(it);

        it = _bloks.begin();
      }
    }

    void AddBlock(Block::BlockType bType, Block* block) { if(_bloks.count(bType) == 0) _bloks[bType] = block; }

    void Generate(Region* reg)
    {
      std::map<Block::BlockType, Block*>::iterator it = _bloks.begin();
      for(; it != _bloks.end(); it++)
        it->second->Generate(reg);
    }

    Block* GetBlock(Block::BlockType bType)
    {
      if(_bloks.count(bType) != 0)
        return _bloks[bType];
      else
        return NULL;
    }

  private:
    std::map<Block::BlockType, Block*> _bloks;
  };

  static BlockProducer BLOCKPRODUCER;

  class RegisterBlock
  {
  public: 
    RegisterBlock(Block::BlockType bType, Block* block)
    {
      BLOCKPRODUCER.AddBlock(bType, block);
    }
  };

  #define REGISTER_BLOCK(TYPE1, TYPE2) static RegisterBlock rb##TYPE2(TYPE1, new TYPE2);
}

#endif
