#include <world.h>

using namespace cube;

void WorldCallback::operator()(osg::Node* node, osg::NodeVisitor* nv)
{
  _world->update();
  traverse(node, nv);
}


World::World()
{
  srand(time(NULL));

  float rnd = osg::PI*2*10 + ((float)rand() / RAND_MAX)* (osg::PI*3*10 - osg::PI*2*10);

  for(int i = 0; i < 10; i++)
  for(int j = 0; j < 10; j++)
    cube::Region::Generation(this, i, j, rnd);

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

  _geode = createGeometry();
  _group->addChild(_geode);

  _geode->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);

  return _group;
}

void World::update()
{
  for(int i = 0; i < _dataUpdate.size(); i++)
  {
    if(_dataUpdate[i]._geom)
    {
        osg::Vec3Array* coords;
        osg::Vec4Array* colours;
        osg::Vec3Array* normals;
        osg::Vec2Array* tcoords;

        osg::DrawArrays* drawArr;

        coords = dynamic_cast<osg::Vec3Array*>(_dataUpdate[i]._geom->getVertexArray());
        colours = dynamic_cast<osg::Vec4Array*>(_dataUpdate[i]._geom->getColorArray());
        normals = dynamic_cast<osg::Vec3Array*>(_dataUpdate[i]._geom->getNormalArray());

        drawArr = dynamic_cast<osg::DrawArrays*>(_dataUpdate[i]._geom->getPrimitiveSet(0));

        tcoords = dynamic_cast<osg::Vec2Array*>(_dataUpdate[i]._geom->getTexCoordArray(0));

        coords->clear();
        colours->clear();
        normals->clear();
        tcoords->clear();

      for(int x = 0; x < GEOM_SIZE; x++)
      for(int y = 0; y < GEOM_SIZE; y++)
        for(int z = 0; z < GEOM_SIZE; z++)
        {
          const cube::Cub &cub = _dataUpdate[i]._reg->GetCub(x + _dataUpdate[i]._xCubOff, y + _dataUpdate[i]._yCubOff, z + _dataUpdate[i]._zCubOff);

          if(cub._type == cube::Cub::Air || !cub._rendered) //!!!!!!!
            continue;

          osg::Vec3d pos = _dataUpdate[i]._reg->GetPosition()
            + osg::Vec3d( (x + _dataUpdate[i]._xCubOff),
                          (y + _dataUpdate[i]._yCubOff),
                          (z + _dataUpdate[i]._zCubOff));

          osg::Vec4d color;

          if(cub._type == cube::Cub::Ground)
            //color = osg::Vec4d(0.5, 0.25, 0.0, 1.0);
            color = osg::Vec4d(1.0, 1.0, 1.0, 1.0);
          else if(cub._type == cube::Cub::Air)
            color = osg::Vec4d(0.0, 0.5, 1.0, 1.0);

          {
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(0.0, -1.0, 0.0));
          }

          {
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(1.0, 0.0, 0.0));
          }

          {
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(0.0, 1.0, 0.0));
          }

          {
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(-1.0, 0.0, 0.0));
          }

          {//5
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(0.0, 0.0, 1.0));
          }

          {//6
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(0.0, 0.0, -1.0));
          }

          drawArr->setCount(coords->size());
        }

        _dataUpdate[i]._geom->dirtyDisplayList();
    }
  }

  _dataUpdate.clear();
}

cube::Region* World::GetRegion(int x, int y)
{
  int i = x / REGION_WIDTH;
  int j = y / REGION_WIDTH;

  return _regions[i][j];
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
  cube::Region* rg = GetRegion(x,y);

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
  cube::Cub& cub = (cube::Cub&)GetCub(vec.x(), vec.y(), vec.z());

  cub._type = cube::Cub::Air;
  cub._rendered = false;

  for(int i = 0; i < _sides.size(); i++)
  {
    osg::Vec3d pos = vec + _sides[i];
    cube::Cub& scub = (cube::Cub&)GetCub(pos.x(), pos.y(), pos.z());
    if(scub._type != cube::Cub::Air)
      scub._rendered = true;
  }

  cube::Region* reg = GetRegion(vec.x(), vec.y());
  vec -= reg->GetPosition();
  vec /= GEOM_SIZE;
  osg::Geometry* curGeom = reg->GetGeometry(vec.z());

  if(curGeom == NULL)
  {
    curGeom = NewOSGGeom();
    reg->SetGeometry(vec.z(), curGeom);

    _geode->addDrawable(curGeom);
  }

  _dataUpdate.push_back(DataUpdate(curGeom, reg, vec));
}

