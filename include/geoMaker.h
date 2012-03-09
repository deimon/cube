#ifndef __GEOMAKER_H__
#define __GEOMAKER_H__

#include <region.h>
#include <perlin.h>

namespace cube
{
  class GeoMaker
  {
  public:
    static void CubFilling(cube::Region* rg, float rnd);
    static void LightFilling(cube::Region* rg);
    static void RenderFilling(cube::Region* rg);

  protected:
    static Perlin* _perlin3d;
    static Perlin* _perlin2d;

    static void GenNoise(cube::Region* rg, float rnd);
  };
}

#endif 