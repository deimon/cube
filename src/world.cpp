#include <world.h>
#include <wood.h>
#include <mathUtils.h>
#include <light.h>
#include <regionManager.h>
#include <geoMaker.h>
#include <RenderGroup.h>

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

  std::list<World::RegionsList*> _addRegionsForRenderFilling;
  std::list<World::RegionsList*> _addRegionsForLightFilling;
  std::list<World::RegionsList*> _addRegionsForCubFilling;

  std::list<cube::Region*> _addToSceneRegions;

  void Clear()
  {
    while(!_completed)
      OpenThreads::Thread::microSleep(100);

    _addToSceneRegions.clear();

    while(!_addRegionsForCubFilling.empty())
    {
      World::RegionsList* rl = _addRegionsForCubFilling.front();
      rl->clear();
      delete rl;
      _addRegionsForCubFilling.pop_front();
    }

    while(!_addRegionsForLightFilling.empty())
    {
      World::RegionsList* rl = _addRegionsForLightFilling.front();
      rl->clear();
      delete rl;
      _addRegionsForLightFilling.pop_front();
    }

    while(!_addRegionsForRenderFilling.empty())
    {
      World::RegionsList* rl = _addRegionsForRenderFilling.front();
      rl->clear();
      delete rl;
      _addRegionsForRenderFilling.pop_front();
    }
  }

protected:
  ~CreateGeomThread() { quit(); }

private:
  virtual void run()
  {
    _done = false;

    while (!_done)
    {
      microSleep(100);
      if(!_completed)
      {
        if(!_addRegionsForCubFilling.empty())
        {
          World::RegionsList* curList = _addRegionsForCubFilling.front();
          _addRegionsForCubFilling.pop_front();

          while(!curList->empty())
          {
            cube::Region* reg = curList->front();
            curList->pop_front();
            reg->CubFilling();
          }

          delete curList;
        }

        if(!_addRegionsForLightFilling.empty())
        {
          World::RegionsList* curList = _addRegionsForLightFilling.front();
          _addRegionsForLightFilling.pop_front();

          while(!curList->empty())
          {
            cube::Region* reg = curList->front();
            curList->pop_front();
            reg->LightFilling();
          }

          delete curList;
        }

        if(!_addRegionsForRenderFilling.empty())
        {
          World::RegionsList* curList = _addRegionsForRenderFilling.front();
          _addRegionsForRenderFilling.pop_front();

          while(!curList->empty())
          {
            cube::Region* reg = curList->front();
            curList->pop_front();
            reg->RenderFilling();

            _world->UpdateRegionGeoms(reg, false);

            _addToSceneRegions.push_back(reg);
          }

          delete curList;
        }

        _completed = true;
      }
    }
  }

  bool _done;

  bool _completed;

  cube::World* _world;
};

World::World()
{
  _hudMode = false;
  _mapCreated = false;
  _newMap = false;
  _cgThread = new CreateGeomThread(this);
}

