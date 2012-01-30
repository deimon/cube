#ifndef __CUBE_H__
#define __CUBE_H__

#define CUBE_SIZE 1

namespace cube
{
  class Cub
  {
  public:
    enum CubeType
    {
      Air = 0,
      Ground = 1
    };
  public:

    Cub()
      : _rendered(false)
      , _type(Air)
    {
    }

    CubeType _type;
    bool _rendered;
  };
}

#endif
