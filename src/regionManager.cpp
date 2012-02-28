#include <regionManager.h>
#include <world.h>

using namespace cube;

cube::Region* RegionManager::ContainsRegion(int xreg, int yreg)
{
  if(_regions.find(xreg) != _regions.end())
  {
    if(_regions[xreg].find(yreg) != _regions[xreg].end())
      return _regions[xreg][yreg];
  }

  return NULL;
}

cube::CubRegion RegionManager::GetCub(float x, float y, float z)
{
  cube::Region* rg = ContainsRegion(Region::ToRegionIndex(x), Region::ToRegionIndex(y));

  if(rg)
  {
    x -= rg->GetPosition().x();
    y -= rg->GetPosition().y();
    return rg->GetCub(x, y, z);
  }

  static cube::Cub nullCub;
  return CubRegion(NULL, nullCub, 0);
}

cube::Region* RegionManager::CreateRegion(int xreg, int yreg)
{
  cube::Region* region = cube::Region::Generation(xreg, yreg);
  region->CubFilling(World::Instance()._rnd);

  return region;
}

void RegionManager::ForacheRegion(Callback& cb)
{
  RegionsContainer::iterator xrg;
  YRegionsContainer::iterator yrg;

  for(xrg = _regions.begin(); xrg != _regions.end(); xrg++)
  for(yrg = xrg->second.begin(); yrg != xrg->second.end(); yrg++)
  {
    cb.operator()(yrg->second);
  }
}