void World::update(double time)
{
  if(_newMap)
  {
    if(_mapCreated)
    {
      _mapCreated = false;
      destroyMap();
    }
    createMap();
    _newMap = false;

    return;
  }

  if(!_mapCreated)
    return;

  // отправка в поток регионов на обработку и получение обработаных регионов
  if(_cgThread->IsCompleted() /*&& !_addRegions.empty()*/)
  {
    while(!_addRegionsForCubFilling.empty())
    {
      RegionsList* rl = _addRegionsForCubFilling.front();
      
      if(rl->empty() && _addRegionsForCubFilling.size() == 1)
        break;

      _cgThread->_addRegionsForCubFilling.push_back(rl);
      _addRegionsForCubFilling.pop_front();
    }

    while(!_addRegionsForLightFilling.empty())
    {
      RegionsList* rl = _addRegionsForLightFilling.front();

      if(rl->empty() && _addRegionsForLightFilling.size() == 1)
        break;

      _cgThread->_addRegionsForLightFilling.push_back(rl);
      _addRegionsForLightFilling.pop_front();
    }

    while(!_addRegionsForRenderFilling.empty())
    {
      RegionsList* rl = _addRegionsForRenderFilling.front();

      if(rl->empty() && _addRegionsForRenderFilling.size() == 1)
        break;

      _cgThread->_addRegionsForRenderFilling.push_back(rl);
      _addRegionsForRenderFilling.pop_front();
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

  int curRegX = Region::ToRegionIndex(_you.x());
  int curRegY = Region::ToRegionIndex(_you.y());

  {
    RenderGroup::DataUpdateContainer updateGeomMap;

    for(int i = -1; i <= 1; i++)
    for(int j = -1; j <= 1; j++)
    {
      cube::Region* reg = RegionManager::Instance().ContainsRegion(curRegX, curRegY);
      if(reg && reg->InScene())
        reg->UpdateCubs(time, &updateGeomMap);
    }

    _renderGroup->PushToUpdate(&updateGeomMap);
  }

  // пересоздание измененных геометрий (удаление/добавление кубика, распространение света и т.д.)
  _renderGroup->Update();

  // добавление и удаление из очереди в сцену по 1 региону за фрейм
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
        _renderGroup->FillRegionGeoms(reg);
      }
    } while(!add && !_addToSceneRegions.empty());

    //del
    if(!_delRegionsForVisual.empty())
    {
      bool del = false;
      do 
      {
        cube::Region* reg = _delRegionsForVisual.front();
        _delRegionsForVisual.pop_front();

        if(reg != NULL && !reg->InVisibleZone() && reg->InScene())
        {
          del = true;
          reg->SetInScene(false);
          _renderGroup->ClearRegionGeoms(reg);
        }
      } while(!del && !_delRegionsForVisual.empty());
    }
  }

  _frame++;
  _frame %= 5;

  // добавление в очередь на обработку регионов появляющихся или исчезающих на горизонте
  bool newRegionList = false;

  if(curRegX > _prevRegX)
  {
    newRegionList = true;
    _prevRegX = curRegX;

    for(int j = -_radius - 2; j <= _radius + 2; j++)
    {
      int i = _radius + 2;
      int x = i + curRegX;
      int y = j + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      _addRegionsForCubFilling.back()->push_back(reg);
    }

    for(int j = -_radius - 1; j <= _radius + 1; j++)
    {
      int i = _radius + 1;
      int x = i + curRegX;
      int y = j + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      _addRegionsForLightFilling.back()->push_back(reg);
    }

    for(int j = -_radius; j <= _radius; j++)
    {
      //add
      int i = _radius;
      int x = i + curRegX;
      int y = j + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      reg->SetVisibleZone(true);
      _addRegionsForRenderFilling.back()->push_back(reg);

      //del
      i = -_radius;
      x = i + curRegX - 1;
      y = j + curRegY;
      reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg)
      {
        reg->SetVisibleZone(false);
        reg->ResetGeom();
        _delRegionsForVisual.push_back(reg);
      }
    }
  }

  if(curRegX < _prevRegX)
  {
    newRegionList = true;
    _prevRegX = curRegX;

    for(int j = -_radius - 2; j <= _radius + 2; j++)
    {
      int i = -_radius - 2;
      int x = i + curRegX;
      int y = j + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      _addRegionsForCubFilling.back()->push_back(reg);
    }

    for(int j = -_radius - 1; j <= _radius + 1; j++)
    {
      int i = -_radius - 1;
      int x = i + curRegX;
      int y = j + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      _addRegionsForLightFilling.back()->push_back(reg);
    }

    for(int j = -_radius; j <= _radius; j++)
    {
      //add
      int i = -_radius;
      int x = i + curRegX;
      int y = j + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      reg->SetVisibleZone(true);
      _addRegionsForRenderFilling.back()->push_back(reg);

      //del
      i = _radius;
      x = i + curRegX + 1;
      y = j + curRegY;
      reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg)
      {
        reg->SetVisibleZone(false);
        reg->ResetGeom();
        _delRegionsForVisual.push_back(reg);
      }
    }
  }

  if(curRegY > _prevRegY)
  {
    newRegionList = true;
    _prevRegY = curRegY;

    for(int j = -_radius - 2; j <= _radius + 2; j++)
    {
      int i = _radius + 2;
      int x = j + curRegX;
      int y = i + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      _addRegionsForCubFilling.back()->push_back(reg);
    }

    for(int j = -_radius - 1; j <= _radius + 1; j++)
    {
      int i = _radius + 1;
      int x = j + curRegX;
      int y = i + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      _addRegionsForLightFilling.back()->push_back(reg);
    }

    for(int j = -_radius; j <= _radius; j++)
    {
      //add
      int i = _radius;
      int x = j + curRegX;
      int y = i + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      reg->SetVisibleZone(true);
      _addRegionsForRenderFilling.back()->push_back(reg);

      //del
      i = -_radius;
      x = j + curRegX;
      y = i + curRegY - 1;
      reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg)
      {
        reg->SetVisibleZone(false);
        reg->ResetGeom();
        _delRegionsForVisual.push_back(reg);
      }
    }
  }

  if(curRegY < _prevRegY)
  {
    newRegionList = true;
    _prevRegY = curRegY;

    for(int j = -_radius - 2; j <= _radius + 2; j++)
    {
      int i = -_radius - 2;
      int x = j + curRegX;
      int y = i + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      _addRegionsForCubFilling.back()->push_back(reg);
    }

    for(int j = -_radius - 1; j <= _radius + 1; j++)
    {
      int i = -_radius - 1;
      int x = j + curRegX;
      int y = i + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      _addRegionsForLightFilling.back()->push_back(reg);
    }

    for(int j = -_radius; j <= _radius; j++)
    {
      //add
      int i = -_radius;
      int x = j + curRegX;
      int y = i + curRegY;
      cube::Region* reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg == NULL)
        reg = cube::Region::Generation(x, y);
      reg->SetVisibleZone(true);
      _addRegionsForRenderFilling.back()->push_back(reg);

      //del
      i = _radius;
      x = j + curRegX;
      y = i + curRegY + 1;
      reg = RegionManager::Instance().ContainsRegion(x, y);
      if(reg)
      {
        reg->SetVisibleZone(false);
        reg->ResetGeom();
        _delRegionsForVisual.push_back(reg);
      }
    }
  }

  if(newRegionList)
  {
    _addRegionsForRenderFilling.push_back(new RegionsList);
    _addRegionsForCubFilling.push_back(new RegionsList);
    _addRegionsForLightFilling.push_back(new RegionsList);
  }
}

