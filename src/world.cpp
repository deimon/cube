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

  std::list<cube::Region*> _addRegions;

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
          for(int i = 0; i < 100 && !_addRegions.empty(); i++)
          {
            cube::Region* reg = _addRegions.front();
            _addRegions.pop_front();

            if(!reg->IsAreaGenerated())
            {
              reg->FillRegion(_world->_rnd);
            }
            _world->UpdateRegionGeoms(reg, false);

            _addToSceneRegions.push_back(reg);
          }
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
      cube::Region* region = cube::Region::Generation(this, i - radius, j - radius);
      region->FillRegion(_rnd);
      region->SetVisibleZone(true);
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

      osg::Vec3d sidePos = pos + CubInfo::Instance().GetNormal(cside) + osg::Vec3d(0.1, 0.1, 0.1);
      cube::Cub* sideCub = GetCub(sidePos.x(), sidePos.y(), sidePos.z());

      if(sideCub == NULL || sideCub->_type == cube::Cub::Air)
      {
        CubInfo::Instance().FillVertCoord(cside, coords, pos);

        _texInfo->FillTexCoord(cub._type, cside, tcoords);

        colours->push_back(color);
        normals->push_back(CubInfo::Instance().GetNormal(cside));
      }
    }

    drawArr->setCount(coords->size());
  }

  geom->dirtyDisplayList();
  geom->dirtyBound();
}

void World::update()
{
  if(_cgThread->IsCompleted() /*&& !_addRegions.empty()*/)
  {
    if(_cgThread->_addRegions.empty())
      _cgThread->_addRegions.swap(_addRegions);
    else
    {
      std::list<cube::Region*>::iterator it = _cgThread->_addRegions.end();
      --it;
      _cgThread->_addRegions.splice(it, _addRegions);
    }

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
    bool add = false;
    do 
    {
      cube::Region* reg = _addToSceneRegions.front();
      _addToSceneRegions.pop_front();

      if(reg->InVisibleZone() && !reg->InScene())
      {
        add = true;
        reg->SetInScene(true);
        //_regions[reg->GetX()][reg->GetY()] = reg;
        //_regionsCreated[regPos.x][regPos.y] = true;

        for(int offset = 0; offset < GEOM_COUNT; offset++)
        {
          osg::Geometry* curGeom = reg->GetGeometry(offset);

          if(curGeom != NULL)
            _geode->addDrawable(curGeom);
        }
      }
    } while(!add && !_addToSceneRegions.empty());

    //del
    if(!_delRegions.empty())
    {
      bool del = false;
      do 
      {
        Areas::v2 regPos = _delRegions.front().second;
        cube::Region* reg = ContainsRegion(regPos.x, regPos.y);
        _delRegions.pop_front();

        if(reg != NULL && !reg->InVisibleZone() && reg->InScene())
        {
          del = true;
          reg->SetInScene(false);
          clearRegionGeoms(reg);
        }
      } while(!del && !_delRegions.empty());
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
      cube::Region* reg = ContainsRegion(addOffs.x, addOffs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(this, addOffs.x, addOffs.y);
      reg->SetVisibleZone(true);
      _addRegions.push_back(reg);

      //del
      Areas::v2 delOff = Areas::Instance()._xn[i];
      delOff.x += curRegX - 1;
      delOff.y += curRegY;
      reg = ContainsRegion(delOff.x, delOff.y);
      reg->SetVisibleZone(false);
      _delRegions.push_back(std::make_pair(reg, delOff));
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
      cube::Region* reg = ContainsRegion(addOffs.x, addOffs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(this, addOffs.x, addOffs.y);
      reg->SetVisibleZone(true);
      _addRegions.push_back(reg);

      //del
      Areas::v2 delOff = Areas::Instance()._xp[i];
      delOff.x += curRegX + 1;
      delOff.y += curRegY;
      reg = ContainsRegion(delOff.x, delOff.y);
      reg->SetVisibleZone(false);
      _delRegions.push_back(std::make_pair(reg, delOff));
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
      cube::Region* reg = ContainsRegion(addOffs.x, addOffs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(this, addOffs.x, addOffs.y);
      reg->SetVisibleZone(true);
      _addRegions.push_back(reg);

      //del
      Areas::v2 delOff = Areas::Instance()._yn[i];
      delOff.x += curRegX;
      delOff.y += curRegY - 1;
      reg = ContainsRegion(delOff.x, delOff.y);
      reg->SetVisibleZone(false);
      _delRegions.push_back(std::make_pair(reg, delOff));
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
      cube::Region* reg = ContainsRegion(addOffs.x, addOffs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(this, addOffs.x, addOffs.y);
      reg->SetVisibleZone(true);
      _addRegions.push_back(reg);

      //del
      Areas::v2 delOff = Areas::Instance()._yp[i];
      delOff.x += curRegX;
      delOff.y += curRegY + 1;
      reg = ContainsRegion(delOff.x, delOff.y);
      reg->SetVisibleZone(false);
      _delRegions.push_back(std::make_pair(reg, delOff));
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

cube::Cub* World::GetCub(float x, float y, float z)
{
  cube::Region* rg = ContainsRegion(Region::ToRegionIndex(x), Region::ToRegionIndex(y));

  if(rg)
  {
    x -= rg->GetPosition().x();
    y -= rg->GetPosition().y();
    return &(rg->GetCub(x, y, z));
  }

  return NULL;
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

  int geomIndex = cvec.z() / GEOM_SIZE;

  cub._type = cube::Cub::Air;
  cub._rendered = false;
  reg->_renderedCubCount[geomIndex]--;
  int renderedCubCount = reg->_renderedCubCount[geomIndex];

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

  osg::Geometry* curGeom = reg->GetGeometry(geomIndex);

  if(renderedCubCount > 0)
  {
    if(curGeom == NULL)
    {
      curGeom = NewOSGGeom();
      reg->SetGeometry(geomIndex, curGeom);

      _geode->addDrawable(curGeom);
    }

    _dataUpdate.push_back(DataUpdate(curGeom, reg, geomIndex));
  }
  else
  {
    _geode->removeDrawable(curGeom);
    reg->SetGeometry(geomIndex, NULL);
  }


  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    cvec = vec + CubInfo::Instance().GetNormal(side);

    cube::Region* sideReg = GetRegion(Region::ToRegionIndex(cvec.x()), Region::ToRegionIndex(cvec.y()));
    cvec -= sideReg->GetPosition();
    cube::Cub& scub = sideReg->GetCub(cvec.x(), cvec.y(), cvec.z());

    int geomSideIndex = cvec.z() / GEOM_SIZE;

    if(scub._type != cube::Cub::Air && (!scub._rendered || reg != sideReg || geomIndex != geomSideIndex))
    {
      if(!scub._rendered)
      {
        scub._rendered = true;
        sideReg->_renderedCubCount[geomSideIndex]++;
      }

      curGeom = sideReg->GetGeometry(geomSideIndex);

      if(curGeom == NULL)
      {
        curGeom = NewOSGGeom();
        sideReg->SetGeometry(geomSideIndex, curGeom);

        _geode->addDrawable(curGeom);
      }

      _dataUpdate.push_back(DataUpdate(curGeom, sideReg, geomSideIndex));
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

    if(addToScene)
      rg->SetInScene(true);

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
