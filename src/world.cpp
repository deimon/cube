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
            if(!reg->IsAreaGenerated2())
            {
              reg->FillRegion2();
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
  _rnd = 7.0f; // osg::PI*2*10 + ((float)rand() / RAND_MAX)* (osg::PI*3*10 - osg::PI*2*10);

  int radius = 8;

  cube::Areas::Instance().SetRadius(radius);

  for(int i = 0; i < Areas::Instance().GetSize(); i++)
  for(int j = 0; j < Areas::Instance().GetSize(); j++)
    if(Areas::Instance()._circle[i][j] == 1)
    {
      cube::Region* region = cube::Region::Generation(i - radius, j - radius);
      region->FillRegion(_rnd);
      region->SetVisibleZone(true);
    }

    for(int i = 0; i < Areas::Instance().GetSize(); i++)
    for(int j = 0; j < Areas::Instance().GetSize(); j++)
      if(Areas::Instance()._circle[i][j] == 1)
      {
        RegionManager::Instance().GetRegion(i - radius, j - radius)->FillRegion2();
      }

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
  curGeom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
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
    const cube::Cub &cub = reg->GetCub(x, y, z + zOffset);

    if(cub._type == cube::Cub::Air || !cub._rendered || cub._blend != blend) //!!!!!!!
      continue;

    osg::Vec3d pos = reg->GetPosition() + osg::Vec3d( x, y, z + zOffset);

    for(int side = CubInfo::FirstSide; side <= CubInfo::EndSide; side++)
    {
      CubInfo::CubeSide cside = (CubInfo::CubeSide)side;

      osg::Vec3d sidePos = pos + CubInfo::Instance().GetNormal(cside) + osg::Vec3d(0.1, 0.1, 0.1);
      cube::Cub* sideCub = RegionManager::Instance().GetCub(sidePos.x(), sidePos.y(), sidePos.z());

      if(sideCub == NULL || sideCub->_type == cube::Cub::Air || sideCub->_type == cube::Cub::LeavesWood || sideCub->_type == cube::Cub::TruncWood)
      {
        CubInfo::Instance().FillVertCoord(cside, coords, pos);

        _texInfo->FillTexCoord(cub._type, cside, tcoords);

        osg::Vec4d color = _texInfo->GetSideColor(cub._type, cside);
        if(sideCub != NULL)
          color *= sideCub->_light;
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
  // отправка в поток регионов на обработку и получение обработаных регионов
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
        //_regions[reg->GetX()][reg->GetY()] = reg;
        //_regionsCreated[regPos.x][regPos.y] = true;

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
    if(!_delRegions.empty())
    {
      bool del = false;
      do 
      {
        Areas::v2 regPos = _delRegions.front().second;
        cube::Region* reg = RegionManager::Instance().ContainsRegion(regPos.x, regPos.y);
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

  // добавление в очередь на обработку регионов появляющихся или исчезающих на горизонте
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
      cube::Region* reg = RegionManager::Instance().ContainsRegion(addOffs.x, addOffs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(addOffs.x, addOffs.y);
      reg->SetVisibleZone(true);
      _addRegions.push_back(reg);

      //del
      Areas::v2 delOff = Areas::Instance()._xn[i];
      delOff.x += curRegX - 1;
      delOff.y += curRegY;
      reg = RegionManager::Instance().ContainsRegion(delOff.x, delOff.y);
      if(reg)
      {
        reg->SetVisibleZone(false);
        _delRegions.push_back(std::make_pair(reg, delOff));
      }
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
      cube::Region* reg = RegionManager::Instance().ContainsRegion(addOffs.x, addOffs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(addOffs.x, addOffs.y);
      reg->SetVisibleZone(true);
      _addRegions.push_back(reg);

      //del
      Areas::v2 delOff = Areas::Instance()._xp[i];
      delOff.x += curRegX + 1;
      delOff.y += curRegY;
      reg = RegionManager::Instance().ContainsRegion(delOff.x, delOff.y);
      if(reg)
      {
        reg->SetVisibleZone(false);
        _delRegions.push_back(std::make_pair(reg, delOff));
      }
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
      cube::Region* reg = RegionManager::Instance().ContainsRegion(addOffs.x, addOffs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(addOffs.x, addOffs.y);
      reg->SetVisibleZone(true);
      _addRegions.push_back(reg);

      //del
      Areas::v2 delOff = Areas::Instance()._yn[i];
      delOff.x += curRegX;
      delOff.y += curRegY - 1;
      reg = RegionManager::Instance().ContainsRegion(delOff.x, delOff.y);
      if(reg)
      {
        reg->SetVisibleZone(false);
        _delRegions.push_back(std::make_pair(reg, delOff));
      }
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
      cube::Region* reg = RegionManager::Instance().ContainsRegion(addOffs.x, addOffs.y);
      if(reg == NULL)
        reg = cube::Region::Generation(addOffs.x, addOffs.y);
      reg->SetVisibleZone(true);
      _addRegions.push_back(reg);

      //del
      Areas::v2 delOff = Areas::Instance()._yp[i];
      delOff.x += curRegX;
      delOff.y += curRegY + 1;
      reg = RegionManager::Instance().ContainsRegion(delOff.x, delOff.y);
      if(reg)
      {
        reg->SetVisibleZone(false);
        _delRegions.push_back(std::make_pair(reg, delOff));
      }
    }
  }
}

void World::del(cube::Cub& cub, cube::Region* reg, int geomIndex, osg::Vec3d wcpos)
{
  std::map<osg::Geometry*, DataUpdate> updateGeomMap;

  cub._type = cube::Cub::Air;
  cub._rendered = false;

  int renderedCubCount = --reg->_renderedCubCount[cub._blend?1:0][geomIndex];

  osg::Geometry* curGeom = reg->GetGeometry(geomIndex, cub._blend);

  updateGeomMap[curGeom] = DataUpdate(curGeom, reg, geomIndex, cub._blend);

  //*************************************
  //cube::Light::RecalcAndFillingLight(cub, wcpos, updateGeomMap);

  std::map<osg::Geometry*, DataUpdate>::iterator i = updateGeomMap.begin();
  for(; i != updateGeomMap.end(); i++)
    _dataUpdate.push_back(i->second);
}

void World::add(cube::Cub& cub, cube::Region* reg, int geomIndex, osg::Vec3d wcpos, bool recalcLight)
{
  std::map<osg::Geometry*, DataUpdate> updateGeomMap;

  if(!cub._rendered)
  {
    cub._rendered = true;
    reg->_renderedCubCount[cub._blend?1:0][geomIndex]++;
  }

  osg::Geometry* curGeom = reg->GetGeometry(geomIndex, cub._blend);

  updateGeomMap[curGeom] = DataUpdate(curGeom, reg, geomIndex, cub._blend);

  //*************************************
  if(recalcLight)
  {
    //cube::Light::FindLightSourceAndFillingLight(cub, wcpos, updateGeomMap);
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
  cube::Cub& cub = reg->GetCub(cvec.x(), cvec.y(), cvec.z());

  del(cub, reg, geomIndex, vec);

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
    cube::Cub& scub = sideReg->GetCub(cvec.x(), cvec.y(), cvec.z());

    int geomSideIndex = cvec.z() / GEOM_SIZE;

    if(scub._type != cube::Cub::Air && (!scub._rendered || reg != sideReg || geomIndex != geomSideIndex || cub._blend != scub._blend))
    {
      add(scub, sideReg, geomSideIndex, wcvec);
    }
  }
}

void World::AddCub(osg::Vec3d vec)
{
  cube::Region* reg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(vec.x()), Region::ToRegionIndex(vec.y()));
  osg::Vec3d cvec = vec - reg->GetPosition();

  CubInfo::CubeSide side = cube::MathUtils::CubIntersection(reg, cvec.x(), cvec.y(), cvec.z(), _you, vec);

  osg::Vec3d norm = CubInfo::Instance().GetNormal(side);
  {
    cvec = vec + norm;

    reg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(cvec.x()), Region::ToRegionIndex(cvec.y()));
    cvec -= reg->GetPosition();
    cube::Cub& scub = reg->GetCub(cvec.x(), cvec.y(), cvec.z());

    if(scub._type == cube::Cub::Air)
    {
      scub._type = cube::Cub::Ground;
      scub._blend = false;

      cvec /= GEOM_SIZE;

      add(scub, reg, cvec.z(), vec + norm, true);
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

  struct WoodGenerateCallback: RegionManager::Callback
  {
    virtual void operator()(cube::Region* reg)
    {
      cube::Wood::Generate(RegionManager::Instance(), reg, 
                           cube::MathUtils::random(0, REGION_WIDTH),
                           cube::MathUtils::random(0, REGION_WIDTH));
    }
  };

  WoodGenerateCallback wgc;
  RegionManager::Instance().ForacheRegion(wgc);

  struct UpdateRegionGeomsCallback: RegionManager::Callback
  {
    virtual void operator()(cube::Region* reg)
    {
      World::Instance().UpdateRegionGeoms(reg);
    }
  };

  UpdateRegionGeomsCallback urgc;
  RegionManager::Instance().ForacheRegion(urgc);

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
  _geode[1]->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);

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
  }

  return _group;
}
