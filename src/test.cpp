#include <osg/ArgumentParser>
#include <osg/Material>
#include <osg/StateAttribute>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <iostream>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgGA/FirstPersonManipulator>
#include <osgGA/NodeTrackerManipulator>
#include <osgGA/TrackballManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/TerrainManipulator>
#include <world.h>

#include <personManipulator.h>

#include "perlin.h"

osg::Group* GetGeometry()
{
  osg::Group* group = new osg::Group;
  osg::Geode* geode = new osg::Geode;

  group->addChild(geode);

  //******************************************
  osg::Geometry* curGeom = new osg::Geometry;
  geode->addDrawable(curGeom);
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

  cube::TextureInfo* texInfo = new cube::TextureInfo("./res/mc16-7.png", 16);

  int m[32][32][32];

  Perlin* perlin = new Perlin(1, 4, 1, 123);

  for(int x = 0; x < 32; x++)
  for(int y = 0; y < 32; y++)
  for(int z = 0; z < 32; z++)
  {
    m[x][y][z] = perlin->Get3D((float)x/100.0f, (float)y/100.0f, (float)z/100.0f) > -0.1f? 1 : 0;
  }

  m[0][0][0] = 0;

  for(int x = 0; x < 32; x++)
  for(int y = 0; y < 32; y++)
  for(int z = 0; z < 32; z++)
  {
    if(m[x][y][z] == 1)
    {
      osg::Vec3d pos = osg::Vec3d(x, y, z);

      for(int side = cube::CubInfo::FirstSide; side <= cube::CubInfo::EndSide; side++)
      {
        cube::CubInfo::CubeSide cside = (cube::CubInfo::CubeSide)side;

        cube::CubInfo::Instance().FillVertCoord(cside, coords, pos);

        texInfo->FillTexCoord(cube::Block::LeavesWood, cside, tcoords);

        osg::Vec4d color = texInfo->GetSideColor(cube::Block::LeavesWood, cside);
        colours->push_back(color);
        normals->push_back(cube::CubInfo::Instance().GetNormal(cside));
      }

      drawArr->setCount(coords->size());
    }
  }

  return group;
}

int mainz( void )
{
  cube::World& world = cube::World::Instance();

  osgViewer::Viewer viewer;
  osgViewer::StatsHandler* sh = new osgViewer::StatsHandler;
  sh ->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
  viewer.addEventHandler(sh);

  osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
  keyswitchManipulator->setAutoComputeHomePosition(false);
  keyswitchManipulator->setHomePosition(osg::Vec3d(5.0, 5.0, 100.0), osg::Vec3d(6.0, 5.0, 100.0), osg::Vec3d(0.0, 0.0, 1.0));

  keyswitchManipulator->addMatrixManipulator( '1', "Flight", new osgGA::FlightManipulator() );
  keyswitchManipulator->addMatrixManipulator( '2', "Terrain", new osgGA::TerrainManipulator() );
  keyswitchManipulator->addMatrixManipulator( '3', "Trackball", new osgGA::TrackballManipulator() );

  viewer.setCameraManipulator( keyswitchManipulator.get() );

  viewer.setSceneData(GetGeometry());

  viewer.setUpViewInWindow(100, 100, 1024, 576, 0);

   osg::Camera* cam = viewer.getCamera();

  double fovy,aspectRatio,z1,z2;
  cam->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
  //aspectRatio=double(traits->width)/double(traits->height);
  cam->setProjectionMatrixAsPerspective(70,aspectRatio,z1,z2);

  //viewer.setRunMaxFrameRate(60);
  viewer.run();
}