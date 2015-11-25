#ifndef LUACXX_DETAIL_POINTER_TO_REF_HH
# define LUACXX_DETAIL_POINTER_TO_REF_HH

# include <type_traits>

namespace luacxx
{
  namespace detail
  {
    template <class T> typename std::enable_if< std::is_pointer<T>::value, typename std::remove_pointer<T>::type&>::type pointer_to_ref(T &t)
    {
      return *t;
    }

    template <class T> typename std::enable_if<!std::is_pointer<T>::value, T&>::type pointer_to_ref(T &t)
    {
      return t;
    }
  }
}

#endif
