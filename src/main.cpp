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
#include "geoMaker.h"

#include <personManipulator.h>

int main( void )
{
  cube::World& world = cube::World::Instance();

  osgViewer::Viewer viewer;
  osgViewer::StatsHandler* sh = new osgViewer::StatsHandler;
  sh ->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
  viewer.addEventHandler(sh);

  osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
  keyswitchManipulator->setAutoComputeHomePosition(false);
  keyswitchManipulator->setHomePosition(osg::Vec3d(5.0, 5.0, 10.0), osg::Vec3d(6.0, 6.0, 10.0), osg::Vec3d(0.0, 0.0, 1.0));

  keyswitchManipulator->addMatrixManipulator( '1', "FirstPerson", new cube::PersonManipulator() );
  keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
  keyswitchManipulator->addMatrixManipulator( '3', "Terrain", new osgGA::TerrainManipulator() );
  keyswitchManipulator->addMatrixManipulator( '4', "Trackball", new osgGA::TrackballManipulator() );

  viewer.setCameraManipulator( keyswitchManipulator.get() );

  viewer.setSceneData(world.GetGeometry());

  viewer.setUpViewInWindow(100, 100, 640, 480, 1);
  viewer.run();
}