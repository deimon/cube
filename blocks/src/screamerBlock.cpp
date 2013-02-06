#include <screamerBlock.h>
#include <region.h>
#include <regionManager.h>
#include <mathUtils.h>
#include <gridUtils.h>

#include <osg/Math>

using namespace cube;

ScreamerBlock::ScreamerBlock()
: Block(Screamer, 1.0)
{
}

void ScreamerBlock::Generate(Region* reg)
{
  //for(int n = 0; n < 3; n++)
  //{
  //  int i = MathUtils::random(0, REGION_WIDTH - 1);
  //  int j = MathUtils::random(0, REGION_WIDTH - 1);

  //  cube::CubRegion cubReg = reg->GetCub(i, j, reg->GetHeight(i, j) + 1);

  //  if(cubReg.GetCubType() == cube::Block::Air)
  //  {
  //    cubReg.SetCubType(cube::Block::Screamer);
  //    osg::Vec3d wcpos(i + reg->GetPosition().x() + 0.1, j + reg->GetPosition().y() + 0.1, reg->GetHeight(i, j) + 1 + 0.1);
  //    cubReg.Updated(wcpos, 0.0);
  //  }
  //}
}

void ScreamerBlock::Update(double updateTime, double curTime, cube::CubRegion& cubReg, osg::Vec3d wcpos, RenderGroup::DataUpdateContainer* dataUpdate)  //wcpos - world cube position
{
  int stepCount = (curTime - (updateTime - _deltaTimeUpdate)) / _deltaTimeUpdate;

  for(int i = 0; i < stepCount;)
  {
    CubInfo::CubeSide side = (CubInfo::CubeSide)MathUtils::random(0, 5);

    osg::Vec3d csvec = wcpos + CubInfo::Instance().GetNormal(side); //csvec - cube side vector

    if(csvec.z() > 0.0 && csvec.z() < 127.0)
    {
      i++;
      cube::CubRegion scubReg = RegionManager::Instance().GetCub(csvec.x(), csvec.y(), csvec.z());
     
      if(i == stepCount)
      {
        scubReg.SetCubType(Screamer);
        scubReg.Updated(csvec, curTime + _deltaTimeUpdate);

        cubReg.NotUpdated();
      }

      if(!scubReg.GetCubRendered())
      {
        scubReg.SetCubRendered(true);
      }

      osg::Geometry* curGeom = scubReg.GetRegion()->GetOrCreateNewGeometry(scubReg.GetGeomIndex(), scubReg.GetCubBlend());

      if(dataUpdate->find(curGeom) == dataUpdate->end())
        (*dataUpdate)[curGeom] = RenderGroup::DataUpdate(curGeom, scubReg.GetRegion(), scubReg.GetGeomIndex(), scubReg.GetCubBlend());

      GridUtils::RemoveCub(wcpos, dataUpdate);
      wcpos = csvec;
    }
  }
}
