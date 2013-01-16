#include <regionManager.h>
#include <world.h>
#include <mathUtils.h>

using namespace cube;

cube::Region* RegionManager::GetRegion(int i, int j)
{
  int cXreg = cube::MathUtils::toCycleCoord(i);
  //int cYreg = cube::MathUtils::toCycleCoord(j);

  _regions[cXreg][j]->SetPosition(i, j);

  return _regions[cXreg][j];
}

cube::Region* RegionManager::ContainsRegion(int xreg, int yreg)
{
  int cXreg = cube::MathUtils::toCycleCoord(xreg);
  //int cYreg = cube::MathUtils::toCycleCoord(yreg);

  if(_regions.find(cXreg) != _regions.end())
  {
    if(_regions[cXreg].find(yreg) != _regions[cXreg].end())
    {
      _regions[cXreg][yreg]->SetPosition(xreg, yreg);

      return _regions[cXreg][yreg];
    }
  }

  return NULL;
}

cube::CubRegion RegionManager::GetCub(float x, float y, float z)
{
  int xreg = Region::ToRegionIndex(x);
  int yreg = Region::ToRegionIndex(y);

  cube::Region* rg = ContainsRegion(xreg, yreg);

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

void RegionManager::Save()
{
  RegionsContainer::iterator xrg;
  YRegionsContainer::iterator yrg;

  for(xrg = _regions.begin(); xrg != _regions.end(); xrg++)
  for(yrg = xrg->second.begin(); yrg != xrg->second.end(); yrg++)
    yrg->second->Save();
}
