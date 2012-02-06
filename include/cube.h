#ifndef __CUBE_H__
#define __CUBE_H__

#define CUBE_SIZE 1

#include <iostream>
#include <osg/Texture2D>

namespace cube
{
  class Cub
  {
  public:

    enum CubeSide
    {
      Y_BACK = 0,
      X_FACE = 1,
      Y_FACE = 2,
      X_BACK = 3,
      Z_FACE = 4,
      Z_BACK = 5
    };

    enum CubeType
    {
      Air = 0,
      Ground = 1,
      Grass = 2
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

  //***************************************************************************
  //**** Texture Information
  //***************************************************************************
  class TextureInfo
  {
  public:
    typedef std::map<Cub::CubeSide, int> SidesTexture;
    typedef std::map<Cub::CubeType, SidesTexture> CubeTextures;

    TextureInfo(std::string path, int count);

    osg::Texture2D* GetTexture() { return _texture; }
    int GetCount() { return _count; }

    void FillTexCoord(Cub::CubeType cubeType, Cub::CubeSide cubeSide, osg::Vec2Array* tcoords);

  protected:
    void init();

    int _count;
    osg::Texture2D* _texture;

    CubeTextures _csTextures;
  };
}

#endif
