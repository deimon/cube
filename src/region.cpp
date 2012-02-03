#include <region.h>
#include <geoMaker.h>
#include <world.h>

using namespace cube;

void Region::Generation(cube::World* world, int xreg, int yreg, float rnd)
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
}
