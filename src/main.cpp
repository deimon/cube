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

int main( void )
{
  SYSTEMTIME sm;
  GetSystemTime(&sm);
  std::cout << "START: " << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << std::endl;

  cube::World& world = cube::World::Instance();

  osgViewer::Viewer viewer;
  viewer.setThreadingModel(osgViewer::Viewer::SingleThreaded);
  viewer.setThreadSafeReferenceCounting(true);

  osgViewer::StatsHandler* sh = new osgViewer::StatsHandler;
  sh ->setKeyEventTogglesOnScreenStats(osgGA::GUIEventAdapter::KEY_F1);
  viewer.addEventHandler(sh);

  osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;
  keyswitchManipulator->setAutoComputeHomePosition(false);
  keyswitchManipulator->setHomePosition(osg::Vec3d(5.0, 5.0, 100.0), osg::Vec3d(6.0, 5.0, 100.0), osg::Vec3d(0.0, 0.0, 1.0));

  keyswitchManipulator->addMatrixManipulator( '1', "FirstPerson", new cube::PersonManipulator() );
  keyswitchManipulator->addMatrixManipulator( '2', "Flight", new osgGA::FlightManipulator() );
  keyswitchManipulator->addMatrixManipulator( '3', "Terrain", new osgGA::TerrainManipulator() );
  keyswitchManipulator->addMatrixManipulator( '4', "Trackball", new osgGA::TrackballManipulator() );

  viewer.setCameraManipulator( keyswitchManipulator.get() );

  viewer.setSceneData(world.GetGeometry());

  viewer.setUpViewInWindow(100, 100, 1024, 576, 0);

  osgViewer::ViewerBase::Windows windows;
  viewer.getWindows(windows);

  for (osgViewer::ViewerBase::Windows::iterator itr = windows.begin(); itr != windows.end(); ++itr)
  {
      (*itr)->useCursor(false);
      //(*itr)->setCursor(osgViewer::GraphicsWindow::NoCursor);
  }

  osg::Camera* cam = viewer.getCamera();

  double fovy,aspectRatio,z1,z2;
  cam->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
  //aspectRatio=double(traits->width)/double(traits->height);
  cam->setProjectionMatrixAsPerspective(70,aspectRatio,z1,z2);

  GetSystemTime(&sm);
  std::cout << "END: " << sm.wMinute << ":" << sm.wSecond << ":" << sm.wMilliseconds << std::endl;
  //viewer.setRunMaxFrameRate(60);
  viewer.run();
}