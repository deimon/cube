#ifndef __SINGLETON_H__
#define __SINGLETON_H__

namespace cube
{
  namespace utils
  {
    template
    <typename DerivedSingleton>
    class Singleton
    {
    public:
      virtual ~Singleton(){}

      static DerivedSingleton& Instance()
      {
        static DerivedSingleton self;
        return self;
      }
    };
  }
}

#endif
