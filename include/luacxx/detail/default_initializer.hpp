#ifndef LUACXX_DETAIL_DEFAULT_INITIALIZER_HH
# define LUACXX_DETAIL_DEFAULT_INITIALIZER_HH

# include <memory>

namespace luacxx
{
  namespace detail
  {
    template <class T> struct default_initializer
    {
        static T create()
        {
          return std::move(T());
        }

        static T empty()
        {
          return std::move(T());
        }
    };

    template <class T> struct default_initializer<std::shared_ptr<T>>
    {
        static std::shared_ptr<T> create()
        {
          return std::move(std::shared_ptr<T>(new T()));
        }

        static std::shared_ptr<T> empty()
        {
          return std::move(std::shared_ptr<T>());
        }
    };
  }
}

#endif
