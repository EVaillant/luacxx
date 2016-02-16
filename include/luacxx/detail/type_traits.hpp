#ifndef LUACXX_DETAIL_TYPE_TRAITS_HPP
# define LUACXX_DETAIL_TYPE_TRAITS_HPP

# include <memory>
# include <lua.hpp>

namespace luacxx
{
  namespace detail
  {
    //
    //
    // register_type
    template <class T> struct register_type
    {
      typedef T type;
    };

    template <class T> struct register_type<std::unique_ptr<T>>
    {
      typedef typename register_type<T>::type type;
    };

    template <class T> struct register_type<std::shared_ptr<T>>
    {
      typedef typename register_type<T>::type type;
    };

    template <class T> struct register_type<T*>
    {
      typedef typename register_type<T>::type type;
    };

    template <class T> struct register_type<T&>
    {
      typedef typename register_type<T>::type type;
    };

    template <class T> struct register_type<T&&>
    {
      typedef typename register_type<T>::type type;
    };

    template <class T> struct register_type<const T>
    {
      typedef typename register_type<T>::type type;
    };

    template <class T> struct register_type<const T*>
    {
      typedef typename register_type<T>::type type;
    };

    template <class T> struct register_type<const T&>
    {
      typedef typename register_type<T>::type type;
    };

    template <class T> struct register_type<const T&&>
    {
      typedef typename register_type<T>::type type;
    };

    //
    //
    // in_out_permission_value
    template <bool aIn, bool aOut, bool dIn> struct in_out_permission_value
    {
      static const bool allow_in   = aIn;
      static const bool allow_out  = aOut;
      static const bool default_in = dIn;
    };

    template <class T>  struct in_out_permission              : in_out_permission_value<true,   false,  true>  {};
    template <class T>  struct in_out_permission<const T>     : in_out_permission_value<true,   false,  true>  {};
    template <class T>  struct in_out_permission<const T*>    : in_out_permission_value<true,   false,  true>  {};
    template <class T>  struct in_out_permission<const T&>    : in_out_permission_value<true,   false,  true>  {};
    template <class T>  struct in_out_permission<T*>          : in_out_permission_value<true,   true,   true>  {};
    template <class T>  struct in_out_permission<T&>          : in_out_permission_value<true,   true,   true>  {};
    template <>         struct in_out_permission<lua_State*>  : in_out_permission_value<false,  true,   false> {};

    //
    //
    //
    template<class>   struct is_shared_ptr : std::false_type {};
    template<class T> struct is_shared_ptr<T&> : is_shared_ptr<T> {};
    template<class T> struct is_shared_ptr<const T> : is_shared_ptr<T> {};
    template<class T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

    template<class>   struct is_unique_ptr : std::false_type {};
    template<class T> struct is_unique_ptr<T&> : is_unique_ptr<T> {};
    template<class T> struct is_unique_ptr<const T> : is_unique_ptr<T> {};
    template<class T> struct is_unique_ptr<std::unique_ptr<T>> : std::true_type {};
  }
}

#endif
