#include <region.h>
#include <geoMaker.h>
#include <world.h>
#include <regionManager.h>

using namespace cube;

int Region::countRegion = 0;

cube::Region* Region::Generation(int xreg, int yreg)
{
  cube::Region* region = new Region();
  region->_xReg = xreg;
  region->_yReg = yreg;
  region->_position.set(REGION_WIDTH * xreg, REGION_WIDTH * yreg, 0.0);

  RegionManager::Instance().SetRegion(xreg, yreg, region);

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
    _airCubCount[0][z] = CUBS_IN_GEOM;
    _airCubCount[1][z] = CUBS_IN_GEOM;
  }

  cube::GeoMaker::FillRegion(this, rnd);

  _areaGenerated = true;
}

void Region::FillRegion2()
{
  cube::GeoMaker::FillRegion2(this);
  _areaGenerated2 = true;
}

int Region::ToRegionIndex(float worldPos)
{
  if(worldPos > 0)
    return (int)worldPos / REGION_WIDTH;
  else
    return (int)worldPos / REGION_WIDTH - 1;
}

cube::CubRegion Region::GetCub(int x, int y, int z)
{
  return CubRegion(this, _m[x][y][z], z / GEOM_SIZE);
}

//*******************************************************
//**class CubRegion

void CubRegion::SetCubType(Cub::CubeType type)
{
  if(_cub._type != type)
  {
    if(type == cube::Cub::Air)
      _region->_airCubCount[_cub._blend?1:0][_geomIndex]++;
    else
      if(_cub._type == cube::Cub::Air)
        _region->_airCubCount[_cub._blend?1:0][_geomIndex]--;

    _cub._type = type;
  }
}

void CubRegion::SetCubRendered(bool rendered)
{
  _cub._rendered = rendered;

  if(rendered)
    _region->_renderedCubCount[_cub._blend?1:0][_geomIndex]++;
  else
    _region->_renderedCubCount[_cub._blend?1:0][_geomIndex]--;
}
