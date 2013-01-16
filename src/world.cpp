#include <world.h>
#include <wood.h>
#include <mathUtils.h>
#include <light.h>
#include <regionManager.h>

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
            if(!reg->IsCubFilled())
            {
              reg->CubFilling(_world->_rnd);

              if(!reg->IsOffside())
              {
                for(int k = 0; k < cube::MathUtils::random(0, 32); k++)
                {
                  cube::Wood::Generate(RegionManager::Instance(), reg, 
                    cube::MathUtils::random(0, REGION_WIDTH),
                    cube::MathUtils::random(0, REGION_WIDTH));
                }
              }
            }
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
            if(!reg->IsLightFilled())
            {
              reg->LightFilling();
            }
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

            if(!reg->IsRenderFilled())
            {
              reg->RenderFilling();
            }

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

void WorldCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
  _world->update();
  traverse(node, nv);
}


World::World()
{
  _addRegionsForRenderFilling.push_back(new RegionsList);
  _addRegionsForCubFilling.push_back(new RegionsList);
  _addRegionsForLightFilling.push_back(new RegionsList);

  _frame = 0;
  // эти параметры надо будет устанавливать
  // в соответствии с местом появления персонажа
  _prevRegX = 0;
  _prevRegY = 0;
  _you.set(5.0, 5.0, 0.0);

  srand(time(NULL));
  _rnd = 777.0f; // osg::PI*2*10 + ((float)rand() / RAND_MAX)* (osg::PI*3*10 - osg::PI*2*10);
  _worldRadius = 8;

  _radius = 4;

#ifdef DUBUGMODE
  {//DEBUG
    SYSTEMTIME sm;
    GetSystemTime(&sm);
    std::cout << "START StartedCubFilling: " << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << std::endl;
  }
#endif

  for(int i = -_radius - 2; i <= _radius + 2; i++)
  for(int j = -_radius - 2; j <= _radius + 2; j++)
  {
    cube::Region* region = cube::Region::Generation(i, j);
    region->CubFilling(_rnd);
  }

  for(int i = -_radius - 2; i <= _radius + 2; i++)
  for(int j = -_radius - 2; j <= _radius + 2; j++)
  {
    cube::Region* region = RegionManager::Instance().GetRegion(i, j);

    for(int k = 0; k < cube::MathUtils::random(0, 32); k++)
    {
      cube::Wood::Generate(RegionManager::Instance(), region, 
        cube::MathUtils::random(0, REGION_WIDTH),
        cube::MathUtils::random(0, REGION_WIDTH));
    }
  }

#ifdef DUBUGMODE
  {//DEBUG
    SYSTEMTIME sm;
    GetSystemTime(&sm);
    std::cout << "END StartedCubFilling: " << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << std::endl;
  }
#endif

  _cgThread = new CreateGeomThread(this);
}

void World::clearRegionGeoms(cube::Region* rg)
{
  if(rg != NULL)
  {
    if(rg->_geometryCreated)
    {
      for(int s = 0; s < 2; s++)
      for(int i = 0; i < GEOM_COUNT; i++)
      {
        osg::Geometry* geom = rg->GetGeometry(i, s == 1);
        if(geom)
        {
          _geode[s]->removeDrawable(geom);
          rg->SetGeometry(i, NULL, s == 1);
        }
      }

      rg->_geometryCreated = false;
    }
  }
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
  curGeom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
  curGeom->setNormalArray(normals);
  curGeom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);

  curGeom->addPrimitiveSet(drawArr);

  curGeom->setTexCoordArray(0,tcoords);

  return curGeom;
}

