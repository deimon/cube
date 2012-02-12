#include <world.h>
#include <mathUtils.h>

#include <OpenThreads/Thread>

using namespace cube;

class CreateGeomThread : public OpenThreads::Thread
{
public:

  CreateGeomThread(cube::World* world)
    : _world(world)
    , _completed(true)
  {
    if (!isRunning()) start();
  }

  virtual void quit(bool waitForThreadToExit = true)
  {
    _done = true;
    if(isRunning() && waitForThreadToExit)
      join();
  }

  bool IsCompleted() { return _completed; }
  void Calculate() { _completed = false; }

  std::list<std::pair<cube::Region*, Areas::v2>> _addRegions;

  std::list<cube::Region*> _addToSceneRegions;

protected:
  ~CreateGeomThread() { quit(); }

private:
  virtual void run()
  {
    _done = false;

    while (!_done)
    {
      microSleep(100);
      //_world->ProcessAddRegions();
      if(!_completed)
      {
        if(!_addRegions.empty())
        {
          std::list<std::pair<cube::Region*, Areas::v2>>::iterator i = _addRegions.begin();
          for(; i != _addRegions.end(); i++)
          {
            Areas::v2 regPos = i->second;
            cube::Region* reg = i->first;

            if(reg == NULL)
            {
              reg = cube::Region::Generation(regPos.x, regPos.y, _world->_rnd);

              //_world->_regions[regPos.x][regPos.y] = reg;
              //_world->_regionsCreated[regPos.x][regPos.y] = true;
            }
            _world->UpdateRegionGeoms(reg, false);

            _addToSceneRegions.push_back(reg);
          }

          _addRegions.clear();
        }

        _completed = true;
      }
    }
  }

  bool _done;

  bool _completed;

  cube::World* _world;
};

void WorldCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
  _world->update();
  traverse(node, nv);
}


World::World()
{
  _frame = 0;
  // эти параметры надо будет устанавливать
  // в соответствии с местом появления персонажа
  _prevRegX = 0;
  _prevRegY = 0;
  _you.set(5.0, 5.0, 0.0);

  srand(time(NULL));
  _rnd = osg::PI*2*10 + ((float)rand() / RAND_MAX)* (osg::PI*3*10 - osg::PI*2*10);

  int radius = 16;

  cube::Areas::Instance().SetRadius(radius);

  for(int i = 0; i < Areas::Instance().GetSize(); i++)
  for(int j = 0; j < Areas::Instance().GetSize(); j++)
    if(Areas::Instance()._circle[i][j] == 1)
    {
      cube::Region* region = cube::Region::Generation(i - radius, j - radius, _rnd);

      _regions[i - radius][j - radius] = region;
      //_regionsCreated[i - radius][j - radius] = true;
    }

  _cgThread = new CreateGeomThread(this);
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
    if(rg->_geometryCreated)
    {
      for(int i = 0; i < GEOM_COUNT; i++)
      {
        osg::Geometry* geom = rg->GetGeometry(i);
        if(geom)
        {
          _geode->removeDrawable(rg->GetGeometry(i));
          rg->SetGeometry(i, NULL);
        }
      }

      rg->_geometryCreated = false;
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

    for(int side = CubInfo::FirstSide; side <= CubInfo::EndSide; side++)
    {
      CubInfo::CubeSide cside = (CubInfo::CubeSide)side;

      CubInfo::Instance().FillVertCoord(cside, coords, pos);

      _texInfo->FillTexCoord(cub._type, cside, tcoords);

      colours->push_back(color);
      normals->push_back(CubInfo::Instance().GetNormal(cside));
    }

    drawArr->setCount(coords->size());
  }

  geom->dirtyDisplayList();
  geom->dirtyBound();
}

