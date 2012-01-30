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

  osg::Vec3d v1(0.0, 0.0, 0.0);
  osg::Vec3d v2(REGION_WIDTH, 0.0, 0.0);
  osg::Vec3d v3(0.0, REGION_WIDTH, 0.0);
  osg::Vec3d v4(REGION_WIDTH, REGION_WIDTH, 0.0);
  _regions[0][0] = cube::Region::Generation(v1);
  _regions[1][0] = cube::Region::Generation(v2);
  _regions[0][1] = cube::Region::Generation(v3);
  _regions[1][1] = cube::Region::Generation(v4);
/*
  _regions.push_back(cube::Region::Generation(osg::Vec3d(-REGION_WIDTH, 0.0, 0.0)));
  _regions.push_back(cube::Region::Generation(osg::Vec3d(0.0, -REGION_WIDTH, 0.0)));
  _regions.push_back(cube::Region::Generation(osg::Vec3d(-REGION_WIDTH, -REGION_WIDTH, 0.0)));

  _regions.push_back(cube::Region::Generation(osg::Vec3d(REGION_WIDTH, -REGION_WIDTH, 0.0)));
  _regions.push_back(cube::Region::Generation(osg::Vec3d(-REGION_WIDTH, REGION_WIDTH, 0.0)));
*/

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

  //osg::Geode *geode = new osg::Geode();
  //osg::Geometry* geom = createGeometry();

  _geode = createGeometry2();
  _group->addChild(_geode);
  //geode->addDrawable(geom);

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

      for(int x = 0; x < GEOM_DEVIDER_SIZE; x++)
      for(int y = 0; y < GEOM_DEVIDER_SIZE; y++)
        for(int z = 0; z < GEOM_DEVIDER_SIZE; z++)
        {
          const cube::Cub &cub = _dataUpdate[i]._reg->GetCub(x + _dataUpdate[i]._xCubOff, y + _dataUpdate[i]._yCubOff, z + _dataUpdate[i]._zCubOff);

          if(cub._type == cube::Cub::Air || !cub._rendered) //!!!!!!!
            continue;

          osg::Vec3d pos = _dataUpdate[i]._reg->GetPosition()
            + osg::Vec3d( (x + _dataUpdate[i]._xCubOff) * CUBE_SIZE,
                          (y + _dataUpdate[i]._yCubOff) * CUBE_SIZE,
                          (z + _dataUpdate[i]._zCubOff) * CUBE_SIZE);

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

cube::Region* World::GetRegion(float x, float y)
{
  int i = x / REGION_SIZE;
  int j = y / REGION_SIZE;

  return _regions[i][j];
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
  vec /= GEOM_DEVIDER_SIZE;
  osg::Geometry* curGeom = reg->GetGeometry(vec.x(), vec.y(), vec.z());

  if(curGeom == NULL)
  {
    curGeom = NewOSGGeom();
    reg->SetGeometry(vec.x(), vec.y(), vec.z(), curGeom);

    _geode->addDrawable(curGeom);
  }

  _dataUpdate.push_back(DataUpdate(curGeom, reg, vec));
}

osg::Geometry* World::createGeometry()
{
  osg::Geometry* geom = new osg::Geometry;

  osg::Vec3Array* coords = new osg::Vec3Array();
  osg::Vec4Array* colours = new osg::Vec4Array();
  osg::Vec3Array* normals = new osg::Vec3Array();
  int numQuads = 0;

  RegionsContainer::iterator xrg;
  YRegionsContainer::iterator yrg;
  for(xrg = _regions.begin(); xrg != _regions.end(); xrg++)
    for(yrg = xrg->second.begin(); yrg != xrg->second.end(); yrg++)
  {
    cube::Region* rg = yrg->second;

    for(int x = 0; x < REGION_SIZE; x++)
    for(int y = 0; y < REGION_SIZE; y++)
    for(int z = 0; z < REGION_SIZE; z++)
    {
      const cube::Cub &cub = rg->GetCub(x, y, z);

      if(cub._type == cube::Cub::Ground) //!!!!!!!
        continue;

      osg::Vec3d pos = rg->GetPosition() + osg::Vec3d(x * CUBE_SIZE, y * CUBE_SIZE, z * CUBE_SIZE);

      coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));
      coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
      coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));
      coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));

      osg::Vec4d color;

      if(cub._type == cube::Cub::Ground)
        color = osg::Vec4d(0.5, 0.25, 0.0, 1.0);
      else if(cub._type == cube::Cub::Air)
        color = osg::Vec4d(0.0, 0.5, 1.0, 1.0);

      colours->push_back(color);
      colours->push_back(color);
      colours->push_back(color);
      colours->push_back(color);

      normals->push_back(osg::Vec3d(0.0, -1.0, 0.0));

      numQuads++;
    }
  }

  geom->setVertexArray(coords);
  geom->setColorArray(colours);
  geom->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
  geom->setNormalArray(normals);
  geom->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4 * numQuads));

  return geom;
}

osg::Geode* World::createGeometry2()
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

    for(int x = 0; x < REGION_SIZE; x++)
      for(int y = 0; y < REGION_SIZE; y++)
        for(int z = 0; z < REGION_SIZE; z++)
        {
          const cube::Cub &cub = rg->GetCub(x, y, z);

          if(cub._type == cube::Cub::Air || !cub._rendered) //!!!!!!!
            continue;


          osg::Vec3d pos = rg->GetPosition() + osg::Vec3d(x * CUBE_SIZE, y * CUBE_SIZE, z * CUBE_SIZE);

          osg::Geometry* curGeom = rg->GetGeometry(x / GEOM_DEVIDER_SIZE, y / GEOM_DEVIDER_SIZE, z / GEOM_DEVIDER_SIZE);

          if(curGeom == NULL)
          {
            curGeom = NewOSGGeom();
            rg->SetGeometry(x / GEOM_DEVIDER_SIZE, y / GEOM_DEVIDER_SIZE, z / GEOM_DEVIDER_SIZE, curGeom);

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
