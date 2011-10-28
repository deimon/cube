#ifndef __WORLD_H__
#define __WORLD_H__

#include "region.h"
#include <osg/NodeCallback>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/Geode>

namespace cube
{
  class World;

  class WorldCallback
    : public osg::NodeCallback
  {
  public:
    WorldCallback(cube::World *world): _world(world) {};
    virtual void operator()(osg::Node* node, osg::NodeVisitor* nv);

  private:
    cube::World *_world;
  };

  class World
  {
  public:
    typedef std::vector<cube::Region*> RegionsContainer;

    World()
    {
      srand(time(NULL));

      _regions.push_back(cube::Region::Generation(osg::Vec3d(0.0, 0.0, 0.0)));
      _regions.push_back(cube::Region::Generation(osg::Vec3d(REGION_WIDTH, 0.0, 0.0)));
      _regions.push_back(cube::Region::Generation(osg::Vec3d(0.0, REGION_WIDTH, 0.0)));
      _regions.push_back(cube::Region::Generation(osg::Vec3d(REGION_WIDTH, REGION_WIDTH, 0.0)));
/*
      _regions.push_back(cube::Region::Generation(osg::Vec3d(-REGION_WIDTH, 0.0, 0.0)));
      _regions.push_back(cube::Region::Generation(osg::Vec3d(0.0, -REGION_WIDTH, 0.0)));
      _regions.push_back(cube::Region::Generation(osg::Vec3d(-REGION_WIDTH, -REGION_WIDTH, 0.0)));

      _regions.push_back(cube::Region::Generation(osg::Vec3d(REGION_WIDTH, -REGION_WIDTH, 0.0)));
      _regions.push_back(cube::Region::Generation(osg::Vec3d(-REGION_WIDTH, REGION_WIDTH, 0.0)));
*/
    }

    ~World()
    {
    }

    osg::Group* GetGeometry()
    {
      _group = new osg::Group;

      //_group->setUpdateCallback(new WorldCallback(this));
      update();

      return _group;
    }

    void update()
    {
      _group->removeChildren(0, _group->getNumChildren());

      //osg::Geode *geode = new osg::Geode();
      //osg::Geometry* geom = createGeometry();

      osg::Geode *geode = createGeometry2();
      _group->addChild(geode);
      //geode->addDrawable(geom);

      geode->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE | osg::StateAttribute::PROTECTED);
    }

