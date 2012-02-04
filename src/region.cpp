#include <region.h>
#include <geoMaker.h>
#include <world.h>

using namespace cube;

int Region::countRegion = 0;

cube::Region* Region::Generation(cube::World* world, int xreg, int yreg, float rnd)
{
  cube::Region* region = new Region();
  region->_xReg = xreg;
  region->_yReg = yreg;
  region->_position.set(REGION_WIDTH * xreg, REGION_WIDTH * yreg, 0.0);

  for(int z = 0; z < GEOM_COUNT; z++)
  {
    region->_geom[z] = NULL;
    region->_renderedCubCount[z] = 0;
  }

  cube::GeoMaker::FillRegion(region, rnd);

  world->_regions[xreg][yreg] = region;

  return region;
}

int Region::ToRegionIndex(float worldPos)
{
  if(worldPos > 0)
    return (int)worldPos / REGION_WIDTH;
  else
    return (int)worldPos / REGION_WIDTH - 1;
}
