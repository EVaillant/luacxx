#ifndef LUACXX_DETAIL_ARG_CALL_HPP
# define LUACXX_DETAIL_ARG_CALL_HPP

# include <luacxx/detail/type_traits.hpp>

# include <toolsbox/any.hpp>

# include <cassert>
# include <type_traits>

namespace luacxx
{
  namespace detail
  {
    template <class T, class C = void> struct arg_call
    {
      static_assert(sizeof(T) > 0, "only specialize must be instanced");
    };

    template <class T> struct arg_call<T, typename std::enable_if<std::is_pointer<T>::value>::type>
    {
      typedef typename register_type<T>::type register_type;
      typedef register_type*                  ptr_register_type;
      typedef std::shared_ptr<register_type>  shared_register_type;

      static bool check(toolsbox::any& value)
      {
        return (value.is<register_type>() || value.is<ptr_register_type>() || value.is<shared_register_type>());
      }

      static T convert(toolsbox::any& value)
      {
        assert(check(value));

        if(value.is<register_type>())
        {
          return &value.as<register_type>();
        }
        else if(value.is<ptr_register_type>())
        {
          return value.as<ptr_register_type>();
        }
        else
        {
          return value.as<shared_register_type>().get();
        }
      }
    };

    template <class T> struct arg_call<T, typename std::enable_if<!std::is_pointer<T>::value && !is_shared_ptr<T>::value>::type>
    {
      typedef typename register_type<T>::type register_type;
      typedef register_type*                  ptr_register_type;
      typedef std::shared_ptr<register_type>  shared_register_type;

      static bool check(toolsbox::any& value)
      {
        return (value.is<register_type>() || value.is<ptr_register_type>() || value.is<shared_register_type>());
      }

      static T& convert(toolsbox::any& value)
      {
        assert(check(value));

        if(value.is<register_type>())
        {
          return value.as<register_type>();
        }
        else if(value.is<ptr_register_type>())
        {
          return *value.as<ptr_register_type>();
        }
        else
        {
          return *value.as<shared_register_type>();
        }
      }
    };

    template <class T> struct arg_call<T, typename std::enable_if<!std::is_pointer<T>::value && is_shared_ptr<T>::value>::type>
    {
      typedef typename register_type<T>::type register_type;
      typedef std::shared_ptr<register_type> shared_register_type;

      static bool check(toolsbox::any& value)
      {
        return value.is<shared_register_type>();
      }

      static T& convert(toolsbox::any& value)
      {
        return value.as<shared_register_type>();
      }
    };
  }
}

#endif