void World::updateGeom(osg::Geometry* geom, cube::Region* reg, int zOffset, bool blend, bool updateScene)
{
  int geomIndex = zOffset / GEOM_SIZE;

  geom = reg->GetGeometry(geomIndex, blend);

  if(reg->_renderedCubCount[blend?1:0][geomIndex] > 0)
  {
    if(geom == NULL)
    {
      geom = NewOSGGeom();
      reg->SetGeometry(geomIndex, geom, blend);

      if(updateScene)
        _geode[blend?1:0]->addDrawable(geom);
    }
  }
  else
  {
    if(updateScene)
      _geode[blend?1:0]->removeDrawable(geom);
    reg->SetGeometry(geomIndex, NULL, blend);
    return;
  }

  //*****************************************************************************

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
    cube::CubRegion cubReg = reg->GetCub(x, y, z + zOffset);

    if(cubReg.GetCubType() == cube::Cub::Air || !cubReg.GetCubRendered() || cubReg.GetCubBlend() != blend) //!!!!!!!
      continue;

    osg::Vec3d pos = reg->GetPosition() + osg::Vec3d( x, y, z + zOffset);

    for(int side = CubInfo::FirstSide; side <= CubInfo::EndSide; side++)
    {
      CubInfo::CubeSide cside = (CubInfo::CubeSide)side;

      osg::Vec3d sidePos = pos + CubInfo::Instance().GetNormal(cside) + osg::Vec3d(0.1, 0.1, 0.1);
      cube::CubRegion scubReg = RegionManager::Instance().GetCub(sidePos.x(), sidePos.y(), sidePos.z());

      if(  scubReg.GetCubType() == cube::Cub::Air 
        || scubReg.GetCubType() == cube::Cub::LeavesWood 
        || scubReg.GetCubType() == cube::Cub::TruncWood
        || (scubReg.GetCubType() == cube::Cub::Water && cubReg.GetCubType() != cube::Cub::Water))
      {
        CubInfo::Instance().FillVertCoord(cside, coords, pos);

        _texInfo->FillTexCoord(cubReg.GetCubType(), cside, tcoords);

        osg::Vec4d color = _texInfo->GetSideColor(cubReg.GetCubType(), cside);

        //float light = scubReg.GetCubLight() + scubReg.GetCubLocLight();
        //if(light > 1.0f)
        //  light = 1.0f;
        //color *= light;

        //colours->push_back(color);
        //colours->push_back(color);
        //colours->push_back(color);
        //colours->push_back(color);

        CubInfo::Instance().FillColorBuffer(cside, colours, pos, color);

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
  // отправка в поток регионов на обработку и получение обработаных регионов
  if(_cgThread->IsCompleted() /*&& !_addRegions.empty()*/)
  {
    while(!_addRegionsForCubFilling.empty())
    {
      RegionsList* rl = _addRegionsForCubFilling.front();
      
      if(rl->empty())
        break;

      _cgThread->_addRegionsForCubFilling.push_back(rl);
      _addRegionsForCubFilling.pop_front();
    }

    while(!_addRegionsForLightFilling.empty())
    {
      RegionsList* rl = _addRegionsForLightFilling.front();

      if(rl->empty())
        break;

      _cgThread->_addRegionsForLightFilling.push_back(rl);
      _addRegionsForLightFilling.pop_front();
    }

    while(!_addRegionsForRenderFilling.empty())
    {
      RegionsList* rl = _addRegionsForRenderFilling.front();

      if(rl->empty())
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

  // пересоздание измененных геометрий (удаление/добавление кубика, распространение света и т.д.)
  for(int i = 0; i < _dataUpdate.size(); i++)
  {
    //if(_dataUpdate[i]._geom)
    updateGeom(_dataUpdate[i]._geom, _dataUpdate[i]._reg, _dataUpdate[i]._zCubOff, _dataUpdate[i]._blend, true);
  }
  _dataUpdate.clear();

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

        for(int s = 0; s < 2; s++)
        for(int offset = 0; offset < GEOM_COUNT; offset++)
        {
          osg::Geometry* curGeom = reg->GetGeometry(offset, s == 1);

          if(curGeom != NULL)
            _geode[s]->addDrawable(curGeom);
        }
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
          clearRegionGeoms(reg);
        }
      } while(!del && !_delRegionsForVisual.empty());
    }
  }

  _frame++;
  _frame %= 5;

  // добавление в очередь на обработку регионов появляющихся или исчезающих на горизонте
  int curRegX = Region::ToRegionIndex(_you.x());
  int curRegY = Region::ToRegionIndex(_you.y());

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
  std::map<osg::Geometry*, DataUpdate> updateGeomMap;

  cubReg.SetCubType(cube::Cub::Air);
  cubReg.SetCubRendered(false);

  bool blend = cubReg.GetCubBlend();
  cubReg.SetCubBlend(false);

  osg::Geometry* curGeom = cubReg.GetRegion()->GetGeometry(cubReg.GetGeomIndex(), blend);

  updateGeomMap[curGeom] = DataUpdate(curGeom, cubReg.GetRegion(), cubReg.GetGeomIndex(), blend);

  //*************************************
  cube::Light::RecalcAndFillingLight(cubReg, wcpos, &updateGeomMap);

  std::map<osg::Geometry*, DataUpdate>::iterator i = updateGeomMap.begin();
  for(; i != updateGeomMap.end(); i++)
    _dataUpdate.push_back(i->second);
}

