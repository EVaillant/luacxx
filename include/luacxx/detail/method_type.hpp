#ifndef LUACXX_DETAIL_METHOD_TYPE_HH
# define LUACXX_DETAIL_METHOD_TYPE_HH

# include <functional>

namespace luacxx
{
  namespace detail
  {
    enum class MethodTypeStyle
    {
      CXX,
      CONST_CXX,
      C
    };

    template <MethodTypeStyle S, class T, class R, class ... ARGS> struct MethodType
    {
        typedef T type_t;
        typedef R (type_t::*method_ptr_t)(ARGS ... args);

        static std::function<R (ARGS... args)> build_functor(type_t* self, method_ptr_t mth)
        {
          return [self, mth](ARGS ... args) -> R
          {
            return (self->*mth)(std::forward<ARGS>(args)...);
          };
        }
    };

    template <class T, class R, class ... ARGS> struct MethodType<MethodTypeStyle::CONST_CXX, T, R, ARGS...>
    {
        typedef T type_t;
        typedef R (type_t::*method_ptr_t)(ARGS ... args) const;

        static std::function<R (ARGS... args)> build_functor(type_t* self, method_ptr_t mth)
        {
          return [self, mth](ARGS ... args) -> R
          {
            return (self->*mth)(std::forward<ARGS>(args)...);
          };
        }
    };

    template <class T, class R, class ... ARGS> struct MethodType<MethodTypeStyle::C, T, R, ARGS...>
    {
        typedef T type_t;
        typedef R (*method_ptr_t)(type_t&p, ARGS ... args);

        static std::function<R (ARGS... args)> build_functor(type_t* self, method_ptr_t mth)
        {
          return [self, mth](ARGS ... args) -> R
          {
            return (*mth)(*self, std::forward<ARGS>(args)...);
          };
        }
    };
  }
}

#endif
