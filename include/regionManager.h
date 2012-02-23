#ifndef __REGION_MANAGER_H__
#define __REGION_MANAGER_H__

#include "singleton.h"
#include "region.h"

namespace cube
{
  class RegionManager: public utils::Singleton<RegionManager>
  {
  public:
    typedef std::map<int, cube::Region*> YRegionsContainer;
    typedef std::map<int, YRegionsContainer> RegionsContainer;

    struct Callback
    {
      virtual void operator()(cube::Region* reg) = 0;
    };

    RegionManager(){}
    ~RegionManager(){}

    cube::Region* GetRegion(int i, int j) { return _regions[i][j]; }
    void SetRegion(int i, int j, cube::Region* reg) { _regions[i][j] = reg; }
    cube::Region* ContainsRegion(int xreg, int yreg);
    cube::Cub* GetCub(float x, float y, float z);
    cube::Region* CreateRegion(int xreg, int yreg);

    void ForacheRegion(Callback& cb);

  protected:
    RegionsContainer _regions;
    std::map<int, std::map<int, bool>> _regionsCreated;
  };
}

#endif