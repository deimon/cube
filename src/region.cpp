#include <region.h>
#include <geoMaker.h>

using namespace cube;

cube::Region* Region::Generation(osg::Vec3d& position)
{
  cube::Region* region = new Region();
  region->_position = position;

  cube::GeoMaker::FillRegion(region);
  //region->GenNoise();

  for(int x = 0; x < GEOM_SIZE; x++)
  for(int y = 0; y < GEOM_SIZE; y++)
  for(int z = 0; z < GEOM_SIZE; z++)
  {
    region->_geom[x][y][z] = NULL;

  }

  return region;
}
