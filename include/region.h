#ifndef __REGION_H__
#define __REGION_H__

#define REGION_SIZE 64
#define REGION_WIDTH REGION_SIZE * CUBE_SIZE
#define GEOM_DEVIDER_SIZE 8
#define GEOM_SIZE REGION_SIZE / GEOM_DEVIDER_SIZE
#define NUM_OCTAVES 8

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

      /*for(int x = 0; x < REGION_SIZE; x++)
      for(int y = 0; y < REGION_SIZE; y++)
        region->_height[x][y] = 5;*/

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

      region->GenNoise();

      /*for(int i = 0; i < REGION_SIZE / 8; i++)
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
      }*/

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

    float CompileNoise(float x, float y)
    {
      float int_X    = int(x);//цела€ часть х
      float fractional_X = x - int_X;//дробь от х
      //аналогично у
      float int_Y    = int(y);
      float fractional_Y = y - int_Y;
      //получаем 4 сглаженных значени€
       float v1 = SmoothedNoise2D(int_X,     int_Y);
       float v2 = SmoothedNoise2D(int_X + 1, int_Y);
       float v3 = SmoothedNoise2D(int_X,     int_Y + 1);
       float v4 = SmoothedNoise2D(int_X + 1, int_Y + 1);
      //интерполируем значени€ 1 и 2 пары и производим интерпол€цию между ними
      float i1 = Cosine_Interpolate(v1 , v2 , fractional_X);
      float i2 = Cosine_Interpolate(v3 , v4 , fractional_X);
    //€ использовал косинусною интерпол€цию »ћ’ќ лучше 
    //по параметрам быстрота-//качество
      return Cosine_Interpolate(i1 , i2 , fractional_Y);
    }

    int PerlinNoise_2D(float x,float y,float factor)
    {
       float total = 0;
       // это число может иметь и другие значени€ хоть cosf(sqrtf(2))*3.14f 
       // главное чтобы было красиво и результат вас устраивал
       float persistence=0.5f;

       // экспериментируйте с этими значени€ми, попробуйте ставить 
       // например sqrtf(3.14f)*0.25f или что-то пот€желее дл€ понимани€ J)
       float frequency = 0.25f;
       float amplitude=1;//амплитуда, в пр€мой зависимости от значени€ настойчивости

       // вводим фактор случайности, чтобы облака не были всегда одинаковыми
       // (ћы ведь помним что ф-ци€ шума когерентна?) 
        
       x+= (factor);
       y+= (factor);

       // NUM_OCTAVES - переменна€, котора€ обозначает число октав,
       // чем больше октав, тем лучше получаетс€ шум
       for(int i=0;i<NUM_OCTAVES;i++)
       {
           total += CompileNoise(x*frequency, y*frequency) * amplitude;
           amplitude *= persistence;
           frequency*=2;
        }
        //здесь можно перевести значени€ цвета   по какой-то формуле
        //например:
        //total=sqrt(total);
        // total=total*total;
        // total=sqrt(1.0f/float(total)); 
        //total=255-total;-и.т.д все зависит от желаемого результата
        total=fabsf(total);
        int res=int(total*255.0f);//приводим цвет к значению 0-255Е
        return res;
    }

    void GenNoise()
    {
      //¬з€то с gameDev http://www.gamedev.ru/articles/?id=30126
      //srand(time(NULL));

      // случайное число, которое призвано внести
      // случайность в нашу текстуру
      float fac =osg::PI*2*10 + ((float)rand() / RAND_MAX)* (osg::PI*3*10 - osg::PI*2*10);

      for(int i=0 ;i<REGION_SIZE;i++)
      {
        for(int j=0 ;j<REGION_SIZE;j++)
        {
           //проходим по всем элементам массива и заполн€ем их значени€ми   
           //pNoise[i*size+j]=PerlinNoise_2D(float(i),float(j),fac);

          int height = REGION_SIZE / 3 + ((PerlinNoise_2D(float(i),float(j),fac) * (REGION_SIZE / 4)) / 255);

          for(int z = 0; z <= height && height < REGION_SIZE; z++)
          {
            cube::Cub* cub = &(_m[i][j][z]);
            cub->_type = cube::Cub::Ground;
          }
        }
      }
    }


    cube::Cub _m[REGION_SIZE][REGION_SIZE][REGION_SIZE];
    int _height[REGION_SIZE][REGION_SIZE];

    osg::Vec3d _position;
  };
}

#endif 