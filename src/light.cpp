#include <light.h>
#include <world.h>

using namespace cube;

void AddModifiedGeom(cube::Cub& cub, osg::Vec3d wcpos, std::map<osg::Geometry*, World::DataUpdate>& updateGeomMap)
{
  cube::Region* reg = cube::World::Instance().GetRegion(Region::ToRegionIndex(wcpos.x()), Region::ToRegionIndex(wcpos.y()));
  wcpos -= reg->GetPosition();
  int geomIndex = wcpos.z() / GEOM_SIZE;
  osg::Geometry* geom = reg->GetGeometry(geomIndex);

  if(updateGeomMap.find(geom) == updateGeomMap.end())
    updateGeomMap[geom] = World::DataUpdate(geom, reg, geomIndex, cub._blend);

  int posX = wcpos.x();
  int posY = wcpos.y();
  int posZ = wcpos.z();

  if(posX == 0)
  {
    cube::Region* sreg = cube::World::Instance().ContainsRegion(reg->GetX() - 1, reg->GetY());
    geom = sreg->GetGeometry(geomIndex);

    if(updateGeomMap.find(geom) == updateGeomMap.end())
      updateGeomMap[geom] = World::DataUpdate(geom, sreg, geomIndex, cub._blend);
  }

  if(posX == REGION_WIDTH - 1)
  {
    cube::Region* sreg = cube::World::Instance().ContainsRegion(reg->GetX() + 1, reg->GetY());
    geom = sreg->GetGeometry(geomIndex);

    if(updateGeomMap.find(geom) == updateGeomMap.end())
      updateGeomMap[geom] = World::DataUpdate(geom, sreg, geomIndex, cub._blend);
  }

  if(posY == 0)
  {
    cube::Region* sreg = cube::World::Instance().ContainsRegion(reg->GetX(), reg->GetY() - 1);
    geom = sreg->GetGeometry(geomIndex);

    if(updateGeomMap.find(geom) == updateGeomMap.end())
      updateGeomMap[geom] = World::DataUpdate(geom, sreg, geomIndex, cub._blend);
  }

  if(posY == REGION_WIDTH - 1)
  {
    cube::Region* sreg = cube::World::Instance().ContainsRegion(reg->GetX(), reg->GetY() + 1);
    geom = sreg->GetGeometry(geomIndex);

    if(updateGeomMap.find(geom) == updateGeomMap.end())
      updateGeomMap[geom] = World::DataUpdate(geom, sreg, geomIndex, cub._blend);
  }

  if(posZ % REGION_WIDTH == 0)
  {
    if(geomIndex > 0)
    {
      geom = reg->GetGeometry(geomIndex - 1);

      if(updateGeomMap.find(geom) == updateGeomMap.end())
        updateGeomMap[geom] = World::DataUpdate(geom, reg, geomIndex - 1, cub._blend);
    }
  }

  if((posZ + 1) % REGION_WIDTH == 0)
  {
    if(geomIndex + 1 < GEOM_COUNT)
    {
      geom = reg->GetGeometry(geomIndex + 1);

      if(updateGeomMap.find(geom) == updateGeomMap.end())
        updateGeomMap[geom] = World::DataUpdate(geom, reg, geomIndex + 1, cub._blend);
    }
  }
}

void Light::RecalcAndFillingLight(cube::Cub& cub, osg::Vec3d wcpos, std::map<osg::Geometry*, World::DataUpdate>& updateGeomMap)
{
  cub._light = 0.0f;
  CubInfo::CubeSide maxLightSide = CubInfo::FirstSide;
  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    osg::Vec3d csvec = wcpos + CubInfo::Instance().GetNormal(side);

    cube::Region* sideReg = cube::World::Instance().GetRegion(Region::ToRegionIndex(csvec.x()), Region::ToRegionIndex(csvec.y()));
    csvec -= sideReg->GetPosition();
    cube::Cub& scub = sideReg->GetCub(csvec.x(), csvec.y(), csvec.z());

    if(scub._type == cube::Cub::Air && scub._light > cub._light)
    {
      cub._light = scub._light;
      maxLightSide = side;
    }
  }

  if(maxLightSide != CubInfo::Z_FACE && cub._light > 0.12f)
    cub._light -= 0.1f;

  AddModifiedGeom(cub, wcpos, updateGeomMap);

  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    osg::Vec3d csvec = wcpos + CubInfo::Instance().GetNormal(side);
    cube::Cub* scub = cube::World::Instance().GetCub(csvec.x(), csvec.y(), csvec.z());

    fillingLight(scub, csvec, side, cub._light, updateGeomMap);
  }
}