osg::Geode* World::createGeometry()
{
  osg::Geode* geode = new osg::Geode;

  osg::StateSet* ss = geode->getOrCreateStateSet();

  osg::Texture2D* tex = new osg::Texture2D;
  osg::Image* img = osgDB::readImageFile("./res/ground.jpg");
  tex->setImage(img);

  ss->setTextureAttributeAndModes(0, tex, osg::StateAttribute::ON);

  tex->setFilter(osg::Texture::MIN_FILTER , osg::Texture::NEAREST);
  tex->setFilter(osg::Texture::MAG_FILTER , osg::Texture::NEAREST);

  RegionsContainer::iterator xrg;
  YRegionsContainer::iterator yrg;
  for(xrg = _regions.begin(); xrg != _regions.end(); xrg++)
    for(yrg = xrg->second.begin(); yrg != xrg->second.end(); yrg++)
  {
    cube::Region* rg = yrg->second;

    osg::Vec3Array* coords;
    osg::Vec4Array* colours;
    osg::Vec3Array* normals;
    osg::Vec2Array* tcoords;

    osg::DrawArrays* drawArr;

    for(int x = 0; x < REGION_WIDTH; x++)
      for(int y = 0; y < REGION_WIDTH; y++)
        for(int z = 0; z < REGION_HEIGHT; z++)
        {
          const cube::Cub &cub = rg->GetCub(x, y, z);

          if(cub._type == cube::Cub::Air || !cub._rendered) //!!!!!!!
            continue;


          osg::Vec3d pos = rg->GetPosition() + osg::Vec3d(x, y, z);

          osg::Geometry* curGeom = rg->GetGeometry(z / GEOM_SIZE);

          if(curGeom == NULL)
          {
            curGeom = NewOSGGeom();
            rg->SetGeometry(z / GEOM_SIZE, curGeom);

            geode->addDrawable(curGeom);
          }

          {
            coords = dynamic_cast<osg::Vec3Array*>(curGeom->getVertexArray());
            colours = dynamic_cast<osg::Vec4Array*>(curGeom->getColorArray());
            normals = dynamic_cast<osg::Vec3Array*>(curGeom->getNormalArray());

            drawArr = dynamic_cast<osg::DrawArrays*>(curGeom->getPrimitiveSet(0));

            tcoords = dynamic_cast<osg::Vec2Array*>(curGeom->getTexCoordArray(0));
          }

          osg::Vec4d color;

          if(cub._type == cube::Cub::Ground)
            //color = osg::Vec4d(0.5, 0.25, 0.0, 1.0);
            color = osg::Vec4d(1.0, 1.0, 1.0, 1.0);
          else if(cub._type == cube::Cub::Air)
            color = osg::Vec4d(0.0, 0.5, 1.0, 1.0);

          {
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(0.0, -1.0, 0.0));
          }

          {
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(1.0, 0.0, 0.0));
          }

          {
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(0.0, 1.0, 0.0));
          }

          {
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(-1.0, 0.0, 0.0));
          }

          {//5
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(0.0, 0.0, 1.0));
          }

          {//6
            coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
            coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
            coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));

            tcoords->push_back(osg::Vec2d(0,0));
            tcoords->push_back(osg::Vec2d(1,0));
            tcoords->push_back(osg::Vec2d(1,1));
            tcoords->push_back(osg::Vec2d(0,1));

            colours->push_back(color);
            normals->push_back(osg::Vec3d(0.0, 0.0, -1.0));
          }

          drawArr->setCount(coords->size());
        }
  }

  return geode;
}
