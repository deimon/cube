#include <screamerBlock.h>
#include <region.h>
#include <regionManager.h>
#include <mathUtils.h>
#include <gridUtils.h>

#include <osg/Math>

using namespace cube;

ScreamerBlock::ScreamerBlock()
: Block(Screamer)
{
}

void ScreamerBlock::Generate(Region* reg)
{
  //for(int n = 0; n < 3; n++)
  {
    int i = MathUtils::random(0, REGION_WIDTH - 1);
    int j = MathUtils::random(0, REGION_WIDTH - 1);

    cube::CubRegion cubReg = reg->GetCub(i, j, reg->GetHeight(i, j) + 1);

    if(cubReg.GetCubType() == cube::Block::Air)
    {
      cubReg.SetCubType(cube::Block::Screamer);
      osg::Vec3d wcpos(i + reg->GetPosition().x(), j + reg->GetPosition().y(), reg->GetHeight(i, j) + 1);
      cubReg.Updated(wcpos, 0.0);
    }
  }
}

void ScreamerBlock::Update(double curTime, cube::CubRegion& cubReg, osg::Vec3d wcpos, RenderGroup::DataUpdateContainer* dataUpdate)  //wcpos - world cube position
{
  CubInfo::CubeSide side = (CubInfo::CubeSide)MathUtils::random(0, 5);

  osg::Vec3d csvec = wcpos + CubInfo::Instance().GetNormal(side); //csvec - cube side vector

  if(csvec.z() > 0.0 && csvec.z() < 127.0)
  {
    cube::CubRegion scubReg = RegionManager::Instance().GetCub(csvec.x(), csvec.y(), csvec.z());
   
    //if(scubReg.GetCubType() != Block::Air && scubReg.GetCubType() != Block::Screamer)
    {
      scubReg.SetCubType(Screamer);
      scubReg.Updated(csvec, curTime + 1.0);
      osg::Geometry* curGeom = scubReg.GetRegion()->GetGeometry(scubReg.GetGeomIndex(), scubReg.GetCubBlend());
      (*dataUpdate)[curGeom] = RenderGroup::DataUpdate(curGeom, scubReg.GetRegion(), scubReg.GetGeomIndex(), scubReg.GetCubBlend());

      cubReg.NotUpdated();
      wcpos.x() = osg::round(wcpos.x()) + 0.1;
      wcpos.y() = osg::round(wcpos.y()) + 0.1;
      GridUtils::RemoveCub(wcpos, dataUpdate);
    }
  }
}
