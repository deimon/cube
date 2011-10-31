#ifndef __REGION_H__
#define __REGION_H__

#define REGION_SIZE 32
#define REGION_WIDTH REGION_SIZE * CUBE_SIZE
#define GEOM_DEVIDER_SIZE 8
#define GEOM_SIZE REGION_SIZE / GEOM_DEVIDER_SIZE
#define NUM_OCTAVES 4
#define PERSON_HEIGHT 1.8

#include "cube.h"
#include <osg/Vec3d>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

namespace cube
{
  class Region
  {
  public:

    static Region* Generation(osg::Vec3d& position);
    const cube::Cub& GetCub(int x, int y, int z){ return _m[x][y][z];}
    const osg::Vec3d& GetPosition(){ return _position;}

  protected:

    _inline float Noise2D(int x, int y)
    {
      int n = x + y * 57;
      n = (n<<13) ^ n;
      return ( 1.0f - ( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) /
          1073741824.0f);
    }

    _inline float Cosine_Interpolate(float x, float y, float a)
    {
      float fac1 = 3*powf(1-a, 2) - 2*powf(1-a,3);
      float fac2 = 3*powf(a, 2) - 2*powf(a, 3);
      return x*fac1 + y*fac2;
    }

    _inline float SmoothedNoise2D(float x, float y)
    {
      float corners = ( Noise2D(x-1, y-1)+Noise2D(x+1, y-1)+
           Noise2D(x-1, y+1)+Noise2D(x+1, y+1) ) / 16;
      float sides   = ( Noise2D(x-1, y)  +Noise2D(x+1, y)  +
           Noise2D(x, y-1)  +Noise2D(x, y+1) ) /  8;
      float center  =  Noise2D(x, y) / 4;
      return corners + sides + center;
    }

    float CompileNoise(float x, float y);
    int PerlinNoise_2D(float x,float y,float factor);
    void GenNoise();

    cube::Cub _m[REGION_SIZE][REGION_SIZE][REGION_SIZE];
    int _height[REGION_SIZE][REGION_SIZE];

    osg::Vec3d _position;
  };
}

#endif 