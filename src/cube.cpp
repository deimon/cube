#include <cube.h>
#include <osgDB/ReadFile>

using namespace cube;

TextureInfo::TextureInfo(std::string path, int count)
{
  init();
  _count = count;

  _texture = new osg::Texture2D;
  osg::Image* img = osgDB::readImageFile(path);
  _texture->setImage(img);
}

void TextureInfo::FillTexCoord(Cub::CubeType cubeType, Cub::CubeSide cubeSide, osg::Vec2Array* tcoords)
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
  _csTextures[Cub::Ground][Cub::X_BACK] = 14;
  _csTextures[Cub::Ground][Cub::Y_BACK] = 14;
  _csTextures[Cub::Ground][Cub::Z_BACK] = 14;
  _csTextures[Cub::Ground][Cub::X_FACE] = 14;
  _csTextures[Cub::Ground][Cub::Y_FACE] = 14;
  _csTextures[Cub::Ground][Cub::Z_FACE] = 14;

  _csTextures[Cub::Grass][Cub::X_BACK] = 15;
  _csTextures[Cub::Grass][Cub::Y_BACK] = 15;
  _csTextures[Cub::Grass][Cub::Z_BACK] = 14;
  _csTextures[Cub::Grass][Cub::X_FACE] = 15;
  _csTextures[Cub::Grass][Cub::Y_FACE] = 15;
  _csTextures[Cub::Grass][Cub::Z_FACE] = 12;
}