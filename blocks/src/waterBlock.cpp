#include <waterBlock.h>
#include <region.h>
#include <regionManager.h>
#include <mathUtils.h>
#include <gridUtils.h>

#include <osg/Math>

using namespace cube;

WaterBlock::WaterBlock()
: Block(Water, 0.5)
{
}

void WaterBlock::Generate(Region* reg)
{
}

void WaterBlock::Update(double updateTime, double curTime, cube::CubRegion& cubReg, osg::Vec3d wcpos, RenderGroup::DataUpdateContainer* dataUpdate)  //wcpos - world cube position
{
  osg::Vec3d csvec = wcpos + CubInfo::Instance().GetNormal(CubInfo::Z_BACK); //csvec - cube side vector
  cube::CubRegion scubReg = RegionManager::Instance().GetCub(csvec.x(), csvec.y(), csvec.z());
  
  if(scubReg.GetCubType() == Air || scubReg.GetCubType() == ObjGrass
    || (scubReg.GetCubType() == Water && scubReg.GetCubState() < 4)
    )
  {
    scubReg.SetCubType(Water);
    scubReg.SetCubState(4);
    scubReg.SetCubBlend(true);
    scubReg.Updated(csvec, curTime + _deltaTimeUpdate);

    if(!scubReg.GetCubRendered())
    {
      scubReg.SetCubRendered(true);
    }

    osg::Geometry* curGeom = scubReg.GetRegion()->GetOrCreateNewGeometry(scubReg.GetGeomIndex(), scubReg.GetCubBlend());

    if(dataUpdate->find(curGeom) == dataUpdate->end())
      (*dataUpdate)[curGeom] = RenderGroup::DataUpdate(curGeom, scubReg.GetRegion(), scubReg.GetGeomIndex(), scubReg.GetCubBlend());
  }
  else if(cubReg.GetCubState() > 0)
  {
    for(int s = CubInfo::FirstSide; s <= CubInfo::EndHorizSide; s++)
    {
      CubInfo::CubeSide side = (CubInfo::CubeSide)s;
      osg::Vec3d csvec = wcpos + CubInfo::Instance().GetNormal(side); //csvec - cube side vector
      cube::CubRegion scubReg = RegionManager::Instance().GetCub(csvec.x(), csvec.y(), csvec.z());

      if(scubReg.GetCubType() == Air || scubReg.GetCubType() == ObjGrass
        || (scubReg.GetCubType() == Water && scubReg.GetCubState() < cubReg.GetCubState() - 1)
        )
      {
        scubReg.SetCubType(Water);
        scubReg.SetCubState(cubReg.GetCubState() - 1);
        scubReg.SetCubBlend(true);
        scubReg.Updated(csvec, curTime + _deltaTimeUpdate);

        if(!scubReg.GetCubRendered())
        {
          scubReg.SetCubRendered(true);
        }

        osg::Geometry* curGeom = scubReg.GetRegion()->GetOrCreateNewGeometry(scubReg.GetGeomIndex(), scubReg.GetCubBlend());

        if(dataUpdate->find(curGeom) == dataUpdate->end())
          (*dataUpdate)[curGeom] = RenderGroup::DataUpdate(curGeom, scubReg.GetRegion(), scubReg.GetGeomIndex(), scubReg.GetCubBlend());
      }
    }
  }

  cubReg.NotUpdated();
}