void World::add(cube::CubRegion& cubReg, osg::Vec3d wcpos, bool recalcLight)
{
  std::map<osg::Geometry*, DataUpdate> updateGeomMap;

  if(!cubReg.GetCubRendered())
  {
    cubReg.SetCubRendered(true);
  }

  osg::Geometry* curGeom = cubReg.GetRegion()->GetGeometry(cubReg.GetGeomIndex(), cubReg.GetCubBlend());

  updateGeomMap[curGeom] = DataUpdate(curGeom, cubReg.GetRegion(), cubReg.GetGeomIndex(), cubReg.GetCubBlend());

  //*************************************
  if(recalcLight)
  {
    cube::Light::FindLightSourceAndFillingLight(cubReg, wcpos, &updateGeomMap);
  }

  std::map<osg::Geometry*, DataUpdate>::iterator i = updateGeomMap.begin();
  for(; i != updateGeomMap.end(); i++)
    _dataUpdate.push_back(i->second);
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

  // нахождение новой травки если её удалили
  /*if((int)(cvec.z()) == reg->GetHeight(cvec.x(), cvec.y()))
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
  }*/


  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    osg::Vec3d wcvec = vec + CubInfo::Instance().GetNormal(side);
    cvec = wcvec;

    cube::Region* sideReg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(cvec.x()), Region::ToRegionIndex(cvec.y()));
    cvec -= sideReg->GetPosition();
    cube::CubRegion scubReg = sideReg->GetCub(cvec.x(), cvec.y(), cvec.z());

    int geomSideIndex = cvec.z() / GEOM_SIZE;

    if(scubReg.GetCubType() != cube::Cub::Air && 
      (!scubReg.GetCubRendered() || reg != sideReg || geomIndex != geomSideIndex || cubReg.GetCubBlend() != scubReg.GetCubBlend()))
    {
      add(scubReg, wcvec);
    }
  }
}

void World::AddCub(osg::Vec3d vec, Cub::CubeType cubeType)
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

    if(scubReg.GetCubType() == cube::Cub::Air)
    {
      scubReg.SetCubType(cubeType);
      if(cubeType == cube::Cub::Water)
      {
        scubReg.SetCubBlend(true);
      }
      else
      {
        //scubReg.GetCubLight() = 0.1f;

        if(scubReg.GetCubType() == cube::Cub::Pumpkin)
        {
          // временный блок
          std::map<osg::Geometry*, DataUpdate> updateGeomMap;

          cube::Light::fillingLocLight(scubReg, vec + norm, 1.0f, &updateGeomMap);

          std::map<osg::Geometry*, DataUpdate>::iterator i = updateGeomMap.begin();
          for(; i != updateGeomMap.end(); i++)
            _dataUpdate.push_back(i->second);
        }
      }

      add(scubReg, vec + norm, true);
    }
  }
}

