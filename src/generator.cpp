#include <generator.h>

using namespace cube;

void Areas::setCircle(int i, int j, int value)
{
  _circle[i + _rad][j + _rad] = value;
}

int Areas::getCircle(int i, int j)
{
  return _circle[i + _rad][j + _rad];
}

void Areas::SetRadius(int rad)
{
  _rad = rad;
  _size = _rad * 2 + 1;

  std::vector<v2> sector;

  v2 offs[2];
  offs[0] = v2(1,0);
  offs[1] = v2(1,-1);
  v2 cur(1, _rad);
  int n = 0;

  while(cur.x != cur.y)
  {
    sector.push_back(cur);
    sector.push_back(v2(cur.y, cur.x));
    cur.x += offs[n].x;
    cur.y += offs[n].y;

    n++;
    n %= 2;
  }
  sector.push_back(cur);
  
  for(int i = 0; i < _size; i++)
  for(int j = 0; j < _size; j++)
    _circle[i][j] = 0;

  setCircle( _rad,     0, 1);
  setCircle(-_rad,     0, 1);
  setCircle(    0,  _rad, 1);
  setCircle(    0, -_rad, 1);

  for(int i =0; i < sector.size(); i++)
  {
    v2 v = sector[i];
    setCircle( v.x, -v.y, 1);
    setCircle( v.x,  v.y, 1);
    setCircle(-v.x,  v.y, 1);
    setCircle(-v.x, -v.y, 1);
  }

  for(int x = -_rad; x < _rad + 1; x++)
  {
    int a, b;
    for(int y = -_rad; y < 1; y++)
      if(getCircle(x, y) == 1)
      {
        _yn.push_back(v2(x, y));
        _xn.push_back(v2(y, x));
        a = y;
        break;
      }

    for(int y = _rad; y > -1; y--)
      if(getCircle(x, y) == 1)
      {
        _yp.push_back(v2(x, y));
        _xp.push_back(v2(y, x));
        b = y;
        break;
      }

    for(int y = a + 1; y < b; y++)
      setCircle(x, y, 1);
  }
}