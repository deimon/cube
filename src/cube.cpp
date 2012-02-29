#include <cube.h>
#include <osgDB/ReadFile>

using namespace cube;

CubInfo::CubeSide CubInfo::FirstSide = CubInfo::Y_BACK;
CubInfo::CubeSide CubInfo::EndSide = CubInfo::Z_BACK;
CubInfo::CubeSide CubInfo::EndHorizSide = CubInfo::X_BACK;

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
  _texture->setFilter(osg::Texture::MIN_FILTER , osg::Texture::NEAREST);
  _texture->setFilter(osg::Texture::MAG_FILTER , osg::Texture::NEAREST);
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

osg::Vec4d& TextureInfo::GetSideColor(Cub::CubeType cubeType, CubInfo::CubeSide cubeSide)
{
  return _csColor[cubeType][cubeSide];
}


void TextureInfo::init()
{
  for(int ct = 0; ct < 7; ct++)
    for(int cs = 0; cs < 7; cs++)
      _csColor[(Cub::CubeType)ct][(CubInfo::CubeSide)cs] = osg::Vec4d(1.0, 1.0, 1.0, 1.0);

  _csTextures[Cub::Ground][CubInfo::X_BACK] = 242;
  _csTextures[Cub::Ground][CubInfo::Y_BACK] = 242;
  _csTextures[Cub::Ground][CubInfo::Z_BACK] = 242;
  _csTextures[Cub::Ground][CubInfo::X_FACE] = 242;
  _csTextures[Cub::Ground][CubInfo::Y_FACE] = 242;
  _csTextures[Cub::Ground][CubInfo::Z_FACE] = 242;

  _csTextures[Cub::Grass][CubInfo::X_BACK] = 243;
  _csTextures[Cub::Grass][CubInfo::Y_BACK] = 243;
  _csTextures[Cub::Grass][CubInfo::Z_BACK] = 242;
  _csTextures[Cub::Grass][CubInfo::X_FACE] = 243;
  _csTextures[Cub::Grass][CubInfo::Y_FACE] = 243;
  _csTextures[Cub::Grass][CubInfo::Z_FACE] = 240;
  _csColor[Cub::Grass][CubInfo::Z_FACE] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);

  _csTextures[Cub::Stone][CubInfo::X_BACK] = 241;
  _csTextures[Cub::Stone][CubInfo::Y_BACK] = 241;
  _csTextures[Cub::Stone][CubInfo::Z_BACK] = 241;
  _csTextures[Cub::Stone][CubInfo::X_FACE] = 241;
  _csTextures[Cub::Stone][CubInfo::Y_FACE] = 241;
  _csTextures[Cub::Stone][CubInfo::Z_FACE] = 241;

  _csTextures[Cub::TruncWood][CubInfo::X_BACK] = 228;
  _csTextures[Cub::TruncWood][CubInfo::Y_BACK] = 228;
  _csTextures[Cub::TruncWood][CubInfo::Z_BACK] = 229;
  _csTextures[Cub::TruncWood][CubInfo::X_FACE] = 228;
  _csTextures[Cub::TruncWood][CubInfo::Y_FACE] = 228;
  _csTextures[Cub::TruncWood][CubInfo::Z_FACE] = 229;

  _csTextures[Cub::LeavesWood][CubInfo::X_BACK] = 196;
  _csTextures[Cub::LeavesWood][CubInfo::Y_BACK] = 196;
  _csTextures[Cub::LeavesWood][CubInfo::Z_BACK] = 196;
  _csTextures[Cub::LeavesWood][CubInfo::X_FACE] = 196;
  _csTextures[Cub::LeavesWood][CubInfo::Y_FACE] = 196;
  _csTextures[Cub::LeavesWood][CubInfo::Z_FACE] = 196;
  _csColor[Cub::LeavesWood][CubInfo::X_BACK] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);
  _csColor[Cub::LeavesWood][CubInfo::Y_BACK] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);
  _csColor[Cub::LeavesWood][CubInfo::Z_BACK] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);
  _csColor[Cub::LeavesWood][CubInfo::X_FACE] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);
  _csColor[Cub::LeavesWood][CubInfo::Y_FACE] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);
  _csColor[Cub::LeavesWood][CubInfo::Z_FACE] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);

  _csTextures[Cub::Pumpkin][CubInfo::X_BACK] = 134;
  _csTextures[Cub::Pumpkin][CubInfo::Y_BACK] = 134;
  _csTextures[Cub::Pumpkin][CubInfo::Z_BACK] = 150;
  _csTextures[Cub::Pumpkin][CubInfo::X_FACE] = 136;
  _csTextures[Cub::Pumpkin][CubInfo::Y_FACE] = 134;
  _csTextures[Cub::Pumpkin][CubInfo::Z_FACE] = 150;
}