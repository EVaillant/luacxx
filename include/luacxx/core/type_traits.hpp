#ifndef LUACXX_CORE_TYPE_TRAITS_HPP
# define LUACXX_CORE_TYPE_TRAITS_HPP

# include <memory>

struct lua_State;
namespace luacxx
{
  typedef lua_State* state_type;

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

  template <> struct register_type<const char*>
  {
    typedef const char* type;
  };

  template <class T> struct register_type<const T&>
  {
    typedef typename register_type<T>::type type;
  };

  template <class T> struct register_type<const T&&>
  {
    typedef typename register_type<T>::type type;
  };

  template <> struct register_type<state_type>
  {
    typedef state_type type;
  };

  //
  //
  // in_out_permission_value
  template <bool aIn, bool aOut, bool dIn, bool dOut = !dIn> struct in_out_permission_value
  {
    static const bool allow_in    = aIn;
    static const bool allow_out   = aOut;
    static const bool default_in  = dIn;
    static const bool default_out = dOut;
  };

  template <class T>  struct in_out_permission              : in_out_permission_value<true,   false,  true> {};
  template <class T>  struct in_out_permission<const T>     : in_out_permission_value<true,   false,  true> {};
  template <class T>  struct in_out_permission<const T*>    : in_out_permission_value<true,   false,  true> {};
  template <class T>  struct in_out_permission<const T&>    : in_out_permission_value<true,   false,  true> {};
  template <class T>  struct in_out_permission<T*>          : in_out_permission_value<true,   true,   true> {};
  template <class T>  struct in_out_permission<T&>          : in_out_permission_value<true,   true,   true> {};
  template <>         struct in_out_permission<state_type>  : in_out_permission_value<true,   false,  true> {};
  template <>         struct in_out_permission<void>        : in_out_permission_value<false,  false,  false, false> {};

  //
  //
  // is_shared_ptr
  template<class>   struct is_shared_ptr : std::false_type {};
  template<class T> struct is_shared_ptr<T&> : is_shared_ptr<T> {};
  template<class T> struct is_shared_ptr<const T> : is_shared_ptr<T> {};
  template<class T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};


  //
  // is_incomplete
  template <class T>  struct is_incomplete : ::std::false_type
  {
  };

  template <> struct is_incomplete<lua_State> : ::std::true_type
  {
  };
}

#endif