  protected:
    osg::Geometry* createGeometry()
    {
      osg::Geometry* geom = new osg::Geometry;

      osg::Vec3Array* coords = new osg::Vec3Array();
      osg::Vec4Array* colours = new osg::Vec4Array();
      osg::Vec3Array* normals = new osg::Vec3Array();
      int numQuads = 0;

      RegionsContainer::iterator rg;
      for(rg = _regions.begin(); rg != _regions.end(); rg++)
      {
        for(int x = 0; x < REGION_SIZE; x++)
        for(int y = 0; y < REGION_SIZE; y++)
        for(int z = 0; z < REGION_SIZE; z++)
        {
          const cube::Cub &cub = (*rg)->GetCub(x, y, z);

          if(cub._type == cube::Cub::Ground) //!!!!!!!
            continue;

          osg::Vec3d pos = (*rg)->GetPosition() + osg::Vec3d(x * CUBE_SIZE, y * CUBE_SIZE, z * CUBE_SIZE);

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

    osg::Geode* createGeometry2()
    {
      osg::Geode* geode = new osg::Geode;

      RegionsContainer::iterator rg;
      for(rg = _regions.begin(); rg != _regions.end(); rg++)
      {
        osg::Geometry* geom[GEOM_SIZE][GEOM_SIZE][GEOM_SIZE];
        for(int x = 0; x < GEOM_SIZE; x++)
          for(int y = 0; y < GEOM_SIZE; y++)
            for(int z = 0; z < GEOM_SIZE; z++)
            {
              geom[x][y][z] = NULL;
            }

        osg::Vec3Array* coords;
        osg::Vec4Array* colours;
        osg::Vec3Array* normals;

        osg::DrawArrays* drawArr;

        for(int x = 0; x < REGION_SIZE; x++)
          for(int y = 0; y < REGION_SIZE; y++)
            for(int z = 0; z < REGION_SIZE; z++)
            {
              const cube::Cub &cub = (*rg)->GetCub(x, y, z);

              if(cub._type == cube::Cub::Air) //!!!!!!!
                continue;

              osg::Vec3d pos = (*rg)->GetPosition() + osg::Vec3d(x * CUBE_SIZE, y * CUBE_SIZE, z * CUBE_SIZE);

              osg::Geometry* curGeom = geom[x / GEOM_DEVIDER_SIZE][y / GEOM_DEVIDER_SIZE][z / GEOM_DEVIDER_SIZE];

              if(curGeom == NULL)
              {
                geom[x / GEOM_DEVIDER_SIZE][y / GEOM_DEVIDER_SIZE][z / GEOM_DEVIDER_SIZE] = new osg::Geometry;
                curGeom = geom[x / GEOM_DEVIDER_SIZE][y / GEOM_DEVIDER_SIZE][z / GEOM_DEVIDER_SIZE];
                geode->addDrawable(curGeom);

                coords = new osg::Vec3Array();
                colours = new osg::Vec4Array();
                normals = new osg::Vec3Array();
                drawArr = new osg::DrawArrays(osg::PrimitiveSet::QUADS, 0, 4);

                curGeom->setVertexArray(coords);
                curGeom->setColorArray(colours);
                curGeom->setColorBinding(osg::Geometry::BIND_PER_PRIMITIVE);
                curGeom->setNormalArray(normals);
                curGeom->setNormalBinding(osg::Geometry::BIND_PER_PRIMITIVE);

                curGeom->addPrimitiveSet(drawArr);
              }
              else
              {
                coords = dynamic_cast<osg::Vec3Array*>(curGeom->getVertexArray());
                colours = dynamic_cast<osg::Vec4Array*>(curGeom->getColorArray());
                normals = dynamic_cast<osg::Vec3Array*>(curGeom->getNormalArray());

                drawArr = dynamic_cast<osg::DrawArrays*>(curGeom->getPrimitiveSet(0));
              }

              osg::Vec4d color;

              if(cub._type == cube::Cub::Ground)
                color = osg::Vec4d(0.5, 0.25, 0.0, 1.0);
              else if(cub._type == cube::Cub::Air)
                color = osg::Vec4d(0.0, 0.5, 1.0, 1.0);

              {
                coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));
                coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
                coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));
                coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));

                colours->push_back(color);
                normals->push_back(osg::Vec3d(0.0, -1.0, 0.0));
              }

              {
                coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
                coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
                coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));
                coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));

                colours->push_back(color);
                normals->push_back(osg::Vec3d(1.0, 0.0, 0.0));
              }

              {
                coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
                coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
                coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));
                coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));

                colours->push_back(color);
                normals->push_back(osg::Vec3d(0.0, 1.0, 0.0));
              }

              {
                coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
                coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));
                coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));
                coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));

                colours->push_back(color);
                normals->push_back(osg::Vec3d(-1.0, 0.0, 0.0));
              }

              {//5
                coords->push_back(pos + osg::Vec3d(0.0, 0.0, 1.0));
                coords->push_back(pos + osg::Vec3d(1.0, 0.0, 1.0));
                coords->push_back(pos + osg::Vec3d(1.0, 1.0, 1.0));
                coords->push_back(pos + osg::Vec3d(0.0, 1.0, 1.0));

                colours->push_back(color);
                normals->push_back(osg::Vec3d(0.0, 0.0, 1.0));
              }

              {//6
                coords->push_back(pos + osg::Vec3d(0.0, 1.0, 0.0));
                coords->push_back(pos + osg::Vec3d(1.0, 1.0, 0.0));
                coords->push_back(pos + osg::Vec3d(1.0, 0.0, 0.0));
                coords->push_back(pos + osg::Vec3d(0.0, 0.0, 0.0));

                colours->push_back(color);
                normals->push_back(osg::Vec3d(0.0, 0.0, -1.0));
              }

              drawArr->setCount(coords->size());
            }
      }

      return geode;
    }

    osg::Group* _group;

  public:
    RegionsContainer _regions;
  };
}

#endif 