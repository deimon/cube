#include <light.h>
#include <world.h>
#include <regionManager.h>

using namespace cube;

void AddModifiedGeom(cube::CubRegion& cubReg, osg::Vec3d wcpos, std::map<osg::Geometry*, World::DataUpdate>* updateGeomMap)
{
  cube::Region* reg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(wcpos.x()), Region::ToRegionIndex(wcpos.y()));
  wcpos -= reg->GetPosition();
  int geomIndex = wcpos.z() / GEOM_SIZE;
  osg::Geometry* geom = reg->GetGeometry(geomIndex);

  if(updateGeomMap->find(geom) == updateGeomMap->end())
    (*updateGeomMap)[geom] = World::DataUpdate(geom, reg, geomIndex, cubReg.GetCubBlend());

  int posX = wcpos.x();
  int posY = wcpos.y();
  int posZ = wcpos.z();

  if(posX == 0)
  {
    cube::Region* sreg = RegionManager::Instance().ContainsRegion(reg->GetX() - 1, reg->GetY());
    geom = sreg->GetGeometry(geomIndex);

    if(updateGeomMap->find(geom) == updateGeomMap->end())
      (*updateGeomMap)[geom] = World::DataUpdate(geom, sreg, geomIndex, cubReg.GetCubBlend());
  }

  if(posX == REGION_WIDTH - 1)
  {
    cube::Region* sreg = RegionManager::Instance().ContainsRegion(reg->GetX() + 1, reg->GetY());
    geom = sreg->GetGeometry(geomIndex);

    if(updateGeomMap->find(geom) == updateGeomMap->end())
      (*updateGeomMap)[geom] = World::DataUpdate(geom, sreg, geomIndex, cubReg.GetCubBlend());
  }

  if(posY == 0)
  {
    cube::Region* sreg = RegionManager::Instance().ContainsRegion(reg->GetX(), reg->GetY() - 1);
    geom = sreg->GetGeometry(geomIndex);

    if(updateGeomMap->find(geom) == updateGeomMap->end())
      (*updateGeomMap)[geom] = World::DataUpdate(geom, sreg, geomIndex, cubReg.GetCubBlend());
  }

  if(posY == REGION_WIDTH - 1)
  {
    cube::Region* sreg = RegionManager::Instance().ContainsRegion(reg->GetX(), reg->GetY() + 1);
    geom = sreg->GetGeometry(geomIndex);

    if(updateGeomMap->find(geom) == updateGeomMap->end())
      (*updateGeomMap)[geom] = World::DataUpdate(geom, sreg, geomIndex, cubReg.GetCubBlend());
  }

  if(posZ % REGION_WIDTH == 0)
  {
    if(geomIndex > 0)
    {
      geom = reg->GetGeometry(geomIndex - 1);

      if(updateGeomMap->find(geom) == updateGeomMap->end())
        (*updateGeomMap)[geom] = World::DataUpdate(geom, reg, geomIndex - 1, cubReg.GetCubBlend());
    }
  }

  if((posZ + 1) % REGION_WIDTH == 0)
  {
    if(geomIndex + 1 < GEOM_COUNT)
    {
      geom = reg->GetGeometry(geomIndex + 1);

      if(updateGeomMap->find(geom) == updateGeomMap->end())
        (*updateGeomMap)[geom] = World::DataUpdate(geom, reg, geomIndex + 1, cubReg.GetCubBlend());
    }
  }
}

void Light::RecalcAndFillingLight(cube::CubRegion& cubReg, osg::Vec3d wcpos, std::map<osg::Geometry*, World::DataUpdate>* updateGeomMap)
{
  float ZFaceLight = 0.0f;
  float oldCubLight = cubReg.GetCubLight();
  cubReg.GetCubLight() = 0.0f;
  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    osg::Vec3d csvec = wcpos + CubInfo::Instance().GetNormal(side);

    cube::Region* sideReg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(csvec.x()), Region::ToRegionIndex(csvec.y()));
    csvec -= sideReg->GetPosition();
    cube::CubRegion scubReg = sideReg->GetCub(csvec.x(), csvec.y(), csvec.z());

    if(scubReg.GetCubType() == cube::Cub::Air)
    {
      if(scubReg.GetCubLight() > cubReg.GetCubLight())
        cubReg.GetCubLight() = scubReg.GetCubLight();

      if(side == CubInfo::Z_FACE)
        ZFaceLight = scubReg.GetCubLight();
    }
  }

  if(fabs(ZFaceLight - cubReg.GetCubLight()) > 0.02f && cubReg.GetCubLight() > 0.12f)
    cubReg.GetCubLight() -= 0.1f;

  if(fabs(oldCubLight - cubReg.GetCubLight()) < 0.02f)
    return;

  if(updateGeomMap)
    AddModifiedGeom(cubReg, wcpos, updateGeomMap);

  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    osg::Vec3d csvec = wcpos + CubInfo::Instance().GetNormal(side);
    cube::CubRegion scubReg = RegionManager::Instance().GetCub(csvec.x(), csvec.y(), csvec.z());

    fillingLight(scubReg, csvec, side, cubReg.GetCubLight(), updateGeomMap);
  }
}