void World::update()
{
  if(_cgThread->IsCompleted() && !_addRegions.empty())
  {
    _cgThread->_addRegions.swap(_addRegions);

    if(_addToSceneRegions.empty())
      _cgThread->_addToSceneRegions.swap(_addToSceneRegions);
    else
    {
      std::list<cube::Region*>::iterator it = _addToSceneRegions.end();
      --it;
      _addToSceneRegions.splice(it, _cgThread->_addToSceneRegions);
    }

    _cgThread->Calculate();
  }

  for(int i = 0; i < _dataUpdate.size(); i++)
  {
    if(_dataUpdate[i]._geom)
      updateGeom(_dataUpdate[i]._geom, _dataUpdate[i]._reg, _dataUpdate[i]._zCubOff);
  }
  _dataUpdate.clear();

  //ProcessAddRegions();

  if(!_addToSceneRegions.empty() && _frame == 0)
  {
    cube::Region* reg = _addToSceneRegions.front();
    _addToSceneRegions.pop_front();

    _regions[reg->GetX()][reg->GetY()] = reg;
    //_regionsCreated[regPos.x][regPos.y] = true;

    for(int offset = 0; offset < GEOM_COUNT; offset++)
    {
      osg::Geometry* curGeom = reg->GetGeometry(offset);

      if(curGeom != NULL)
        _geode->addDrawable(curGeom);
    }

    //del
    if(!_delRegions.empty())
    {
      Areas::v2 regPos = _delRegions.front().second;
      cube::Region* reg = ContainsRegion(regPos.x, regPos.y);
      _delRegions.pop_front();

      if(reg != NULL)
        clearRegionGeoms(reg);
    }
  }

  _frame++;
  _frame %= 5;

  //***************
  int curRegX = Region::ToRegionIndex(_you.x());
  int curRegY = Region::ToRegionIndex(_you.y());

  if(curRegX > _prevRegX)
  {
    _prevRegX = curRegX;

    for(int i = 0; i < Areas::Instance().GetSize(); i++)
    {
      //add
      Areas::v2 addOffs = Areas::Instance()._xp[i];
      addOffs.x += curRegX;
      addOffs.y += curRegY;
      _addRegions.push_back(std::make_pair(ContainsRegion(addOffs.x, addOffs.y), addOffs));

      //del
      Areas::v2 delOff = Areas::Instance()._xn[i];
      delOff.x += curRegX - 1;
      delOff.y += curRegY;
      _delRegions.push_back(std::make_pair(ContainsRegion(delOff.x, delOff.y), delOff));
    }
  }

  if(curRegX < _prevRegX)
  {
    _prevRegX = curRegX;

    for(int i = 0; i < Areas::Instance().GetSize(); i++)
    {
      //add
      Areas::v2 addOffs = Areas::Instance()._xn[i];
      addOffs.x += curRegX;
      addOffs.y += curRegY;
      _addRegions.push_back(std::make_pair(ContainsRegion(addOffs.x, addOffs.y), addOffs));

      //del
      Areas::v2 delOff = Areas::Instance()._xp[i];
      delOff.x += curRegX + 1;
      delOff.y += curRegY;
      _delRegions.push_back(std::make_pair(ContainsRegion(delOff.x, delOff.y), delOff));
    }
  }

  if(curRegY > _prevRegY)
  {
    _prevRegY = curRegY;

    for(int i = 0; i < Areas::Instance().GetSize(); i++)
    {
      //add
      Areas::v2 addOffs = Areas::Instance()._yp[i];
      addOffs.x += curRegX;
      addOffs.y += curRegY;
      _addRegions.push_back(std::make_pair(ContainsRegion(addOffs.x, addOffs.y), addOffs));

      //del
      Areas::v2 delOff = Areas::Instance()._yn[i];
      delOff.x += curRegX;
      delOff.y += curRegY - 1;
      _delRegions.push_back(std::make_pair(ContainsRegion(delOff.x, delOff.y), delOff));
    }
  }

  if(curRegY < _prevRegY)
  {
    _prevRegY = curRegY;

    for(int i = 0; i < Areas::Instance().GetSize(); i++)
    {
      //add
      Areas::v2 addOffs = Areas::Instance()._yn[i];
      addOffs.x += curRegX;
      addOffs.y += curRegY;
      _addRegions.push_back(std::make_pair(ContainsRegion(addOffs.x, addOffs.y), addOffs));

      //del
      Areas::v2 delOff = Areas::Instance()._yp[i];
      delOff.x += curRegX;
      delOff.y += curRegY + 1;
      _delRegions.push_back(std::make_pair(ContainsRegion(delOff.x, delOff.y), delOff));
    }
  }
}

cube::Region* World::ContainsRegion(int xreg, int yreg)
{
  if(_regions.find(xreg) != _regions.end())
  {
    if(_regions[xreg].find(yreg) != _regions[xreg].end())
      return _regions[xreg][yreg];
  }

  return NULL;
}

bool World::ContainsRegionSafe(int xreg, int yreg)
{
  //if(_regionsCreated.find(xreg) != std::map::end())
  //{
  //  if(_regionsCreated[xreg].find(yreg) != std::map::end())
  //    return _regionsCreated[xreg][yreg];
  //}

  return false;
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
  curGeom->setUseVertexBufferObjects(true);

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


  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    cvec = vec + CubInfo::Instance().GetNormal(side);

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

void World::AddCub(osg::Vec3d vec)
{
  cube::Region* reg = GetRegion(Region::ToRegionIndex(vec.x()), Region::ToRegionIndex(vec.y()));
  osg::Vec3d cvec = vec - reg->GetPosition();

  CubInfo::CubeSide side = cube::MathUtils::CubIntersection(reg, cvec.x(), cvec.y(), cvec.z(), _you, vec);

  osg::Vec3d norm = CubInfo::Instance().GetNormal(side);
  {
    cvec = vec + norm;

    reg = GetRegion(Region::ToRegionIndex(cvec.x()), Region::ToRegionIndex(cvec.y()));
    cvec -= reg->GetPosition();
    cube::Cub& scub = reg->GetCub(cvec.x(), cvec.y(), cvec.z());

    if(scub._type == cube::Cub::Air)
    {
      scub._type = cube::Cub::Ground;
      scub._rendered = true;
      reg->_renderedCubCount[(int)cvec.z() / GEOM_SIZE]++;

      cvec /= GEOM_SIZE;
      osg::Geometry* curGeom = reg->GetGeometry(cvec.z());

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

void World::ProcessAddRegions()
{
  
}

void World::UpdateRegionGeoms(cube::Region* rg, bool addToScene)
{
  if(!rg->_geometryCreated)
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

        if(addToScene)
          _geode->addDrawable(curGeom);
      }

      if(curGeom)
        updateGeom(curGeom, rg, offset * GEOM_SIZE);
    }

    rg->_geometryCreated = true;
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
