#include <cube.h>
#include <region.h>
#include <regionManager.h>
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

const osg::Vec3d& CubInfo::GetVertex(CubInfo::CubeSide cubeSide, int numVertex)
{
  return _vertex[cubeSide][numVertex];
}

void CubInfo::FillVertCoord(CubInfo::CubeSide cubeSide, osg::Vec3Array* coords, osg::Vec3d offset, cube::CubRegion* cubReg)
{
  if(cubReg != NULL && cubReg->GetCubType() == Block::Water)
  {
    coords->push_back(offset + osg::componentMultiply(_vertex[cubeSide][0], osg::Vec3d(1.0, 1.0, cubReg->GetCubState() * 0.2 + 0.2)));
    coords->push_back(offset + osg::componentMultiply(_vertex[cubeSide][1], osg::Vec3d(1.0, 1.0, cubReg->GetCubState() * 0.2 + 0.2)));
    coords->push_back(offset + osg::componentMultiply(_vertex[cubeSide][2], osg::Vec3d(1.0, 1.0, cubReg->GetCubState() * 0.2 + 0.2)));
    coords->push_back(offset + osg::componentMultiply(_vertex[cubeSide][3], osg::Vec3d(1.0, 1.0, cubReg->GetCubState() * 0.2 + 0.2)));
  }
  else
  {
    coords->push_back(offset + _vertex[cubeSide][0]);
    coords->push_back(offset + _vertex[cubeSide][1]);
    coords->push_back(offset + _vertex[cubeSide][2]);
    coords->push_back(offset + _vertex[cubeSide][3]);
  }
}

void CubInfo::CrossFillVertCoord(osg::Vec3Array* coords, osg::Vec3d offset, int side)
{
  int ver1 = side % 2 ? 1 : 0;
  int ver2 = side % 2 ? 2 : 3;

  CubInfo::CubeSide cs1 = side < 2 ? CubInfo::X_FACE : CubInfo::X_BACK;
  CubInfo::CubeSide cs2 = side < 2 ? CubInfo::X_BACK : CubInfo::X_FACE;

  coords->push_back(offset + _vertex[cs1][ver1]);
  coords->push_back(offset + _vertex[cs2][ver1]);
  coords->push_back(offset + _vertex[cs2][ver2]);
  coords->push_back(offset + _vertex[cs1][ver2]);
}

void CubInfo::FillColorBufferLight(CubInfo::CubeSide cubeSide, osg::FloatArray* colors, osg::Vec3d pos, osg::Vec4d color)
{
  float sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  for(int n = 0; n < 4; n++)
  {
    osg::Vec3d posV = pos + _vertex[cubeSide][n] + (_normals[cubeSide] / 5.0 + osg::Vec3d(0.1f, 0.1f, 0.1f)); //osg::Vec3d(0.1f, -0.1f, 0.1f);
    for(int i = -1; i < 1; i++)
      for(int j = -1; j < 1; j++)
      {
        osg::Vec3d posC;

        if(cubeSide == CubInfo::Y_BACK || cubeSide == CubInfo::Y_FACE)
          posC = posV + osg::Vec3d(i, 0.0f, j);
        else if(cubeSide == CubInfo::X_BACK || cubeSide == CubInfo::X_FACE)
          posC = posV + osg::Vec3d(0.0f, i, j);
        else if(cubeSide == CubInfo::Z_BACK || cubeSide == CubInfo::Z_FACE)
          posC = posV + osg::Vec3d(i, j, 0.0f);

        cube::CubRegion cubReg = cube::RegionManager::Instance().GetCub(posC.x(), posC.y(), posC.z());
        float li = cubReg.GetCubLight();
        if(li > 1.0f)
          li = 1.0f;
        sum[n] += li;
      }

    sum[n] /= 4.0f;
  }

  //for(int n = 0; n < 4; n++)
  //  sum[n] += 0.05f;

  colors->push_back(sum[0]);
  colors->push_back(sum[1]);
  colors->push_back(sum[2]);
  colors->push_back(sum[3]);
}

