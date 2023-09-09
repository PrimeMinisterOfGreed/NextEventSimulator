
#include <functional>
#include <optional>

template <typename T, typename... Args> class FunctionPointer
{
  private:
    std::optional<std::function<T(Args...)>> fncPtr{};

  public:
    T operator()(Args... args)
    {
        if (fncPtr.has_value())
            fncPtr.value()(args...);
    }

    ~FunctionPointer() = default;
    FunctionPointer(std::function<T(Args...)> fnc) : fncPtr(fnc)
    {
    }

    FunctionPointer()
    {
    }

    void Attach(std::function<T(Args...)> fnc)
    {
        fncPtr.emplace(fnc);
    }
};
