#ifndef __GENERATOR_H__
#define __GENERATOR_H__

#include <vector>
#include <map>
#include "singleton.h"

namespace cube
{
  class Areas : public utils::Singleton<Areas>
  {
  public:
    struct v2{ v2() { x = y = 0; } v2(int x_, int y_) { x = x_; y = y_; } int x,y; };

    Areas() {}

    void SetRadius(int rad);

    int GetSize() { return _size; }

    std::vector<v2> _yp; //y positiv
    std::vector<v2> _yn; //y negativ
    std::vector<v2> _xp; //x positiv
    std::vector<v2> _xn; //x negativ

    std::map<int, std::map<int, int>> _circle;

  protected:
    void setCircle(int i, int j, int value);
    int getCircle(int i, int j);

    int _rad;
    int _size;
  };
/*
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
*/
}

#endif 