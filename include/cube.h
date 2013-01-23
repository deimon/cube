#ifndef __CUBE_H__
#define __CUBE_H__

#define CUBE_SIZE 1
#define CUBE_TYPE 11

#include <iostream>
#include <osg/Texture2D>
#include <singleton.h>
#include <osg/Vec4>

#include <baseBlock.h>

namespace cube
{
  class CubRegion;

  class Cub
  {
  public:

    Cub()
      : _rendered(false)
      , _type(Block::Air)
      , _blend(false)
      , _light(0.1f)
      , _locLight(0.0f)
    {
    }

  protected:
    Block::BlockType _type;
    bool _rendered;
    bool _blend;
    float _light;
    float _locLight;

    friend class CubRegion;
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
    static CubeSide EndHorizSide;

    CubInfo();
    const osg::Vec3& GetNormal(CubInfo::CubeSide cubeSide);
    const osg::Vec3& GetVertex(CubInfo::CubeSide cubeSide, int numVertex);

    void FillVertCoord(CubInfo::CubeSide cubeSide, osg::Vec3Array* coords, osg::Vec3d offset);
    void CrossFillVertCoord(osg::Vec3Array* coords, osg::Vec3d offset, int side);
    void FillColorBuffer(CubInfo::CubeSide cubeSide, osg::Vec4Array* colors, osg::Vec3d pos, osg::Vec4d color);
    void SimpleFillColorBuffer(osg::Vec4Array* colors, osg::Vec4d color);

  protected:

    std::map<CubInfo::CubeSide, osg::Vec3> _normals;
    std::map<CubInfo::CubeSide, std::map<int, osg::Vec3> > _vertex;
  };

  //***************************************************************************
  //**** Texture Information
  //***************************************************************************
  class TextureInfo
  {
  public:
    typedef std::map<CubInfo::CubeSide, int> SidesTexture;
    typedef std::map<Block::BlockType, SidesTexture> CubeTextures;

    typedef std::map<CubInfo::CubeSide, osg::Vec4d> SidesColor;
    typedef std::map<Block::BlockType, SidesColor> CubeColors;

    TextureInfo(std::string path, int count);

    osg::Texture2D* GetTexture() { return _texture; }
    int GetCount() { return _count; }

    void FillTexCoord(Block::BlockType cubeType, CubInfo::CubeSide cubeSide, osg::Vec2Array* tcoords);
    osg::Vec4d& GetSideColor(Block::BlockType cubeType, CubInfo::CubeSide cubeSide);

  protected:
    void init();

    int _count;
    osg::Texture2D* _texture;

    CubeTextures _csTextures;
    CubeColors _csColor;
  };
}

#endif