void Light::fillingLight(cube::CubRegion& cubReg, osg::Vec3d wcpos, CubInfo::CubeSide side, float prevLight,
                         std::map<osg::Geometry*, World::DataUpdate>* updateGeomMap)
{
  if(cubReg.GetCubType() == cube::Cub::Air)
  {
    bool next = false;
    if(side == CubInfo::Z_BACK && (prevLight - cubReg.GetCubLight()) > 0.02f)
    {
      next = true;
      cubReg.GetCubLight() = prevLight;
    }
    else if((prevLight - cubReg.GetCubLight()) > 0.12f)
    {
      next = true;
      cubReg.GetCubLight() = prevLight - 0.1f;
    }

    if(next)
    {
      if(updateGeomMap)
        AddModifiedGeom(cubReg, wcpos, updateGeomMap);

      for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
      {
        CubInfo::CubeSide lside = (CubInfo::CubeSide)i;
        osg::Vec3d csvec = wcpos + CubInfo::Instance().GetNormal(lside);
        cube::CubRegion scubReg = RegionManager::Instance().GetCub(csvec.x(), csvec.y(), csvec.z());

        fillingLight(scubReg, csvec, lside, cubReg.GetCubLight(), updateGeomMap);
      }
    }
  }
}

void Light::fillingLocLight(cube::CubRegion& cubReg, osg::Vec3d wcpos, float prevLight,
                            std::map<osg::Geometry*, World::DataUpdate>* updateGeomMap)
{
  if(cubReg.GetCubLocLight() < prevLight)
  {
    cubReg.GetCubLocLight() = prevLight;

    if(updateGeomMap)
      AddModifiedGeom(cubReg, wcpos, updateGeomMap);

    if(prevLight > 0.12f)
    {
      for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
      {
        CubInfo::CubeSide lside = (CubInfo::CubeSide)i;
        osg::Vec3d csvec = wcpos + CubInfo::Instance().GetNormal(lside);
        cube::CubRegion scubReg = RegionManager::Instance().GetCub(csvec.x(), csvec.y(), csvec.z());

        fillingLocLight(scubReg, csvec, prevLight - 0.1f, updateGeomMap);
      }
    }
  }
}

void Light::FindLightSourceAndFillingLight(cube::CubRegion& cubReg, osg::Vec3d wcpos, std::map<osg::Geometry*,
                                           World::DataUpdate>* updateGeomMap)
{
  Light::MapCubPos listCubPos;

  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    osg::Vec3d wcsvec = wcpos + CubInfo::Instance().GetNormal(side);
    osg::Vec3d csvec = wcsvec;

    cube::Region* sideReg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(csvec.x()), Region::ToRegionIndex(csvec.y()));
    csvec -= sideReg->GetPosition();
    cube::CubRegion scubReg = sideReg->GetCub(csvec.x(), csvec.y(), csvec.z());

    if(scubReg.GetCubType() == cube::Cub::Air && ((cubReg.GetCubLight() - scubReg.GetCubLight()) > 0.02f || side == CubInfo::Z_BACK))
    {
      //найти увеличение и заполнить свет оттуда
      findLightSource(scubReg, wcsvec, listCubPos, updateGeomMap);
    }
  }

  Light::MapCubPos listLightSource;
  Light::MapCubPos::iterator i = listCubPos.begin();
  for(; i != listCubPos.end(); i++)
    if(*(i->second.second) > 0.12f)  // i->second.second = cubLight
      listLightSource[i->first] = i->second;

  i = listLightSource.begin();
  for(; i != listLightSource.end(); i++)
  {
    for(int s = CubInfo::FirstSide; s <= CubInfo::EndSide; s++)
    {
      CubInfo::CubeSide side = (CubInfo::CubeSide)s;
      osg::Vec3d csvec = i->second.first + CubInfo::Instance().GetNormal(side);
      cube::CubRegion scubReg = RegionManager::Instance().GetCub(csvec.x(), csvec.y(), csvec.z());

      cube::Light::fillingLight(scubReg, csvec, side, *(i->second.second), updateGeomMap); // i->second.second = cubLight
    }
  }
}

void Light::findLightSource(cube::CubRegion& cubReg, osg::Vec3d wcpos, MapCubPos& listCubPos, 
                            std::map<osg::Geometry*, World::DataUpdate>* updateGeomMap)
{
  float cubLight = cubReg.GetCubLight();
  cubReg.GetCubLight() = 0.1f;

  if(updateGeomMap)
    AddModifiedGeom(cubReg, wcpos, updateGeomMap);

  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    osg::Vec3d wcsvec = wcpos + CubInfo::Instance().GetNormal(side);
    osg::Vec3d csvec = wcsvec;

    cube::Region* sideReg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(csvec.x()), Region::ToRegionIndex(csvec.y()));
    csvec -= sideReg->GetPosition();
    cube::CubRegion scubReg = sideReg->GetCub(csvec.x(), csvec.y(), csvec.z());

    if(scubReg.GetCubType() == cube::Cub::Air && scubReg.GetCubLight() > 0.12)
    {
      if((scubReg.GetCubLight() - cubLight) > 0.02f || (scubReg.GetCubLight() > 0.98 && side != CubInfo::Z_BACK) /*&& listCubPos.find(&scub) == listCubPos.end()*/)
        listCubPos[scubReg.GetCub()] = std::make_pair(wcsvec, &scubReg.GetCubLight());
      else
        findLightSource(scubReg, wcsvec, listCubPos, updateGeomMap);
    }
  }
}