#include <region.h>
#include <geoMaker.h>
#include <world.h>

using namespace cube;

int Region::countRegion = 0;

cube::Region* Region::Generation(cube::World* world, int xreg, int yreg)
{
  cube::Region* region = new Region();
  region->_xReg = xreg;
  region->_yReg = yreg;
  region->_position.set(REGION_WIDTH * xreg, REGION_WIDTH * yreg, 0.0);

  world->_regions[xreg][yreg] = region;

  return region;
}

void Region::FillRegion(float rnd)
{
  for(int z = 0; z < GEOM_COUNT; z++)
  {
    _geom[0][z] = NULL;
    _geom[1][z] = NULL;
    _renderedCubCount[0][z] = 0;
    _renderedCubCount[1][z] = 0;
  }

  cube::GeoMaker::FillRegion(this, rnd);

  _areaGenerated = true;
}

int Region::ToRegionIndex(float worldPos)
{
  if(worldPos > 0)
    return (int)worldPos / REGION_WIDTH;
  else
    return (int)worldPos / REGION_WIDTH - 1;
}