void World::del(cube::CubRegion& cubReg, osg::Vec3d wcpos)
{
  RenderGroup::DataUpdateContainer updateGeomMap;

  cubReg.SetCubType(cube::Block::Air);
  cubReg.SetCubRendered(false);

  bool blend = cubReg.GetCubBlend();
  cubReg.SetCubBlend(false);

  osg::Geometry* curGeom = cubReg.GetRegion()->GetGeometry(cubReg.GetGeomIndex(), blend);

  updateGeomMap[curGeom] = RenderGroup::DataUpdate(curGeom, cubReg.GetRegion(), cubReg.GetGeomIndex(), blend);

  //*************************************
  cube::Light::RecalcAndFillingLight(cubReg, wcpos, &updateGeomMap);

  _renderGroup->PushToUpdate(&updateGeomMap);
}

void World::add(cube::CubRegion& cubReg, osg::Vec3d wcpos, bool recalcLight)
{
  RenderGroup::DataUpdateContainer updateGeomMap;

  if(!cubReg.GetCubRendered())
  {
    cubReg.SetCubRendered(true);
  }

  osg::Geometry* curGeom = cubReg.GetRegion()->GetGeometry(cubReg.GetGeomIndex(), cubReg.GetCubBlend());

  updateGeomMap[curGeom] = RenderGroup::DataUpdate(curGeom, cubReg.GetRegion(), cubReg.GetGeomIndex(), cubReg.GetCubBlend());

  //*************************************
  if(recalcLight)
  {
    cube::Light::FindLightSourceAndFillingLight(cubReg, wcpos, &updateGeomMap);
  }

  _renderGroup->PushToUpdate(&updateGeomMap);
}


void World::RemoveCub(osg::Vec3d vec)
{
  int geomIndex = vec.z() / GEOM_SIZE;

  if(geomIndex < 0 || geomIndex > GEOM_COUNT)
    return;

  cube::Region* reg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(vec.x()), Region::ToRegionIndex(vec.y()));
  osg::Vec3d cvec = vec - reg->GetPosition();
  cube::CubRegion cubReg = reg->GetCub(cvec.x(), cvec.y(), cvec.z());

  del(cubReg, vec);

  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    osg::Vec3d wcvec = vec + CubInfo::Instance().GetNormal(side);
    cvec = wcvec;

    cube::Region* sideReg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(cvec.x()), Region::ToRegionIndex(cvec.y()));
    cvec -= sideReg->GetPosition();
    cube::CubRegion scubReg = sideReg->GetCub(cvec.x(), cvec.y(), cvec.z());

    int geomSideIndex = cvec.z() / GEOM_SIZE;

    if(scubReg.GetCubType() != cube::Block::Air && 
      (!scubReg.GetCubRendered() || reg != sideReg || geomIndex != geomSideIndex || cubReg.GetCubBlend() != scubReg.GetCubBlend()))
    {
      add(scubReg, wcvec);
    }
  }
}

void World::AddCub(osg::Vec3d vec, Block::BlockType cubeType)
{
  cube::Region* reg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(vec.x()), Region::ToRegionIndex(vec.y()));
  osg::Vec3d cvec = vec - reg->GetPosition();

  CubInfo::CubeSide side = cube::MathUtils::CubIntersection(reg, cvec.x(), cvec.y(), cvec.z(), _you, vec);

  osg::Vec3d norm = CubInfo::Instance().GetNormal(side);
  {
    cvec = vec + norm;

    reg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(cvec.x()), Region::ToRegionIndex(cvec.y()));
    cvec -= reg->GetPosition();
    cube::CubRegion scubReg = reg->GetCub(cvec.x(), cvec.y(), cvec.z());

    if(scubReg.GetCubType() == cube::Block::Air)
    {
      scubReg.SetCubType(cubeType);
      if(cubeType == cube::Block::Water)
      {
        scubReg.SetCubBlend(true);
      }
      else
      {
        //scubReg.GetCubLight() = 0.1f;

        if(scubReg.GetCubType() == cube::Block::Pumpkin)
        {
          // временный блок
          RenderGroup::DataUpdateContainer updateGeomMap;

          cube::Light::fillingLocLight(scubReg, vec + norm, 1.0f, &updateGeomMap);

          _renderGroup->PushToUpdate(&updateGeomMap);
        }
      }

      add(scubReg, vec + norm, true);
    }
  }
}

