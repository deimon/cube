#include <world.h>
#include <generator.h>

using namespace cube;

void WorldCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
  _world->update();
  traverse(node, nv);
}


World::World()
{
  // эти параметры надо будет устанавливать
  // в соответствии с местом появления персонажа
  _prevRegX = 0;
  _prevRegY = 0;
  _you.set(5.0, 5.0, 0.0);

  srand(time(NULL));
  _rnd = osg::PI*2*10 + ((float)rand() / RAND_MAX)* (osg::PI*3*10 - osg::PI*2*10);

  int radius = 5;

  cube::Areas::Instance().SetRadius(radius);

  for(int i = 0; i < Areas::Instance().GetSize(); i++)
  for(int j = 0; j < Areas::Instance().GetSize(); j++)
    if(Areas::Instance()._circle[i][j] == 1)
      cube::Region::Generation(this, i - radius, j - radius, _rnd);

  _sides.push_back(osg::Vec3d( 1.0f,  0.0f,  0.0f));
  _sides.push_back(osg::Vec3d(-1.0f,  0.0f,  0.0f));
  _sides.push_back(osg::Vec3d( 0.0f,  1.0f,  0.0f));
  _sides.push_back(osg::Vec3d( 0.0f, -1.0f,  0.0f));
  _sides.push_back(osg::Vec3d( 0.0f,  0.0f,  1.0f));
  _sides.push_back(osg::Vec3d( 0.0f,  0.0f, -1.0f));
}

osg::Group* World::GetGeometry()
{
  _group = new osg::Group;

  _group->setUpdateCallback(new WorldCallback(this));
  _group->removeChildren(0, _group->getNumChildren());

  createGeometry();
  _group->addChild(_geode);

  _geode->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);

  return _group;
}

void World::clearRegionGeoms(cube::Region* rg)
{
  if(rg != NULL)
  {
    for(int i = 0; i < GEOM_COUNT; i++)
    {
      _geode->removeDrawable(rg->GetGeometry(i));
      rg->SetGeometry(i, NULL);
    }
  }
}

void World::updateGeom(osg::Geometry* geom, cube::Region* reg, int zOffset)
{
  osg::Vec3Array* coords;
  osg::Vec4Array* colours;
  osg::Vec3Array* normals;
  osg::Vec2Array* tcoords;

  osg::DrawArrays* drawArr;

  coords = dynamic_cast<osg::Vec3Array*>(geom->getVertexArray());
  colours = dynamic_cast<osg::Vec4Array*>(geom->getColorArray());
  normals = dynamic_cast<osg::Vec3Array*>(geom->getNormalArray());
  drawArr = dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0));
  tcoords = dynamic_cast<osg::Vec2Array*>(geom->getTexCoordArray(0));

  coords->clear();
  colours->clear();
  normals->clear();
  tcoords->clear();

  for(int x = 0; x < GEOM_SIZE; x++)
  for(int y = 0; y < GEOM_SIZE; y++)
  for(int z = 0; z < GEOM_SIZE; z++)
  {
    const cube::Cub &cub = reg->GetCub(x, y, z + zOffset);

    if(cub._type == cube::Cub::Air || !cub._rendered) //!!!!!!!
      continue;

    osg::Vec3d pos = reg->GetPosition() + osg::Vec3d( x, y, z + zOffset);

    osg::Vec4d color = osg::Vec4d(1.0, 1.0, 1.0, 1.0);

    {//0
      coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));
      coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
      coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));
      coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));

      _texInfo->FillTexCoord(cub._type, Cub::Y_BACK, tcoords);

      colours->push_back(color);
      normals->push_back(osg::Vec3d(0.0, -1.0, 0.0));
    }

    {//1
      coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
      coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
      coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));
      coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));

      _texInfo->FillTexCoord(cub._type, Cub::X_FACE, tcoords);

      colours->push_back(color);
      normals->push_back(osg::Vec3d(1.0, 0.0, 0.0));
    }

    {//2
      coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
      coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
      coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));
      coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));

      _texInfo->FillTexCoord(cub._type, Cub::Y_FACE, tcoords);

      colours->push_back(color);
      normals->push_back(osg::Vec3d(0.0, 1.0, 0.0));
    }

    {//3
      coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
      coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));
      coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));
      coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));

      _texInfo->FillTexCoord(cub._type, Cub::X_BACK, tcoords);

      colours->push_back(color);
      normals->push_back(osg::Vec3d(-1.0, 0.0, 0.0));
    }

    {//4
      coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));
      coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));
      coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));
      coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));

      _texInfo->FillTexCoord(cub._type, Cub::Z_FACE, tcoords);

      colours->push_back(color);
      normals->push_back(osg::Vec3d(0.0, 0.0, 1.0));
    }

    {//5
      coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
      coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
      coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
      coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));

      _texInfo->FillTexCoord(cub._type, Cub::Z_BACK, tcoords);

      colours->push_back(color);
      normals->push_back(osg::Vec3d(0.0, 0.0, -1.0));
    }

    drawArr->setCount(coords->size());
  }

  geom->dirtyDisplayList();
  geom->dirtyBound();
}

