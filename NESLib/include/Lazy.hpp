
#include <functional>
template <typename T> class Lazy
{
  private:
    std::function<T()> _creator;

  public:
    Lazy(std::function<T()> creator) : _creator(creator)
    {
    }

    T &operator->()
    {
        static T &instance = _creator();
        return instance;
    }
};