void CubInfo::FillColorBufferLocLight(CubInfo::CubeSide cubeSide, osg::FloatArray* colors, osg::Vec3d pos, osg::Vec4d color)
{
  float sum[4] = {0.0f, 0.0f, 0.0f, 0.0f};

  for(int n = 0; n < 4; n++)
  {
    osg::Vec3d posV = pos + _vertex[cubeSide][n] + (_normals[cubeSide] / 5.0 + osg::Vec3d(0.1f, 0.1f, 0.1f)); //osg::Vec3d(0.1f, -0.1f, 0.1f);
    for(int i = -1; i < 1; i++)
      for(int j = -1; j < 1; j++)
      {
        osg::Vec3d posC;

        if(cubeSide == CubInfo::Y_BACK || cubeSide == CubInfo::Y_FACE)
          posC = posV + osg::Vec3d(i, 0.0f, j);
        else if(cubeSide == CubInfo::X_BACK || cubeSide == CubInfo::X_FACE)
          posC = posV + osg::Vec3d(0.0f, i, j);
        else if(cubeSide == CubInfo::Z_BACK || cubeSide == CubInfo::Z_FACE)
          posC = posV + osg::Vec3d(i, j, 0.0f);

        cube::CubRegion cubReg = cube::RegionManager::Instance().GetCub(posC.x(), posC.y(), posC.z());
        float li = cubReg.GetCubLocLight();
        if(li > 1.0f)
          li = 1.0f;
        sum[n] += li;
      }

      sum[n] /= 4.0f;
  }

  //for(int n = 0; n < 4; n++)
  //  sum[n] += 0.05f;

  colors->push_back(sum[0]);
  colors->push_back(sum[1]);
  colors->push_back(sum[2]);
  colors->push_back(sum[3]);
}

