#ifndef LUACXX_DETAIL_COMMON_HH
# define LUACXX_DETAIL_COMMON_HH

# include <type_traits>
# include <memory>

# include <lua.hpp>

# include <vector>
# include <list>
# include <set>
# include <map>

namespace luacxx
{
  namespace detail
  {
    template<class>   struct is_smart_ptr : std::false_type {};
    template<class T> struct is_smart_ptr<T&> : is_smart_ptr<T> {};
    template<class T> struct is_smart_ptr<const T> : is_smart_ptr<T> {};
    template<class T> struct is_smart_ptr<std::shared_ptr<T>> : std::true_type {};

    template<class>   struct is_container : std::false_type {};
    template<class T, class A> struct  is_container<std::list<T, A>> : std::true_type {};
    template<class T, class A> struct  is_container<std::vector<T, A>> : std::true_type {};
    template<class T, class C, class A> struct  is_container<std::set<T, C, A>> : std::true_type {};
    template<class K, class V, class C, class A> struct  is_container<std::map<K, V, C, A>> : std::true_type {};

    template<class>   struct is_bind_function : std::false_type {};
    template<class T> struct is_bind_function<T&> : is_bind_function<T> {};
    template<class T> struct is_bind_function<const T> : is_bind_function<T> {};
    template<class R, class ... ARGS> struct is_bind_function<std::function<R (ARGS...)>> : std::true_type {};

    template <class T> using is_lua_state = typename std::is_same<lua_State*, T>::type;

    template<class T> using is_output_parameter = typename std::conditional<
      !is_lua_state<T>::value &&
      (std::is_pointer<T>::value || std::is_reference<T>::value) &&
      !std::is_const<
          typename std::remove_reference<
            typename std::remove_pointer<T>::type
          >::type
       >::value
    ,
      std::true_type
    ,
      std::false_type
    >::type;

    template <class T> using is_lua_native_type = typename std::conditional<
      std::is_floating_point<T>::value    ||
      std::is_integral<T>::value          ||
      std::is_same<std::string, T>::value ||
      std::is_same<bool, T>::value        ||
      is_bind_function<T>::value          ||
      std::is_enum<T>::value              ||
      is_container<T>::value              ||
      is_lua_state<T>::value
      ,
      std::true_type
      ,
      std::false_type
    >::type;

    template <class T> using remove_const_ref_ptr = typename std::conditional<
      std::is_same<lua_State*, T>::value
      ,
      T
      ,
      typename std::remove_const<
        typename std::remove_reference<
          typename std::remove_pointer<T>::type
        >::type
      >::type
    >::type;

    template <class T> using output_local_type = typename std::conditional<
      !is_smart_ptr<T>::value  && !is_lua_native_type<T>::value
      ,
      typename std::add_pointer<T>::type
      ,
      T
    >::type;

    template <class T> using input_local_type = typename std::conditional<
      !is_smart_ptr<T>::value  && !is_lua_native_type<T>::value
      ,
      std::shared_ptr<T>
      ,
      T
    >::type;

    template <class T> using local_type_wo_const_ref_ptr = typename std::conditional<
      is_output_parameter<T>::value
      ,
      output_local_type<T>
      ,
      input_local_type <T>
    >::type;
    template <class T> using local_type = local_type_wo_const_ref_ptr<remove_const_ref_ptr<T>>;

    template <class T> struct register_type
    {
      typedef T type;
    };

    template <class T> struct register_type<T*>
    {
      typedef typename register_type<T>::type type;
    };

    template <class T> struct register_type<T&>
    {
      typedef typename register_type<T>::type type;
    };

    template <class T> struct register_type<const T>
    {
      typedef typename register_type<T>::type type;
    };

    template <class T> struct register_type<std::shared_ptr<T>>
    {
      typedef typename register_type<T>::type type;
    };
  }
}

#endif