void World::update()
{
  for(int i = 0; i < _dataUpdate.size(); i++)
  {
    if(_dataUpdate[i]._geom)
      updateGeom(_dataUpdate[i]._geom, _dataUpdate[i]._reg, _dataUpdate[i]._zCubOff);
  }
  _dataUpdate.clear();

  //***************
  int curRegX = Region::ToRegionIndex(_you.x());
  int curRegY = Region::ToRegionIndex(_you.y());

  if(curRegX > _prevRegX)
  {
    _prevRegX = curRegX;

    for(int i = 0; i < Areas::Instance().GetSize(); i++)
    {
      //add
      Areas::v2 offs = Areas::Instance()._xp[i];

      cube::Region* reg = ContainsReion(curRegX + offs.x, curRegY + offs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(this, curRegX + offs.x, curRegY + offs.y, _rnd);
      _addRegions[reg->GetId()] = reg;

      //del
      Areas::v2 delOff = Areas::Instance()._xn[i];
      reg = ContainsReion(curRegX + delOff.x - 1, curRegY + delOff.y);
      if(reg != NULL)
        _delRegions[reg->GetId()] = reg;
    }
  }

  if(curRegX < _prevRegX)
  {
    _prevRegX = curRegX;

    for(int i = 0; i < Areas::Instance().GetSize(); i++)
    {
      //add
      Areas::v2 offs = Areas::Instance()._xn[i];

      cube::Region* reg = ContainsReion(curRegX + offs.x, curRegY + offs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(this, curRegX + offs.x, curRegY + offs.y, _rnd);
      _addRegions[reg->GetId()] = reg;

      //del
      Areas::v2 delOff = Areas::Instance()._xp[i];
      reg = ContainsReion(curRegX + delOff.x + 1, curRegY + delOff.y);
      if(reg != NULL)
        _delRegions[reg->GetId()] = reg;
    }
  }

  if(curRegY > _prevRegY)
  {
    _prevRegY = curRegY;

    for(int i = 0; i < Areas::Instance().GetSize(); i++)
    {
      //add
      Areas::v2 offs = Areas::Instance()._yp[i];

      cube::Region* reg = ContainsReion(curRegX + offs.x, curRegY + offs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(this, curRegX + offs.x, curRegY + offs.y, _rnd);
      _addRegions[reg->GetId()] = reg;

      //del
      Areas::v2 delOff = Areas::Instance()._yn[i];
      reg = ContainsReion(curRegX + delOff.x, curRegY + delOff.y - 1);
      if(reg != NULL)
        _delRegions[reg->GetId()] = reg;
    }
  }

  if(curRegY < _prevRegY)
  {
    _prevRegY = curRegY;

    for(int i = 0; i < Areas::Instance().GetSize(); i++)
    {
      //add
      Areas::v2 offs = Areas::Instance()._yn[i];

      cube::Region* reg = ContainsReion(curRegX + offs.x, curRegY + offs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(this, curRegX + offs.x, curRegY + offs.y, _rnd);
      _addRegions[reg->GetId()] = reg;

      //del
      Areas::v2 delOff = Areas::Instance()._yp[i];
      reg = ContainsReion(curRegX + delOff.x, curRegY + delOff.y + 1);
      if(reg != NULL)
        _delRegions[reg->GetId()] = reg;
    }
  }

  std::map<long, cube::Region*>::iterator i = _addRegions.begin();
  for(; i != _addRegions.end(); i++)
  {
    UpdateRegionGeoms(i->second);
  }
  _addRegions.clear();

  i = _delRegions.begin();
  for(; i != _delRegions.end(); i++)
  {
    clearRegionGeoms(i->second);
  }
  _delRegions.clear();
}

cube::Region* World::ContainsReion(int xreg, int yreg)
{
  if(_regions.count(xreg))
  {
    if(_regions[xreg].count(yreg))
      return _regions[xreg][yreg];
  }

  return NULL;
}

const cube::Cub& World::GetCub(float x, float y, float z)
{
  cube::Region* rg = GetRegion(Region::ToRegionIndex(x), Region::ToRegionIndex(y));

  if(rg)
  {
    x -= rg->GetPosition().x();
    y -= rg->GetPosition().y();
    return rg->GetCub(x, y, z);
  }

  cube::Cub cub;

  return cub;
}

osg::Geometry* NewOSGGeom()
{
  osg::Geometry* curGeom = new osg::Geometry;

  osg::Vec3Array* coords;
  osg::Vec4Array* colours;
  osg::Vec3Array* normals;
  osg::Vec2Array* tcoords;
  osg::DrawArrays* drawArr;

  coords = new osg::Vec3Array();
  colours = new osg::Vec4Array();
  normals = new osg::Vec3Array();
  drawArr = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4);
  tcoords = new osg::Vec2Array();

  curGeom->setVertexArray(coords);
  curGeom->setColorArray(colours);
  curGeom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
  curGeom->setNormalArray(normals);
  curGeom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);

  curGeom->addPrimitiveSet(drawArr);

  curGeom->setTexCoordArray(0,tcoords);

  return curGeom;
}

void World::RemoveCub(osg::Vec3d vec)
{
  cube::Region* reg = GetRegion(Region::ToRegionIndex(vec.x()), Region::ToRegionIndex(vec.y()));
  osg::Vec3d cvec = vec - reg->GetPosition();
  cube::Cub& cub = reg->GetCub(cvec.x(), cvec.y(), cvec.z());

  cub._type = cube::Cub::Air;
  cub._rendered = false;
  reg->_renderedCubCount[(int)cvec.z() / GEOM_SIZE]--;
  int renderedCubCount = reg->_renderedCubCount[(int)cvec.z() / GEOM_SIZE];

  if((int)(cvec.z()) == reg->GetHeight(cvec.x(), cvec.y()))
  {
    for(int i = cvec.z(); i >= 0; i--)
    {
      cube::Cub& bottomCub = reg->GetCub(cvec.x(), cvec.y(), i);
      if(bottomCub._type != cube::Cub::Air)
      {
        reg->SetHeight(cvec.x(), cvec.y(), i);

        if(bottomCub._type == cube::Cub::Ground)
        {
          bottomCub._type = cube::Cub::Grass;

          int bz = i / GEOM_SIZE;
          osg::Geometry* curGeom = reg->GetGeometry(bz);

          _dataUpdate.push_back(DataUpdate(curGeom, reg, bz));
        }

        break;
      }
    }
  }

  cvec /= GEOM_SIZE;
  osg::Geometry* curGeom = reg->GetGeometry(cvec.z());

  if(renderedCubCount > 0)
  {
    if(curGeom == NULL)
    {
      curGeom = NewOSGGeom();
      reg->SetGeometry(cvec.z(), curGeom);

      _geode->addDrawable(curGeom);
    }

    _dataUpdate.push_back(DataUpdate(curGeom, reg, cvec.z()));
  }
  else
  {
    _geode->removeDrawable(curGeom);
    reg->SetGeometry(cvec.z(), NULL);
  }


  for(int i = 0; i < _sides.size(); i++)
  {
    cvec = vec + _sides[i];

    reg = GetRegion(Region::ToRegionIndex(cvec.x()), Region::ToRegionIndex(cvec.y()));
    cvec -= reg->GetPosition();
    cube::Cub& scub = reg->GetCub(cvec.x(), cvec.y(), cvec.z());

    if(scub._type != cube::Cub::Air && !scub._rendered)
    {
      scub._rendered = true;
      reg->_renderedCubCount[(int)cvec.z() / GEOM_SIZE]++;

      cvec /= GEOM_SIZE;
      curGeom = reg->GetGeometry(cvec.z());

      if(curGeom == NULL)
      {
        curGeom = NewOSGGeom();
        reg->SetGeometry(cvec.z(), curGeom);

        _geode->addDrawable(curGeom);
      }

      _dataUpdate.push_back(DataUpdate(curGeom, reg, cvec.z()));
    }
  }
}

void World::UpdateRegionGeoms(cube::Region* rg)
{
  for(int offset = 0; offset < GEOM_COUNT; offset++)
  {
    if(rg->_renderedCubCount[offset] < 1)
      continue;

    osg::Geometry* curGeom = rg->GetGeometry(offset);

    if(curGeom == NULL)
    {
      curGeom = NewOSGGeom();
      rg->SetGeometry(offset, curGeom);

      _geode->addDrawable(curGeom);
    }

    if(curGeom)
      updateGeom(curGeom, rg, offset * GEOM_SIZE);
  }
}

osg::Geode* World::createGeometry()
{
  _geode = new osg::Geode;

  osg::StateSet* ss = _geode->getOrCreateStateSet();

  _texInfo = new TextureInfo("./res/mc.jpg", 4);

  ss->setTextureAttributeAndModes(0, _texInfo->GetTexture(), osg::StateAttribute::ON);

  //tex->setFilter(osg::Texture::MIN_FILTER , osg::Texture::NEAREST);
  //tex->setFilter(osg::Texture::MAG_FILTER , osg::Texture::NEAREST);

  RegionsContainer::iterator xrg;
  YRegionsContainer::iterator yrg;
  for(xrg = _regions.begin(); xrg != _regions.end(); xrg++)
    for(yrg = xrg->second.begin(); yrg != xrg->second.end(); yrg++)
  {
    UpdateRegionGeoms(yrg->second);
  }

  return _geode;
}
