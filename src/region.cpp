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

  cube::GeoMaker::FillRegion(region, rnd);
  //region->GenNoise();

  for(int x = 0; x < GEOM_SIZE; x++)
  for(int y = 0; y < GEOM_SIZE; y++)
  for(int z = 0; z < GEOM_SIZE; z++)
  {
    region->_geom[x][y][z] = NULL;

  }

  world->_regions[xreg][yreg] = region;
}
