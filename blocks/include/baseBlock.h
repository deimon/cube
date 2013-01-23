#ifndef __BASEBLOCK_H__
#define __BASEBLOCK_H__

#include <map>
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
      ObjGrass = 9,
      Screamer = 10
    };

    Block(BlockType type)
      : _type(type)
    {
    }

    virtual void Generate(Region* reg) = 0;

    virtual void Update() {};

    protected:
      BlockType _type;
  };

  class BlockProducer
  {
  public:
    
    ~BlockProducer()
    {
      std::map<std::string, Block*>::iterator it = _bloks.begin();
      while(_bloks.begin() != _bloks.end())
      {
        delete it->second;
        _bloks.erase(it);

        it = _bloks.begin();
      }
    }

    void AddBlock(const std::string& name, Block* block) { if(_bloks.count(name) == 0) _bloks[name] = block; }

    void Generate(Region* reg)
    {
      std::map<std::string, Block*>::iterator it = _bloks.begin();
      for(; it != _bloks.end(); it++)
        it->second->Generate(reg);
    }

  private:
    std::map<std::string, Block*> _bloks;
  };

  static BlockProducer BLOCKPRODUCER;

  class RegisterBlock
  {
  public: 
    RegisterBlock(const std::string& name, Block* block)
    {
      BLOCKPRODUCER.AddBlock(name, block);
    }
  };

  #define REGISTER_BLOCK(TYPE) static RegisterBlock rb##TYPE(#TYPE, new TYPE);
}

#endif
