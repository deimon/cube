#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#define VISIBLE_ZONE 11

namespace cube
{
  class Areas
  {
  public:
    static int circle[11][11];

    struct v2{ int x,y; };

    static v2 yp[VISIBLE_ZONE]; //y positiv
    static v2 yn[VISIBLE_ZONE]; //y negativ
    static v2 xp[VISIBLE_ZONE]; //x positiv
    static v2 xn[VISIBLE_ZONE]; //x negativ
  };

  int Areas::circle[VISIBLE_ZONE][VISIBLE_ZONE] = { {0,0,0,0,1,1,1,0,0,0,0},
                                                    {0,0,1,1,1,1,1,1,1,0,0},
                                                    {0,1,1,1,1,1,1,1,1,1,0},
                                                    {0,1,1,1,1,1,1,1,1,1,0},
                                                    {1,1,1,1,1,1,1,1,1,1,1},
                                                    {1,1,1,1,1,1,1,1,1,1,1},
                                                    {1,1,1,1,1,1,1,1,1,1,1},
                                                    {0,1,1,1,1,1,1,1,1,1,0},
                                                    {0,1,1,1,1,1,1,1,1,1,0},
                                                    {0,0,1,1,1,1,1,1,1,0,0},
                                                    {0,0,0,0,1,1,1,0,0,0,0}};

  Areas::v2 Areas::yp[VISIBLE_ZONE] 
    = { {-5,  1}, {-4,  3}, {-3,  4}, {-2,  4}, {-1,  5}, { 0,  5}, { 1,  5}, { 2,  4}, { 3,  4}, { 4,  3}, { 5,  1}};

  Areas::v2 Areas::yn[VISIBLE_ZONE] 
    = { {-5, -1}, {-4, -3}, {-3, -4}, {-2, -4}, {-1, -5}, { 0, -5}, { 1, -5}, { 2, -4}, { 3, -4}, { 4, -3}, { 5, -1}};

  Areas::v2 Areas::xp[VISIBLE_ZONE] 
    = { { 1, -5}, { 3, -4}, { 4, -3}, { 4, -2}, { 5, -1}, { 5,  0}, { 5,  1}, { 4,  2}, { 4,  3}, { 3,  4}, { 1,  5}};

  Areas::v2 Areas::xn[VISIBLE_ZONE] 
    = { {-1, -5}, {-3, -4}, {-4, -3}, {-4, -2}, {-5, -1}, {-5,  0}, {-5,  1}, {-4,  2}, {-4,  3}, {-3,  4}, {-1,  5}};
}

#endif 