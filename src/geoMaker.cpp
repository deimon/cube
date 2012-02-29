#include <geoMaker.h>
#include "perlin.h"
#include <regionManager.h>
#include <light.h>

using namespace cube;

void GeoMaker::CubFilling(cube::Region* rg, float rnd)
{
  GenNoise(rg, rnd);

  Perlin* perlin = new Perlin(2, 1, 1.0f, 123);

  for(int k = REGION_HEIGHT - 1; k >= 0; k--)
  {
    for(int i = 0; i < REGION_WIDTH; i++)
    {
      for(int j = 0; j < REGION_WIDTH; j++)
      {
        cube::CubRegion cubReg = rg->GetCub(i, j, k);
        if(cubReg.GetCubType() != cube::Cub::Air && 
          perlin->Get((float)(i + rg->GetX() * REGION_WIDTH)/30.0f, (float)(j + rg->GetY() * REGION_WIDTH)/30.0f, (float)k/20.0f) < -0.4f)
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

            if(cubReg.GetCubType() == cube::Cub::Air && cubReg.GetCubLight() > 0.12f)
            {
              cpos.z() -= 1.0f;
              cube::CubRegion downCubReg = RegionManager::Instance().GetCub(cpos.x(), cpos.y(), cpos.z());

              if(downCubReg.GetCubType() == cube::Cub::Air)
              {
                cube::Light::StartFillingLight(downCubReg, cpos, cubReg.GetCubLight(), rg->GetX(), rg->GetY());
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

float GeoMaker::CompileNoise(float x, float y)
{
  float int_X    = int(x);//����� ����� �
  float fractional_X = x - int_X;//����� �� �
  //���������� �
  float int_Y    = int(y);
  float fractional_Y = y - int_Y;
  //�������� 4 ���������� ��������
  float v1 = SmoothedNoise2D(int_X,     int_Y);
  float v2 = SmoothedNoise2D(int_X + 1, int_Y);
  float v3 = SmoothedNoise2D(int_X,     int_Y + 1);
  float v4 = SmoothedNoise2D(int_X + 1, int_Y + 1);
  //������������� �������� 1 � 2 ���� � ���������� ������������ ����� ����
  float i1 = Cosine_Interpolate(v1 , v2 , fractional_X);
  float i2 = Cosine_Interpolate(v3 , v4 , fractional_X);
  //� ����������� ���������� ������������ ���� �����
  //�� ���������� ��������-//��������
  return Cosine_Interpolate(i1 , i2 , fractional_Y);
}

int GeoMaker::PerlinNoise_2D(float x,float y,float factor)
{
  x += 32000.0f;
  y += 32000.0f;
  float total = 0;
  // ��� ����� ����� ����� � ������ �������� ���� cosf(sqrtf(2))*3.14f
  // ������� ����� ���� ������� � ��������� ��� ���������
  float persistence=0.5f;

  // ����������������� � ����� ����������, ���������� �������
  // �������� sqrtf(3.14f)*0.25f ��� ���-�� ��������� ��� ��������� J)
  float frequency = 0.25f;
  float amplitude=1;//���������, � ������ ����������� �� �������� �������������

  // ������ ������ �����������, ����� ������ �� ���� ������ �����������
  // (�� ���� ������ ��� �-��� ���� ����������?)

  x+= (factor);
  y+= (factor);

  // NUM_OCTAVES - ����������, ������� ���������� ����� �����,
  // ��� ������ �����, ��� ����� ���������� ���
  for(int i=0;i<NUM_OCTAVES;i++)
  {
    total += CompileNoise(x*frequency, y*frequency) * amplitude;
    amplitude *= persistence;
    frequency*=2;
  }
  //����� ����� ��������� �������� �����   �� �����-�� �������
  //��������:
  //total=sqrt(total);
  // total=total*total;
  // total=sqrt(1.0f/float(total));
  //total=255-total;-�.�.� ��� ������� �� ��������� ����������
  total=fabsf(total);
  int res=int(total*255.0f);//�������� ���� � �������� 0-255�
  return res;
}

void GeoMaker::GenNoise(cube::Region* rg, float rnd)
{
  //����� � gameDev http://www.gamedev.ru/articles/?id=30126
  //srand(time(NULL));

  // ��������� �����, ������� �������� ������
  // ����������� � ���� ��������
  //float fac =osg::PI*2*10 + ((float)rand() / RAND_MAX)* (osg::PI*3*10 - osg::PI*2*10);

  int xOffset = rg->GetX() * REGION_WIDTH;
  int yOffset = rg->GetY() * REGION_WIDTH;

  for(int i = -1; i < REGION_WIDTH + 1; i++)
  {
    for(int j = -1; j < REGION_WIDTH + 1; j++)
    {
      //�������� �� ���� ��������� ������� � ��������� �� ����������
      //pNoise[i*size+j]=PerlinNoise_2D(float(i),float(j),fac);

      int height = REGION_HEIGHT / 2 + ((PerlinNoise_2D(float(i + xOffset) / 3.0f, float(j + yOffset) / 3.0f, rnd) * (REGION_HEIGHT / 3)) / 255);
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

      //float light = 1.0f;
      //for(int z = height+1; z < REGION_HEIGHT; z++)
      //{
      //  cube::Cub& cub = rg->GetCub(i, j, z);
      //  cub._light = light;

      //  if(light > 0.01)
      //    light -= 0.1f;
      //}
    }
  }
}