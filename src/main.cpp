#include <osg/ArgumentParser>
#include <osg/Material>
#include <osg/StateAttribute>

#include <osgDB/ReadFile>
#include <osgDB/WriteFile>

#include <iostream>
#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include "geoMaker.h"

int main( void )
{
  cube::World* world = new cube::World;

  osgViewer::Viewer viewer;
  viewer.addEventHandler(new osgViewer::StatsHandler);

  viewer.setSceneData(world->GetGeometry());

  viewer.setUpViewInWindow(100, 100, 640, 480, 1);
  viewer.run();
}