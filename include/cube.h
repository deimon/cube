#ifndef __CUBE_H__
#define __CUBE_H__

#define CUBE_SIZE 1

#include <iostream>
#include <osg/Texture2D>
#include <singleton.h>
#include <osg/Vec4>

namespace cube
{
  class CubRegion;

  class Cub
  {
  public:

    enum CubeType
    {
      Air = 0,
      Ground = 1,
      Grass = 2,
      Stone = 3,
      TruncWood = 4,
      LeavesWood = 5
    };
  public:

    Cub()
      : _rendered(false)
      , _type(Air)
      , _blend(false)
      , _light(1.0f)
    {
    }

  protected:
    CubeType _type;
    bool _rendered;
    bool _blend;
    float _light;

    friend CubRegion;
  };

  class CubInfo: public utils::Singleton<CubInfo>
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

    static CubeSide FirstSide;
    static CubeSide EndSide;

    CubInfo();
    const osg::Vec3& GetNormal(CubInfo::CubeSide cubeSide);
    const osg::Vec3& GetVertex(CubInfo::CubeSide cubeSide, int numVertex);

    void FillVertCoord(CubInfo::CubeSide cubeSide, osg::Vec3Array* coords, osg::Vec3d offset);

  protected:

    std::map<CubInfo::CubeSide, osg::Vec3> _normals;
    std::map<CubInfo::CubeSide, std::map<int, osg::Vec3>> _vertex;
  };

  //***************************************************************************
  //**** Texture Information
  //***************************************************************************
  class TextureInfo
  {
  public:
    typedef std::map<CubInfo::CubeSide, int> SidesTexture;
    typedef std::map<Cub::CubeType, SidesTexture> CubeTextures;

    typedef std::map<CubInfo::CubeSide, osg::Vec4d> SidesColor;
    typedef std::map<Cub::CubeType, SidesColor> CubeColors;

    TextureInfo(std::string path, int count);

    osg::Texture2D* GetTexture() { return _texture; }
    int GetCount() { return _count; }

    void FillTexCoord(Cub::CubeType cubeType, CubInfo::CubeSide cubeSide, osg::Vec2Array* tcoords);
    osg::Vec4d& GetSideColor(Cub::CubeType cubeType, CubInfo::CubeSide cubeSide);

  protected:
    void init();

    int _count;
    osg::Texture2D* _texture;

    CubeTextures _csTextures;
    CubeColors _csColor;
  };
}

#endif