void World::UpdateRegionGeoms(cube::Region* rg, bool addToScene)
{
  if(!rg->_geometryCreated && rg->InVisibleZone())
  {
    _renderGroup->UpdateRegionGeoms(rg, addToScene);

    if(addToScene)
      rg->SetInScene(true);

    rg->_geometryCreated = true;
  }
}

void World::CreateMap(int rnd)
{
  _newRnd = rnd;
  _newMap = true;
  _you.set(5.0, 5.0, 100.0);
}

void World::startFilling(World* world, int radius, int i, int j)
{
  cube::Region* region = cube::Region::Generation(i, j);
  
  world->_addRegionsForCubFilling.back()->push_back(region);
  
  if(radius <= world->_radius + 1)
    world->_addRegionsForLightFilling.back()->push_back(region);

  if(radius <= world->_radius)
  {
    region->SetVisibleZone(true);
    world->_addRegionsForRenderFilling.back()->push_back(region);
  }
}

void World::createMap()
{
  if(_rnd == 777)
    _newRnd = 27;
  _rnd = _newRnd;

  cube::GeoMaker::InitPerlin(_rnd);

  _frame = 0;
  srand(time(NULL));

  _addRegionsForCubFilling.push_back(new RegionsList);

  _addRegionsForLightFilling.push_back(new RegionsList);
  _addRegionsForLightFilling.push_back(new RegionsList);

  _addRegionsForRenderFilling.push_back(new RegionsList);
  _addRegionsForRenderFilling.push_back(new RegionsList);
  _addRegionsForRenderFilling.push_back(new RegionsList);

  // эти параметры надо будет устанавливать
  // в соответствии с местом появления персонажа
  _prevRegX = 0;
  _prevRegY = 0;

  _worldRadius = 8;
  _radius = 4;

  int i = 0, j = 0;
  startFilling(this, 0, i, j);
  for(int r = 1; r <= _radius + 2; r++)
  {
    j = -r;
    for(i = -r; i <= r-1; i++)
      startFilling(this, r, i, j);

    i = r;
    for(j = -r; j <= r-1; j++)
      startFilling(this, r, i, j);

    j = r;
    for(i = r; i >= -(r-1); i--)
      startFilling(this, r, i, j);

    i = -r;
    for(j = r; j >= -(r-1); j--)
      startFilling(this, r, i, j);

    _addRegionsForCubFilling.push_back(new RegionsList);
    
    if(r <= _radius + 1)
      _addRegionsForLightFilling.push_back(new RegionsList);

    if(r <= _radius)
      _addRegionsForRenderFilling.push_back(new RegionsList);
  }

  _mapCreated = true;
}

void World::destroyMap()
{
  _cgThread->Clear();
  _renderGroup->ClearDataUpdate();
  _delRegionsForVisual.clear();
  _addToSceneRegions.clear();

  while(!_addRegionsForCubFilling.empty())
  {
    RegionsList* rl = _addRegionsForCubFilling.front();
    rl->clear();
    delete rl;
    _addRegionsForCubFilling.pop_front();
  }

  while(!_addRegionsForLightFilling.empty())
  {
    RegionsList* rl = _addRegionsForLightFilling.front();
    rl->clear();
    delete rl;
    _addRegionsForLightFilling.pop_front();
  }

  while(!_addRegionsForRenderFilling.empty())
  {
    RegionsList* rl = _addRegionsForRenderFilling.front();
    rl->clear();
    delete rl;
    _addRegionsForRenderFilling.pop_front();
  }

  struct ClearRegCallback : public RegionManager::Callback
  {
    ClearRegCallback(World* world) : _world(world) {}
    void operator()(cube::Region* reg)
    {
      _world->_renderGroup->ClearRegionGeoms(reg);
    }

    World* _world;
  };

  RegionManager::Instance().ForacheRegion(ClearRegCallback(this));

  RegionManager::Instance().Save();
  RegionManager::Instance().Clear();
}

osg::Group* World::GetGeometry()
{
  _renderGroup = new RenderGroup(this);

  _newRnd = 777;
  _you.set(5.0, 5.0, 100.0);
  createMap();

  return _renderGroup;
}
