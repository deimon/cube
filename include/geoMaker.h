#ifndef __GEOMAKER_H__
#define __GEOMAKER_H__

#include <region.h>

namespace cube
{
  class GeoMaker
  {
  public:
    static void FillRegion(cube::Region* rg, float rnd);
    static void FillRegion2(cube::Region* rg);

  protected:
    static inline float Noise2D(int x, int y)
    {
      int n = x + y * 57;
      n = (n<<13) ^ n;
      return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) /
        1073741824.0f);
    }

    static inline float Cosine_Interpolate(float x, float y, float a)
    {
      float fac1 = 3*powf(1-a, 2) - 2*powf(1-a,3);
      float fac2 = 3*powf(a, 2) - 2*powf(a, 3);
      return x*fac1 + y*fac2;
    }

    static inline float SmoothedNoise2D(float x, float y)
    {
      float corners = ( Noise2D(x-1, y-1)+Noise2D(x+1, y-1)+
        Noise2D(x-1, y+1)+Noise2D(x+1, y+1) ) / 16;
      float sides   = ( Noise2D(x-1, y)  +Noise2D(x+1, y)  +
        Noise2D(x, y-1)  +Noise2D(x, y+1) ) /  8;
      float center  =  Noise2D(x, y) / 4;
      return corners + sides + center;
    }

    static float CompileNoise(float x, float y);
    static int PerlinNoise_2D(float x,float y,float factor);
    static void GenNoise(cube::Region* rg, float rnd);
  };
}

#endif 