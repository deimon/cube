#include <geoMaker.h>

using namespace cube;

void GeoMaker::FillRegion(cube::Region* rg)
{
  GenNoise(rg);
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

void GeoMaker::GenNoise(cube::Region* rg)
{
  //����� � gameDev http://www.gamedev.ru/articles/?id=30126
  //srand(time(NULL));

  // ��������� �����, ������� �������� ������
  // ����������� � ���� ��������
  float fac =osg::PI*2*10 + ((float)rand() / RAND_MAX)* (osg::PI*3*10 - osg::PI*2*10);

  for(int i=0; i<REGION_SIZE; i++)
  {
    for(int j=0; j<REGION_SIZE; j++)
    {
      //�������� �� ���� ��������� ������� � ��������� �� ����������
      //pNoise[i*size+j]=PerlinNoise_2D(float(i),float(j),fac);

      int height = REGION_SIZE / 4 + ((PerlinNoise_2D(float(i),float(j),fac) * (REGION_SIZE / 4)) / 255);
      rg->SetHeight(i, j, height);

      cube::Cub& cub = rg->GetCub(i, j, height);
      cub._type = cube::Cub::Ground;
      cub._rendered = true;

      for(int z = 0; z <= height && height < REGION_SIZE; z++)
      {
        cube::Cub& cub = rg->GetCub(i, j, z);
        cub._type = cube::Cub::Ground;
      }
    }
  }

  for(int i=1; i<REGION_SIZE-1; i++)
  {
    for(int j=1; j<REGION_SIZE-1; j++)
    {

      int min = std::min<int>(std::min<int>(rg->GetHeight(i-1, j), rg->GetHeight(i+1, j)),
        std::min<int>(rg->GetHeight(i, j-1), rg->GetHeight(i, j+1)));
      for(int k = min+1; k < rg->GetHeight(i, j); k++)
      {
        cube::Cub& cub = rg->GetCub(i, j, k);
        //cub->_type = cube::Cub::Ground;
        cub._rendered = true;
      }
    }
  }
}