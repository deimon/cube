#include <gridUtils.h>
#include <regionManager.h>
#include <mathUtils.h>
#include <light.h>

using namespace cube;

void GridUtils::RemoveCub(osg::Vec3d vec, RenderGroup::DataUpdateContainer* updateGeomMap)
{
  int geomIndex = vec.z() / GEOM_SIZE;

  if(geomIndex < 0 || geomIndex > GEOM_COUNT)
    return;

  cube::Region* reg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(vec.x()), Region::ToRegionIndex(vec.y()));
  osg::Vec3d cvec = vec - reg->GetPosition();
  cube::CubRegion cubReg = reg->GetCub(cvec.x(), cvec.y(), cvec.z());

  del(cubReg, vec, updateGeomMap);

  for(int i = CubInfo::FirstSide; i <= CubInfo::EndSide; i++)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)i;
    osg::Vec3d wcvec = vec + CubInfo::Instance().GetNormal(side);
    cvec = wcvec;

    cube::Region* sideReg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(cvec.x()), Region::ToRegionIndex(cvec.y()));
    cvec -= sideReg->GetPosition();
    cube::CubRegion scubReg = sideReg->GetCub(cvec.x(), cvec.y(), cvec.z());

    int geomSideIndex = cvec.z() / GEOM_SIZE;

    if(scubReg.GetCubType() != cube::Block::Air && 
      (!scubReg.GetCubRendered() || reg != sideReg || geomIndex != geomSideIndex || cubReg.GetCubBlend() != scubReg.GetCubBlend()))
    {
      add(scubReg, wcvec, false, updateGeomMap);
    }
  }
}

void GridUtils::AddCub(osg::Vec3d vec, Block::BlockType cubeType, RenderGroup::DataUpdateContainer* updateGeomMap)
{
  cube::Region* reg = RegionManager::Instance().GetRegion(Region::ToRegionIndex(vec.x()), Region::ToRegionIndex(vec.y()));
  osg::Vec3d cvec = vec - reg->GetPosition();
  cube::CubRegion scubReg = reg->GetCub(cvec.x(), cvec.y(), cvec.z());

  if(scubReg.GetCubType() == cube::Block::Air)
  {
    scubReg.SetCubType(cubeType);
    if(cubeType == cube::Block::Water)
    {
      scubReg.SetCubBlend(true);
    }
    else
    {
      //scubReg.GetCubLight() = 0.1f;

      if(scubReg.GetCubType() == cube::Block::Pumpkin)
      {
        cube::Light::fillingLocLight(scubReg, vec, 1.0f, updateGeomMap);
      }
    }

    add(scubReg, vec, true, updateGeomMap);
  }
}

void GridUtils::del(cube::CubRegion& cubReg, osg::Vec3d wcpos, RenderGroup::DataUpdateContainer* updateGeomMap)
{
  cubReg.SetCubType(cube::Block::Air);
  cubReg.SetCubRendered(false);

  bool blend = cubReg.GetCubBlend();
  cubReg.SetCubBlend(false);

  osg::Geometry* curGeom = cubReg.GetRegion()->GetGeometry(cubReg.GetGeomIndex(), blend);

  (*updateGeomMap)[curGeom] = RenderGroup::DataUpdate(curGeom, cubReg.GetRegion(), cubReg.GetGeomIndex(), blend);

  //*************************************
  cube::Light::RecalcAndFillingLight(cubReg, wcpos, updateGeomMap);
}

void GridUtils::add(cube::CubRegion& cubReg, osg::Vec3d wcpos, bool recalcLight, RenderGroup::DataUpdateContainer* updateGeomMap)
{
  if(!cubReg.GetCubRendered())
  {
    cubReg.SetCubRendered(true);
  }

  osg::Geometry* curGeom = cubReg.GetRegion()->GetGeometry(cubReg.GetGeomIndex(), cubReg.GetCubBlend());

  (*updateGeomMap)[curGeom] = RenderGroup::DataUpdate(curGeom, cubReg.GetRegion(), cubReg.GetGeomIndex(), cubReg.GetCubBlend());

  //*************************************
  if(recalcLight)
  {
    cube::Light::FindLightSourceAndFillingLight(cubReg, wcpos, updateGeomMap);
  }
}
