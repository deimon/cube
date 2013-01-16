#include <region.h>
#include <geoMaker.h>
#include <world.h>
#include <regionManager.h>
#include <mathUtils.h>
#include <wood.h>

using namespace cube;

int Region::countRegion = 0;

cube::Region* Region::Generation(int xreg, int yreg)
{
  int cXreg = cube::MathUtils::toCycleCoord(xreg);
  //int cYreg = cube::MathUtils::toCycleCoord(yreg);

  cube::Region* region = new Region();
  region->_xReg = cXreg;
  region->_yReg = yreg;
  region->SetPosition(xreg, yreg);

  if(abs(yreg) > World::Instance()._worldRadius)
    region->_offside = true;

  RegionManager::Instance().SetRegion(cXreg, yreg, region);

  return region;
}

void Region::CubFilling(float rnd)
{
  if(_cubFilled)
    return;

  for(int z = 0; z < GEOM_COUNT; z++)
  {
    _geom[0][z] = NULL;
    _geom[1][z] = NULL;
    _renderedCubCount[0][z] = 0;
    _renderedCubCount[1][z] = 0;
    _airCubCount[0][z] = CUBS_IN_GEOM;
    _airCubCount[1][z] = CUBS_IN_GEOM;
  }

  cube::GeoMaker::CubFilling(this, rnd);

  if(!this->IsOffside())
  {
    for(int k = 0; k < cube::MathUtils::random(0, 32); k++)
    {
      cube::Wood::Generate(RegionManager::Instance(), this, 
        cube::MathUtils::random(0, REGION_WIDTH),
        cube::MathUtils::random(0, REGION_WIDTH));
    }
  }

  _cubFilled = true;
}

void Region::LightFilling()
{
  if(_lightFilled)
    return;

  cube::GeoMaker::LightFilling(this);
  _lightFilled = true;
}

void Region::RenderFilling()
{
  if(_renderFilled)
    return;

  cube::GeoMaker::RenderFilling(this);
  _renderFilled = true;
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
  x = x % REGION_WIDTH;
  if(x < 0)
    x += REGION_WIDTH;

  y = y % REGION_WIDTH;
  if(y < 0)
    y += REGION_WIDTH;

  return CubRegion(this, _m[x][y][z], z / GEOM_SIZE);
}

//*******************************************************
//**class CubRegion

void CubRegion::SetCubType(Cub::CubeType type)
{
  if(_cub._type != type)
  {
    if(type == cube::Cub::Air)
      _region->_airCubCount[0][_geomIndex]++;
    else
      if(_cub._type == cube::Cub::Air)
        _region->_airCubCount[0][_geomIndex]--;

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

void CubRegion::SetCubBlend(bool blend)
{
  if(_cub._blend != blend)
  {
    if(blend)
      _region->_airCubCount[0][_geomIndex]++;
    else
      _region->_airCubCount[0][_geomIndex]--;

    _cub._blend = blend;
  }
}
