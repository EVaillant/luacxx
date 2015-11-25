#ifndef LUACXX_DETAIL_FORWARD_CONVERTER_HH
# define LUACXX_DETAIL_FORWARD_CONVERTER_HH

# include <luacxx/detail/common.hpp>

namespace luacxx
{
  namespace detail
  {
    template <class F, class T> typename std::enable_if<std::is_pointer<F>::value && std::is_pointer<T>::value, T>::type forward_converter(F &f)
    {
      return f;
    }

    template <class F, class T> typename std::enable_if<std::is_pointer<F>::value && !std::is_pointer<T>::value && !is_smart_ptr<T>::value, T&>::type forward_converter(F &f)
    {
      return *f;
    }

    template <class F, class T> typename std::enable_if<is_smart_ptr<F>::value && std::is_pointer<T>::value, T>::type forward_converter(F &f)
    {
      return f.get();
    }

    template <class F, class T> typename std::enable_if<is_smart_ptr<F>::value && is_smart_ptr<T>::value, T&>::type forward_converter(F &f)
    {
      return f;
    }

    template <class F, class T> typename std::enable_if<is_smart_ptr<F>::value && !std::is_pointer<T>::value && !is_smart_ptr<T>::value, T&>::type forward_converter(F &f)
    {
      return *f.get();
    }

    template <class F, class T> typename std::enable_if<!std::is_pointer<F>::value && !is_smart_ptr<F>::value && std::is_pointer<T>::value, T>::type forward_converter(F &f)
    {
      return &f;
    }

    template <class F, class T> typename std::enable_if<!std::is_pointer<F>::value && !is_smart_ptr<F>::value && !std::is_pointer<T>::value && !is_smart_ptr<F>::value, T&>::type forward_converter(F &f)
    {
      return f;
    }
  }
}

#endif