void CubInfo::SimpleFillColorBuffer(osg::Vec4Array* colors, osg::Vec4d color)
{
  colors->push_back(color);
  colors->push_back(color);
  colors->push_back(color);
  colors->push_back(color);
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

void TextureInfo::FillTexCoord(Block::BlockType cubeType, CubInfo::CubeSide cubeSide, osg::Vec2Array* tcoords)
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

osg::Vec4d& TextureInfo::GetSideColor(Block::BlockType cubeType, CubInfo::CubeSide cubeSide)
{
  return _csColor[cubeType][cubeSide];
}


void TextureInfo::init()
{
  for(int ct = 0; ct < CUBE_TYPE; ct++)
    for(int cs = 0; cs <= CubInfo::Z_BACK; cs++)
      _csColor[(Block::BlockType)ct][(CubInfo::CubeSide)cs] = osg::Vec4d(1.0, 1.0, 1.0, 1.0);

  _csTextures[Block::Ground][CubInfo::X_BACK] = 242;
  _csTextures[Block::Ground][CubInfo::Y_BACK] = 242;
  _csTextures[Block::Ground][CubInfo::Z_BACK] = 242;
  _csTextures[Block::Ground][CubInfo::X_FACE] = 242;
  _csTextures[Block::Ground][CubInfo::Y_FACE] = 242;
  _csTextures[Block::Ground][CubInfo::Z_FACE] = 242;

  _csTextures[Block::Grass][CubInfo::X_BACK] = 243;
  _csTextures[Block::Grass][CubInfo::Y_BACK] = 243;
  _csTextures[Block::Grass][CubInfo::Z_BACK] = 242;
  _csTextures[Block::Grass][CubInfo::X_FACE] = 243;
  _csTextures[Block::Grass][CubInfo::Y_FACE] = 243;
  _csTextures[Block::Grass][CubInfo::Z_FACE] = 240;
  _csColor[Block::Grass][CubInfo::Z_FACE] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);

  _csTextures[Block::Stone][CubInfo::X_BACK] = 241;
  _csTextures[Block::Stone][CubInfo::Y_BACK] = 241;
  _csTextures[Block::Stone][CubInfo::Z_BACK] = 241;
  _csTextures[Block::Stone][CubInfo::X_FACE] = 241;
  _csTextures[Block::Stone][CubInfo::Y_FACE] = 241;
  _csTextures[Block::Stone][CubInfo::Z_FACE] = 241;

  _csTextures[Block::TruncWood][CubInfo::X_BACK] = 228;
  _csTextures[Block::TruncWood][CubInfo::Y_BACK] = 228;
  _csTextures[Block::TruncWood][CubInfo::Z_BACK] = 229;
  _csTextures[Block::TruncWood][CubInfo::X_FACE] = 228;
  _csTextures[Block::TruncWood][CubInfo::Y_FACE] = 228;
  _csTextures[Block::TruncWood][CubInfo::Z_FACE] = 229;

  _csTextures[Block::LeavesWood][CubInfo::X_BACK] = 196;
  _csTextures[Block::LeavesWood][CubInfo::Y_BACK] = 196;
  _csTextures[Block::LeavesWood][CubInfo::Z_BACK] = 196;
  _csTextures[Block::LeavesWood][CubInfo::X_FACE] = 196;
  _csTextures[Block::LeavesWood][CubInfo::Y_FACE] = 196;
  _csTextures[Block::LeavesWood][CubInfo::Z_FACE] = 196;
  _csColor[Block::LeavesWood][CubInfo::X_BACK] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);
  _csColor[Block::LeavesWood][CubInfo::Y_BACK] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);
  _csColor[Block::LeavesWood][CubInfo::Z_BACK] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);
  _csColor[Block::LeavesWood][CubInfo::X_FACE] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);
  _csColor[Block::LeavesWood][CubInfo::Y_FACE] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);
  _csColor[Block::LeavesWood][CubInfo::Z_FACE] = osg::Vec4d(0.68, 0.94, 0.47, 1.0);

  _csTextures[Block::Pumpkin][CubInfo::X_BACK] = 134;
  _csTextures[Block::Pumpkin][CubInfo::Y_BACK] = 134;
  _csTextures[Block::Pumpkin][CubInfo::Z_BACK] = 150;
  _csTextures[Block::Pumpkin][CubInfo::X_FACE] = 136;
  _csTextures[Block::Pumpkin][CubInfo::Y_FACE] = 134;
  _csTextures[Block::Pumpkin][CubInfo::Z_FACE] = 150;

  _csTextures[Block::Water][CubInfo::X_BACK] = 61;
  _csTextures[Block::Water][CubInfo::Y_BACK] = 61;
  _csTextures[Block::Water][CubInfo::Z_BACK] = 61;
  _csTextures[Block::Water][CubInfo::X_FACE] = 61;
  _csTextures[Block::Water][CubInfo::Y_FACE] = 61;
  _csTextures[Block::Water][CubInfo::Z_FACE] = 61;
  _csColor[Block::Water][CubInfo::X_BACK] = osg::Vec4d(1.0, 1.0, 1.0, 1.5);
  _csColor[Block::Water][CubInfo::Y_BACK] = osg::Vec4d(1.0, 1.0, 1.0, 1.5);
  _csColor[Block::Water][CubInfo::Z_BACK] = osg::Vec4d(1.0, 1.0, 1.0, 1.5);
  _csColor[Block::Water][CubInfo::X_FACE] = osg::Vec4d(1.0, 1.0, 1.0, 1.5);
  _csColor[Block::Water][CubInfo::Y_FACE] = osg::Vec4d(1.0, 1.0, 1.0, 1.5);
  _csColor[Block::Water][CubInfo::Z_FACE] = osg::Vec4d(1.0, 1.0, 1.0, 1.5);

  _csTextures[Block::Ironstone][CubInfo::X_BACK] = 209;
  _csTextures[Block::Ironstone][CubInfo::Y_BACK] = 209;
  _csTextures[Block::Ironstone][CubInfo::Z_BACK] = 209;
  _csTextures[Block::Ironstone][CubInfo::X_FACE] = 209;
  _csTextures[Block::Ironstone][CubInfo::Y_FACE] = 209;
  _csTextures[Block::Ironstone][CubInfo::Z_FACE] = 209;

  _csTextures[Block::ObjGrass][CubInfo::X_BACK] = 215;
  _csColor[Block::ObjGrass][CubInfo::X_BACK] = osg::Vec4d(0.51, 0.91, 0.19, 1.0);

  _csTextures[Block::Screamer][CubInfo::X_BACK] = 151;
  _csTextures[Block::Screamer][CubInfo::Y_BACK] = 151;
  _csTextures[Block::Screamer][CubInfo::Z_BACK] = 151;
  _csTextures[Block::Screamer][CubInfo::X_FACE] = 151;
  _csTextures[Block::Screamer][CubInfo::Y_FACE] = 151;
  _csTextures[Block::Screamer][CubInfo::Z_FACE] = 151;
}