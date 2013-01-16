#include <geoMaker.h>
#include <regionManager.h>
#include <light.h>

using namespace cube;

Perlin* GeoMaker::_perlin3d = new Perlin(2, 1, 1.0f, 123);
Perlin* GeoMaker::_perlin2d = new Perlin(1, 1, 1.0f, 123);

void GeoMaker::CubFilling(cube::Region* rg, float rnd)
{
  if(rg->IsOffside())
    GenOffsideNoise(rg, rnd);
  else
  {
    GenNoise(rg, rnd);

    for(int k = REGION_HEIGHT - 1; k >= 0; k--)
    {
      for(int i = 0; i < REGION_WIDTH; i++)
      {
        for(int j = 0; j < REGION_WIDTH; j++)
        {
          cube::CubRegion cubReg = rg->GetCub(i, j, k);
          if(cubReg.GetCubType() != cube::Cub::Air && 
            _perlin3d->Get((float)(i + rg->GetX() * REGION_WIDTH)/30.0f, (float)(j + rg->GetY() * REGION_WIDTH)/30.0f, (float)k/20.0f) < -0.4f)
          {
            cubReg.SetCubType(cube::Cub::Air);

            if(cubReg.GetCubRendered())
            {
              cubReg.SetCubRendered(false);
            }

            if(k == rg->GetHeight(i, j))
              rg->SetHeight(i,j, k - 1);
          }
          else
            if(k == rg->GetHeight(i, j) && cubReg.GetCubType() == cube::Cub::Ground)
              cubReg.SetCubType(cube::Cub::Grass);
        }
      }
    }
  }
}

void GeoMaker::LightFilling(cube::Region* rg)
{
  for(int gIndex = GEOM_COUNT - 1; gIndex >= 0; gIndex--)
  {
    if(rg->_airCubCount[0][gIndex] == CUBS_IN_GEOM)
    {
      for(int i = 0; i < REGION_WIDTH; i++)
        for(int j = 0; j < REGION_WIDTH; j++)
          for(int k = 0; k < REGION_WIDTH; k++)
          {
            cube::CubRegion cubReg = rg->GetCub(i, j, gIndex * REGION_WIDTH + k);
            cubReg.GetCubLight() = 1.0f;
          }
    }
    else
    {
      for(int k = REGION_WIDTH; k >= 0; k--)
      {
        bool found = false;

        for(int i = 0; i < REGION_WIDTH; i++)
        {
          for(int j = 0; j < REGION_WIDTH; j++)
          {
            osg::Vec3d cpos = rg->GetPosition() + osg::Vec3d(i + 0.1f, j + 0.1f, gIndex * REGION_WIDTH + k + 0.1f);
            cube::CubRegion cubReg = RegionManager::Instance().GetCub(cpos.x(), cpos.y(), cpos.z());

            if((cubReg.GetCubType() == cube::Cub::Air || cubReg.GetCubBlend()) && cubReg.GetCubLight() > 0.12f)
            {
              cpos.z() -= 1.0f;
              cube::CubRegion downCubReg = RegionManager::Instance().GetCub(cpos.x(), cpos.y(), cpos.z());

              if(downCubReg.GetCubType() == cube::Cub::Air || downCubReg.GetCubBlend())
              {
                cube::Light::StartFillingLight(downCubReg, cpos, cubReg.GetCubLight(), rg->GetPosition());
                found = true;
              }
            }
          }
        }

        if(!found)
          return;
      }
    }
  }
}

