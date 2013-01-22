#include <RenderGroup.h>
#include <world.h>
#include <regionManager.h>

#include <osg/NodeCallback>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Texture2D>

using namespace cube;

class WorldCallback
  : public osg::NodeCallback
{
public:
  WorldCallback(cube::World *world): _world(world) {};
  virtual void operator()(osg::Node* node, osg::NodeVisitor* nv)
  {
    _world->update();
    traverse(node, nv);
  }

private:
  cube::World *_world;
};

RenderGroup::RenderGroup(cube::World *world)
{
  setUpdateCallback(new WorldCallback(world));
  removeChildren(0, getNumChildren());

  _texInfo = new TextureInfo("./res/mc16-7.png", 16);
  getOrCreateStateSet()->setTextureAttributeAndModes(0, _texInfo->GetTexture(), osg::StateAttribute::ON);

  _geode[0] = new osg::Geode;
  _geode[1] = new osg::Geode;

  addChild(_geode[0]);
  addChild(_geode[1]);

  _geode[0]->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);
  _geode[1]->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);
  _geode[1]->getOrCreateStateSet()->setMode(GL_BLEND, osg::StateAttribute::ON);

  {
    osg::Shader *vs = new osg::Shader(osg::Shader::VERTEX);
    vs->loadShaderSourceFromFile("./res/shaders/main.vert");
    osg::Shader *fs = new osg::Shader(osg::Shader::FRAGMENT);
    fs->loadShaderSourceFromFile("./res/shaders/main.frag");

    osg::StateSet* ss = getOrCreateStateSet();

    osg::Program* program = new osg::Program();
    program->addShader(vs);
    program->addShader(fs);
    ss->setAttributeAndModes(program, osg::StateAttribute::ON | osg::StateAttribute::PROTECTED);
    ss->addUniform(new osg::Uniform("texture", 0));

    _worldLight = new osg::Uniform("sun", 0.6f);
    ss->addUniform(_worldLight);
  }
}

void RenderGroup::Update()
{
  for(int i = 0; i < _dataUpdate.size(); i++)
  {
    //if(_dataUpdate[i]._geom)
    updateGeom(_dataUpdate[i]._geom, _dataUpdate[i]._reg, _dataUpdate[i]._zCubOff, _dataUpdate[i]._blend, true);
  }
  _dataUpdate.clear();
}

 void RenderGroup::PushToUpdate(std::map<osg::Geometry*, RenderGroup::DataUpdate>* updateGeomMap)
 {
   std::map<osg::Geometry*, RenderGroup::DataUpdate>::iterator i = updateGeomMap->begin();
   for(; i != updateGeomMap->end(); i++)
     _dataUpdate.push_back(i->second);
 }

void RenderGroup::UpdateRegionGeoms(cube::Region* rg, bool addToScene)
{
  for(int s = 0; s < 2; s++)
    for(int offset = 0; offset < GEOM_COUNT; offset++)
    {
      if(rg->_renderedCubCount[s][offset] < 1)
        continue;

      osg::Geometry* curGeom = rg->GetGeometry(offset, s == 1);

      updateGeom(curGeom, rg, offset * GEOM_SIZE, s == 1, addToScene);
    }
}

void RenderGroup::FillRegionGeoms(cube::Region* rg)
{
  for(int s = 0; s < 2; s++)
    for(int offset = 0; offset < GEOM_COUNT; offset++)
    {
      osg::Geometry* curGeom = rg->GetGeometry(offset, s == 1);

      if(curGeom != NULL)
        _geode[s]->addDrawable(curGeom);
    }
}

void RenderGroup::ClearRegionGeoms(cube::Region* rg)
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
    else
      if(!rg->_geomToClear.empty())
      {
        for(int i = 0; i < rg->_geomToClear.size(); i++)
          _geode[rg->_geomToClear[i].first]->removeDrawable(rg->_geomToClear[i].second);

        rg->_geomToClear.clear();
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

void RenderGroup::updateGeom(osg::Geometry* geom, cube::Region* reg, int zOffset, bool blend, bool updateScene)
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

    if(cubReg.GetCubType() == cube::Block::Air || !cubReg.GetCubRendered() || cubReg.GetCubBlend() != blend) //!!!!!!!
      continue;

    osg::Vec3d pos = reg->GetPosition() + osg::Vec3d( x, y, z + zOffset);

    for(int side = CubInfo::FirstSide; side <= CubInfo::EndSide; side++)
    {
      CubInfo::CubeSide cside = (CubInfo::CubeSide)side;

      osg::Vec3d sidePos = pos + CubInfo::Instance().GetNormal(cside) + osg::Vec3d(0.1, 0.1, 0.1);
      cube::CubRegion scubReg = RegionManager::Instance().GetCub(sidePos.x(), sidePos.y(), sidePos.z());

      if(  scubReg.GetCubType() == cube::Block::Air 
        || scubReg.GetCubType() == cube::Block::LeavesWood 
        || scubReg.GetCubType() == cube::Block::TruncWood
        || (scubReg.GetCubType() == cube::Block::Water && cubReg.GetCubType() != cube::Block::Water))
      {
        CubInfo::Instance().FillVertCoord(cside, coords, pos);

        _texInfo->FillTexCoord(cubReg.GetCubType(), cside, tcoords);

        osg::Vec4d color = _texInfo->GetSideColor(cubReg.GetCubType(), cside);

        CubInfo::Instance().FillColorBuffer(cside, colours, pos, color);

        normals->push_back(CubInfo::Instance().GetNormal(cside));
      }
    }

    drawArr->setCount(coords->size());
  }

  geom->dirtyDisplayList();
  geom->dirtyBound();
}
