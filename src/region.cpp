#include <region.h>

using namespace cube;

cube::Region* Region::Generation(osg::Vec3d& position)
{
  cube::Region* region = new Region();
  region->_position = position;

  for(int x = 0; x < REGION_SIZE; x++)
  for(int y = 0; y < REGION_SIZE; y++)
  for(int z = 0; z < REGION_SIZE; z++)
  {
    cube::Cub* cub = &(region->_m[x][y][z]);
    
    cub->_type = cube::Cub::Air;
  }

  region->GenNoise();

  return region;
}

float Region::CompileNoise(float x, float y)
{
  float int_X    = int(x);//цела€ часть х
  float fractional_X = x - int_X;//дробь от х
  //аналогично у
  float int_Y    = int(y);
  float fractional_Y = y - int_Y;
  //получаем 4 сглаженных значени€
   float v1 = SmoothedNoise2D(int_X,     int_Y);
   float v2 = SmoothedNoise2D(int_X + 1, int_Y);
   float v3 = SmoothedNoise2D(int_X,     int_Y + 1);
   float v4 = SmoothedNoise2D(int_X + 1, int_Y + 1);
  //интерполируем значени€ 1 и 2 пары и производим интерпол€цию между ними
  float i1 = Cosine_Interpolate(v1 , v2 , fractional_X);
  float i2 = Cosine_Interpolate(v3 , v4 , fractional_X);
//€ использовал косинусною интерпол€цию »ћ’ќ лучше 
//по параметрам быстрота-//качество
  return Cosine_Interpolate(i1 , i2 , fractional_Y);
}

int Region::PerlinNoise_2D(float x,float y,float factor)
{
   float total = 0;
   // это число может иметь и другие значени€ хоть cosf(sqrtf(2))*3.14f 
   // главное чтобы было красиво и результат вас устраивал
   float persistence=0.5f;

   // экспериментируйте с этими значени€ми, попробуйте ставить 
   // например sqrtf(3.14f)*0.25f или что-то пот€желее дл€ понимани€ J)
   float frequency = 0.25f;
   float amplitude=1;//амплитуда, в пр€мой зависимости от значени€ настойчивости

   // вводим фактор случайности, чтобы облака не были всегда одинаковыми
   // (ћы ведь помним что ф-ци€ шума когерентна?) 
    
   x+= (factor);
   y+= (factor);

   // NUM_OCTAVES - переменна€, котора€ обозначает число октав,
   // чем больше октав, тем лучше получаетс€ шум
   for(int i=0;i<NUM_OCTAVES;i++)
   {
       total += CompileNoise(x*frequency, y*frequency) * amplitude;
       amplitude *= persistence;
       frequency*=2;
    }
    //здесь можно перевести значени€ цвета   по какой-то формуле
    //например:
    //total=sqrt(total);
    // total=total*total;
    // total=sqrt(1.0f/float(total)); 
    //total=255-total;-и.т.д все зависит от желаемого результата
    total=fabsf(total);
    int res=int(total*255.0f);//приводим цвет к значению 0-255Е
    return res;
}

void Region::GenNoise()
{
  //¬з€то с gameDev http://www.gamedev.ru/articles/?id=30126
  //srand(time(NULL));

  // случайное число, которое призвано внести
  // случайность в нашу текстуру
  float fac =osg::PI*2*10 + ((float)rand() / RAND_MAX)* (osg::PI*3*10 - osg::PI*2*10);

  for(int i=0 ;i<REGION_SIZE;i++)
  {
    for(int j=0 ;j<REGION_SIZE;j++)
    {
       //проходим по всем элементам массива и заполн€ем их значени€ми   
       //pNoise[i*size+j]=PerlinNoise_2D(float(i),float(j),fac);

      int height = REGION_SIZE / 3 + ((PerlinNoise_2D(float(i),float(j),fac) * (REGION_SIZE / 4)) / 255);

      for(int z = 0; z <= height && height < REGION_SIZE; z++)
      {
        cube::Cub* cub = &(_m[i][j][z]);
        cub->_type = cube::Cub::Ground;
      }
    }
  }
}