#ifndef __REGION_H__
#define __REGION_H__

#define REGION_SIZE 64
#define REGION_WIDTH REGION_SIZE * CUBE_SIZE
#define GEOM_DEVIDER_SIZE 16
#define GEOM_SIZE REGION_SIZE / GEOM_DEVIDER_SIZE

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

    static Region* Generation(osg::Vec3d& position)
    {
      cube::Region* region = new Region();
      region->_position = position;

      for(int x = 0; x < REGION_SIZE; x++)
      for(int y = 0; y < REGION_SIZE; y++)
        region->_height[x][y] = 5;

      //for(int x = 0; x < REGION_SIZE; x++)
      //  for(int y = 0; y < REGION_SIZE; y++)
      //    for(int z = 0; z < REGION_SIZE; z++)
      //    {
      //      cube::Cub* cub = &(region->_m[x][y][z]);

      //      if(z > REGION_SIZE / 2)
      //        cub->_type = cube::Cub::Air;
      //      else
      //        cub->_type = cube::Cub::Ground;
      //    }

      for(int i = 0; i < REGION_SIZE / 8; i++)
      {
        int height = REGION_SIZE * rand() / RAND_MAX;
        int x = REGION_SIZE * rand() / RAND_MAX;
        int y = REGION_SIZE * rand() / RAND_MAX;

        cube::Cub* cub = &(region->_m[x][y][height]);
        cub->_type = cube::Cub::Ground;

        region->_height[x][y] = height;
      }

      for(int i = 0; i < 8; i++)
      for(int x = 1; x < REGION_SIZE-1; x++)
      for(int y = 1; y < REGION_SIZE-1; y++)
      {
        int newHeight = (region->_height[x-1][y-1] + region->_height[x+1][y+1] + region->_height[x][y] +
                          region->_height[x-1][y+1] + region->_height[x+1][y-1] + region->_height[x][y-1] +
                          region->_height[x  ][y+1] + region->_height[x-1][y  ] + region->_height[x+1][y]) / 8;

        if(newHeight > region->_height[x][y] && newHeight < REGION_SIZE)
          region->_height[x][y] = newHeight;

        if(newHeight < REGION_SIZE)
        {
          cube::Cub* cub = &(region->_m[x][y][newHeight]);
          cub->_type = cube::Cub::Ground;
        }
      }

      return region;
    }

    const cube::Cub& GetCub(int x, int y, int z)
    {
      return _m[x][y][z];
    }

    const osg::Vec3d& GetPosition()
    {
      return _position;
    }

  protected:
    cube::Cub _m[REGION_SIZE][REGION_SIZE][REGION_SIZE];
    int _height[REGION_SIZE][REGION_SIZE];

    osg::Vec3d _position;
  };
}

#endif 