void Light::fillingLight(cube::Cub* cub, osg::Vec3d wcpos, CubInfo::CubeSide side, float prevLight,
                         std::map<osg::Geometry*, World::DataUpdate>& updateGeomMap)
{
  if(cub->_type == cube::Cub::Air)
  {
    bool next = false;
    if(side == CubInfo::Z_BACK && (prevLight - cub->_light) > 0.02f)
    {
      next = true;
      cub->_light = prevLight;
    }
    else if((prevLight - cub->_light) > 0.12f)
    {
      next = true;
      cub->_light = prevLight - 0.1f;
    }

    if(next)
    {
      AddModifiedGeom(*cub, wcpos, updateGeomMap);

      for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
      {
        CubInfo::CubeSide lside = (CubInfo::CubeSide)i;
        osg::Vec3d csvec = wcpos + CubInfo::Instance().GetNormal(lside);
        cube::Cub* scub = cube::World::Instance().GetCub(csvec.x(), csvec.y(), csvec.z());

        fillingLight(scub, csvec, lside, cub->_light, updateGeomMap);
      }
    }
  }
}

void Light::FindLightSourceAndFillingLight(cube::Cub& cub, osg::Vec3d wcpos, std::map<osg::Geometry*,
                                           World::DataUpdate>& updateGeomMap)
{
  Light::MapCubPos listCubPos;

  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    osg::Vec3d wcsvec = wcpos + CubInfo::Instance().GetNormal(side);
    osg::Vec3d csvec = wcsvec;

    cube::Region* sideReg = cube::World::Instance().GetRegion(Region::ToRegionIndex(csvec.x()), Region::ToRegionIndex(csvec.y()));
    csvec -= sideReg->GetPosition();
    cube::Cub& scub = sideReg->GetCub(csvec.x(), csvec.y(), csvec.z());

    if(scub._type == cube::Cub::Air && ((cub._light - scub._light) > 0.02f || side == CubInfo::Z_BACK))
    {
      //найти увеличение и заполнить свет оттуда
      findLightSource(scub, wcsvec, listCubPos, updateGeomMap);
    }
  }

  Light::MapCubPos listLightSource;
  Light::MapCubPos::iterator i = listCubPos.begin();
  for(; i != listCubPos.end(); i++)
    if(i->first->_light > 0.12f)
      listLightSource[i->first] = i->second;

  i = listLightSource.begin();
  for(; i != listLightSource.end(); i++)
  {
    for(int s = CubInfo::FirstSide; s <= CubInfo::EndSide; s++)
    {
      CubInfo::CubeSide side = (CubInfo::CubeSide)s;
      osg::Vec3d csvec = i->second + CubInfo::Instance().GetNormal(side);
      cube::Cub* scub = cube::World::Instance().GetCub(csvec.x(), csvec.y(), csvec.z());

      cube::Light::fillingLight(scub, csvec, side, i->first->_light, updateGeomMap);
    }
  }
}

void Light::findLightSource(cube::Cub& cub, osg::Vec3d wcpos, MapCubPos& listCubPos, 
                            std::map<osg::Geometry*, World::DataUpdate>& updateGeomMap)
{
  float cubLight = cub._light;
  cub._light = 0.1f;

  AddModifiedGeom(cub, wcpos, updateGeomMap);

  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    osg::Vec3d wcsvec = wcpos + CubInfo::Instance().GetNormal(side);
    osg::Vec3d csvec = wcsvec;

    cube::Region* sideReg = cube::World::Instance().GetRegion(Region::ToRegionIndex(csvec.x()), Region::ToRegionIndex(csvec.y()));
    csvec -= sideReg->GetPosition();
    cube::Cub& scub = sideReg->GetCub(csvec.x(), csvec.y(), csvec.z());

    if(scub._type == cube::Cub::Air && scub._light > 0.12)
    {
      if((scub._light - cubLight) > 0.02f || (scub._light > 0.98 && side != CubInfo::Z_BACK) /*&& listCubPos.find(&scub) == listCubPos.end()*/)
        listCubPos[&scub].set(wcsvec);
      else
        findLightSource(scub, wcsvec, listCubPos, updateGeomMap);
    }
  }
}