void World::UpdateRegionGeoms(cube::Region* rg, bool addToScene)
{
  if(!rg->_geometryCreated)
  {
    for(int s = 0; s < 2; s++)
    for(int offset = 0; offset < GEOM_COUNT; offset++)
    {
      if(rg->_renderedCubCount[s][offset] < 1)
        continue;

      osg::Geometry* curGeom = rg->GetGeometry(offset, s == 1);

      updateGeom(curGeom, rg, offset * GEOM_SIZE, s == 1, addToScene);
    }

    if(addToScene)
      rg->SetInScene(true);

    rg->_geometryCreated = true;
  }
}

osg::Geode* World::createGeometry()
{
  _geode[0] = new osg::Geode;
  _geode[1] = new osg::Geode;

  _texInfo = new TextureInfo("./res/mc16-7.png", 16);

  _group->getOrCreateStateSet()->setTextureAttributeAndModes(0, _texInfo->GetTexture(), osg::StateAttribute::ON);

#ifdef DUBUGMODE
  {//DEBUG
    SYSTEMTIME sm;
    GetSystemTime(&sm);
    std::cout << "START StartedLightFilling: " << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << std::endl;
  }
#endif

  for(int i = -_radius - 1; i <= _radius + 1; i++)
  for(int j = -_radius - 1; j <= _radius + 1; j++)
  {
    RegionManager::Instance().GetRegion(i, j)->LightFilling();
  }

#ifdef DUBUGMODE
  {//DEBUG
    SYSTEMTIME sm;
    GetSystemTime(&sm);
    std::cout << "END StartedLightFilling AND START StartedRenderFilling: " << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << std::endl;
  }
#endif

  for(int i = -_radius; i <= _radius; i++)
    for(int j = -_radius; j <= _radius; j++)
    {
      cube::Region* region = RegionManager::Instance().GetRegion(i, j);
      region->SetVisibleZone(true);
      region->RenderFilling();
    }

#ifdef DUBUGMODE
    {//DEBUG
      SYSTEMTIME sm;
      GetSystemTime(&sm);
      std::cout << "END StartedRenderFilling: " << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << std::endl;
    }
#endif

  for(int i = -_radius; i <= _radius; i++)
  for(int j = -_radius; j <= _radius; j++)
  {
    cube::Region* region = RegionManager::Instance().GetRegion(i, j);

    World::Instance().UpdateRegionGeoms(region);
  }

#ifdef DUBUGMODE
  {//DEBUG
    SYSTEMTIME sm;
    GetSystemTime(&sm);
    std::cout << "END UpdateRegionGeoms: " << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << std::endl;
  }
#endif

  return NULL;
}

osg::Group* World::GetGeometry()
{
  _group = new osg::Group;

  _group->setUpdateCallback(new WorldCallback(this));
  _group->removeChildren(0, _group->getNumChildren());

  createGeometry();
  _group->addChild(_geode[0]);
  _group->addChild(_geode[1]);

  _geode[0]->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);
  _geode[1]->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);
  _geode[1]->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

  {
    osg::Shader *vs = new osg::Shader(osg::Shader::VERTEX);
    vs->loadShaderSourceFromFile("./res/shaders/main.vert");
    osg::Shader *fs = new osg::Shader(osg::Shader::FRAGMENT);
    fs->loadShaderSourceFromFile("./res/shaders/main.frag");

    osg::StateSet* ss = _group->getOrCreateStateSet();

    osg::Program* program = new osg::Program();
    program->addShader(vs);
    program->addShader(fs);
    ss->setAttributeAndModes(program, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
    ss->addUniform(new osg::Uniform("texture", 0));

    _worldLight = new osg::Uniform("sun", 0.6f);
    ss->addUniform(_worldLight);
  }

  return _group;
}
