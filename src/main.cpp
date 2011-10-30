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

int main( void )
{
  cube::World* world = new cube::World;

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);

  osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

  keyswitchManipulator->addMatrixManipulator( '1', "Trackball", new osgGA::TrackballManipulator() );
  keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
  keyswitchManipulator->addMatrixManipulator( '3', "Terrain", new osgGA::TerrainManipulator() );
  keyswitchManipulator->addMatrixManipulator( '4', "FirstPerson", new osgGA::FirstPersonManipulator() );

  viewer.setCameraManipulator( keyswitchManipulator.get() );

  viewer.setSceneData(world->GetGeometry());

  viewer.setUpViewInWindow(100, 100, 640, 480, 1);
  viewer.run();
}