void GeoMaker::RenderFilling(cube::Region* rg)
{
  for(int gIndex = GEOM_COUNT - 1; gIndex >= 0; gIndex--)
  {
    if(rg->_airCubCount[0][gIndex] == CUBS_IN_GEOM || rg->_airCubCount[0][gIndex] == 0)
      continue;

    for(int i = -1; i <= REGION_WIDTH; i++)
    {
      for(int j = -1; j <= REGION_WIDTH; j++)
      {
        for(int k = REGION_WIDTH - 1; k >= 0; k--)
        {
          osg::Vec3d cpos = rg->GetPosition() + osg::Vec3d(i + 0.1f, j + 0.1f, gIndex * REGION_WIDTH + k + 0.1f);
          cube::CubRegion cubReg = RegionManager::Instance().GetCub(cpos.x(), cpos.y(), cpos.z());
          if(cubReg.GetCubType() == cube::Cub::Air)
          {
            for(int s = CubInfo::FirstSide; s <= CubInfo::EndSide; s++)
            {
              CubInfo::CubeSide side = (CubInfo::CubeSide)s;

              osg::Vec3d vec = rg->GetPosition() + osg::Vec3d(i + 0.1f, j + 0.1f, gIndex * REGION_WIDTH + k + 0.1f) + CubInfo::Instance().GetNormal(side);

              if(vec.z() < 0 || vec.z() > 128)
                continue;

              int rx = Region::ToRegionIndex(vec.x());
              int ry = Region::ToRegionIndex(vec.y());

              cube::Region* srg = RegionManager::Instance().ContainsRegion(rx, ry);

              if(srg == NULL)
              {
                srg = RegionManager::Instance().CreateRegion(rx, ry);
              }

              vec -= srg->GetPosition();

              cube::CubRegion scubReg = srg->GetCub(vec.x(), vec.y(), vec.z());

              if(!scubReg.GetCubRendered() && scubReg.GetCubType() != cube::Cub::Air)
              {
                scubReg.SetCubRendered(true);
              }
            }
          }
        }
      }
    }
  }
}

void GeoMaker::GenNoise(cube::Region* rg, float rnd)
{
  int xOffset = rg->GetX() * REGION_WIDTH;
  int yOffset = rg->GetY() * REGION_WIDTH;

  for(int i = -1; i < REGION_WIDTH + 1; i++)
  {
    for(int j = -1; j < REGION_WIDTH + 1; j++)
    {
      int height = 4
        + (_perlin2d->Get(float(i + xOffset) / 100.0f, float(j + yOffset) / 100.0f) * 0.5f + 0.5f) * (80)
        + (_perlin2d->Get(float(i + xOffset) / 20.0f, float(j + yOffset) / 20.0f) * 0.5f + 0.5f) * (25);

      rg->SetHeight(i, j, height);

      if(i < 0 || i >= REGION_WIDTH || j < 0 || j >= REGION_WIDTH)
        continue;
      
      cube::CubRegion cubReg = rg->GetCub(i, j, height);
      cubReg.SetCubType(cube::Cub::Grass);
      cubReg.SetCubRendered(true);

      for(int z = 0; z < height && height < REGION_HEIGHT; z++)
      {
        cube::CubRegion cubReg = rg->GetCub(i, j, z);

        if(z / 85.0 < ((float)rand() / RAND_MAX))
          cubReg.SetCubType(cube::Cub::Stone);
        else
          cubReg.SetCubType(cube::Cub::Ground);
      }
    }
  }
}

void GeoMaker::GenOffsideNoise(cube::Region* rg, float rnd)
{
  int xOffset = rg->GetX() * REGION_WIDTH;
  int yOffset = rg->GetY() * REGION_WIDTH;

  for(int i = -1; i < REGION_WIDTH + 1; i++)
  {
    for(int j = -1; j < REGION_WIDTH + 1; j++)
    {
      int height = 4
        + (_perlin2d->Get(float(i + xOffset) / 100.0f, float(j + yOffset) / 100.0f) * 0.5f + 0.5f) * (80)
        + (_perlin2d->Get(float(i + xOffset) / 20.0f, float(j + yOffset) / 20.0f) * 0.5f + 0.5f) * (25);

      rg->SetHeight(i, j, height);

      if(i < 0 || i >= REGION_WIDTH || j < 0 || j >= REGION_WIDTH)
        continue;

      cube::CubRegion cubReg = rg->GetCub(i, j, height);
      cubReg.SetCubType(cube::Cub::Stone);
      cubReg.SetCubRendered(true);

      for(int z = 0; z < height && height < REGION_HEIGHT; z++)
      {
        cube::CubRegion cubReg = rg->GetCub(i, j, z);

        cubReg.SetCubType(cube::Cub::Stone);
      }
    }
  }
}
