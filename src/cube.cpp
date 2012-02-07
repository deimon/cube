#include <cube.h>
#include <osgDB/ReadFile>

using namespace cube;

CubInfo::CubeSide CubInfo::FirstSide = CubInfo::Y_BACK;
CubInfo::CubeSide CubInfo::EndSide = CubInfo::Z_BACK;

CubInfo::CubInfo()
{
  //Init Normals
  _normals[CubInfo::X_BACK] = osg::Vec3d(-1.0, 0.0, 0.0);
  _normals[CubInfo::Y_BACK] = osg::Vec3d( 0.0,-1.0, 0.0);
  _normals[CubInfo::Z_BACK] = osg::Vec3d( 0.0, 0.0,-1.0);
  _normals[CubInfo::X_FACE] = osg::Vec3d( 1.0, 0.0, 0.0);
  _normals[CubInfo::Y_FACE] = osg::Vec3d( 0.0, 1.0, 0.0);
  _normals[CubInfo::Z_FACE] = osg::Vec3d( 0.0, 0.0, 1.0);

  //Init Vertex
  _vertex[CubInfo::Y_BACK][0] = osg::Vec3d(0.0, 0.0, 0.0);
  _vertex[CubInfo::Y_BACK][1] = osg::Vec3d(1.0, 0.0, 0.0);
  _vertex[CubInfo::Y_BACK][2] = osg::Vec3d(1.0, 0.0, 1.0);
  _vertex[CubInfo::Y_BACK][3] = osg::Vec3d(0.0, 0.0, 1.0);

  _vertex[CubInfo::X_FACE][0] = osg::Vec3d(1.0, 0.0, 0.0);
  _vertex[CubInfo::X_FACE][1] = osg::Vec3d(1.0, 1.0, 0.0);
  _vertex[CubInfo::X_FACE][2] = osg::Vec3d(1.0, 1.0, 1.0);
  _vertex[CubInfo::X_FACE][3] = osg::Vec3d(1.0, 0.0, 1.0);

  _vertex[CubInfo::Y_FACE][0] = osg::Vec3d(1.0, 1.0, 0.0);
  _vertex[CubInfo::Y_FACE][1] = osg::Vec3d(0.0, 1.0, 0.0);
  _vertex[CubInfo::Y_FACE][2] = osg::Vec3d(0.0, 1.0, 1.0);
  _vertex[CubInfo::Y_FACE][3] = osg::Vec3d(1.0, 1.0, 1.0);

  _vertex[CubInfo::X_BACK][0] = osg::Vec3d(0.0, 1.0, 0.0);
  _vertex[CubInfo::X_BACK][1] = osg::Vec3d(0.0, 0.0, 0.0);
  _vertex[CubInfo::X_BACK][2] = osg::Vec3d(0.0, 0.0, 1.0);
  _vertex[CubInfo::X_BACK][3] = osg::Vec3d(0.0, 1.0, 1.0);

  _vertex[CubInfo::Z_FACE][0] = osg::Vec3d(0.0, 0.0, 1.0);
  _vertex[CubInfo::Z_FACE][1] = osg::Vec3d(1.0, 0.0, 1.0);
  _vertex[CubInfo::Z_FACE][2] = osg::Vec3d(1.0, 1.0, 1.0);
  _vertex[CubInfo::Z_FACE][3] = osg::Vec3d(0.0, 1.0, 1.0);

  _vertex[CubInfo::Z_BACK][0] = osg::Vec3d(0.0, 1.0, 0.0);
  _vertex[CubInfo::Z_BACK][1] = osg::Vec3d(1.0, 1.0, 0.0);
  _vertex[CubInfo::Z_BACK][2] = osg::Vec3d(1.0, 0.0, 0.0);
  _vertex[CubInfo::Z_BACK][3] = osg::Vec3d(0.0, 0.0, 0.0);
}

const osg::Vec3& CubInfo::GetNormal(CubInfo::CubeSide cubeSide)
{
  return _normals[cubeSide];
}

const osg::Vec3& CubInfo::GetVertex(CubInfo::CubeSide cubeSide, int numVertex)
{
  return _vertex[cubeSide][numVertex];
}

void CubInfo::FillVertCoord(CubInfo::CubeSide cubeSide, osg::Vec3Array* coords, osg::Vec3d offset)
{
  coords->push_back(offset + _vertex[cubeSide][0]);
  coords->push_back(offset + _vertex[cubeSide][1]);
  coords->push_back(offset + _vertex[cubeSide][2]);
  coords->push_back(offset + _vertex[cubeSide][3]);
}


TextureInfo::TextureInfo(std::string path, int count)
{
  init();
  _count = count;

  _texture = new osg::Texture2D;
  osg::Image* img = osgDB::readImageFile(path);
  _texture->setImage(img);
}

void TextureInfo::FillTexCoord(Cub::CubeType cubeType, CubInfo::CubeSide cubeSide, osg::Vec2Array* tcoords)
{
  int num = _csTextures[cubeType][cubeSide];
  int yk = num / _count;
  int xk = num % _count;

  float step = 1.0 / (float)_count;

  tcoords->push_back(osg::Vec2d(step * xk    , step * yk    ));
  tcoords->push_back(osg::Vec2d(step * (xk+1), step * yk    ));
  tcoords->push_back(osg::Vec2d(step * (xk+1), step * (yk+1)));
  tcoords->push_back(osg::Vec2d(step * xk    , step * (yk+1)));
}

void TextureInfo::init()
{
  _csTextures[Cub::Ground][CubInfo::X_BACK] = 14;
  _csTextures[Cub::Ground][CubInfo::Y_BACK] = 14;
  _csTextures[Cub::Ground][CubInfo::Z_BACK] = 14;
  _csTextures[Cub::Ground][CubInfo::X_FACE] = 14;
  _csTextures[Cub::Ground][CubInfo::Y_FACE] = 14;
  _csTextures[Cub::Ground][CubInfo::Z_FACE] = 14;

  _csTextures[Cub::Grass][CubInfo::X_BACK] = 15;
  _csTextures[Cub::Grass][CubInfo::Y_BACK] = 15;
  _csTextures[Cub::Grass][CubInfo::Z_BACK] = 14;
  _csTextures[Cub::Grass][CubInfo::X_FACE] = 15;
  _csTextures[Cub::Grass][CubInfo::Y_FACE] = 15;
  _csTextures[Cub::Grass][CubInfo::Z_FACE] = 12;

  _csTextures[Cub::Stone][CubInfo::X_BACK] = 13;
  _csTextures[Cub::Stone][CubInfo::Y_BACK] = 13;
  _csTextures[Cub::Stone][CubInfo::Z_BACK] = 13;
  _csTextures[Cub::Stone][CubInfo::X_FACE] = 13;
  _csTextures[Cub::Stone][CubInfo::Y_FACE] = 13;
  _csTextures[Cub::Stone][CubInfo::Z_FACE] = 11;
}