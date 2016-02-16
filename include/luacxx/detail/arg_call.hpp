#ifndef LUACXX_DETAIL_ARG_CALL_HPP
# define LUACXX_DETAIL_ARG_CALL_HPP

# include <luacxx/detail/type_traits.hpp>
# include <cassert>

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
      typedef std::shared_ptr<register_type> shared_register_type;
      typedef std::unique_ptr<register_type> unique_register_type;

      static bool check(toolsbox::any& value)
      {
        return (value.is<register_type>()) || (value.is<shared_register_type>()) || (value.is<unique_register_type>());
      }

      static T convert(toolsbox::any& value)
      {
        assert(check(value));

        if(value.is<register_type>())
        {
          return &value.as<register_type>();
        }
        else if(value.is<shared_register_type>())
        {
          return value.as<shared_register_type>().get();
        }
        else
        {
          return value.as<unique_register_type>().get();
        }
      }
    };

    template <class T> struct arg_call<T, typename std::enable_if<!std::is_pointer<T>::value && !is_shared_ptr<T>::value && !is_unique_ptr<T>::value>::type>
    {
      typedef typename register_type<T>::type register_type;
      typedef std::shared_ptr<register_type> shared_register_type;
      typedef std::unique_ptr<register_type> unique_register_type;

      static bool check(toolsbox::any& value)
      {
        return (value.is<register_type>()) || (value.is<shared_register_type>()) || (value.is<unique_register_type>());
      }

      static T& convert(toolsbox::any& value)
      {
        assert(check(value));

        if(value.is<register_type>())
        {
          return value.as<register_type>();
        }
        else if(value.is<shared_register_type>())
        {
          return *value.as<shared_register_type>();
        }
        else
        {
          return *value.as<unique_register_type>();
        }
      }
    };

    template <class T> struct arg_call<T, typename std::enable_if<!std::is_pointer<T>::value && is_shared_ptr<T>::value>::type>
    {
      typedef typename register_type<T>::type register_type;
      typedef std::shared_ptr<register_type> shared_register_type;
      typedef std::unique_ptr<register_type> unique_register_type;

      static bool check(toolsbox::any& value)
      {
        return value.is<unique_register_type>() || value.is<shared_register_type>();
      }

      static T& convert(toolsbox::any& value)
      {
        if(value.is<unique_register_type>())
        {
          value = shared_register_type(value.as<unique_register_type>().release());
        }

        return value.as<shared_register_type>();
      }
    };

    template <class T> struct arg_call<T, typename std::enable_if<!std::is_pointer<T>::value && is_unique_ptr<T>::value>::type>
    {
      typedef typename register_type<T>::type register_type;
      typedef std::unique_ptr<register_type> unique_register_type;

      static bool check(toolsbox::any& value)
      {
        return value.is<unique_register_type>();
      }

      static T& convert(toolsbox::any& value)
      {
        return value.as<unique_register_type>();
      }
    };

    template <class T> auto cast_arg_call(toolsbox::any& value) -> decltype(arg_call<T>::convert(value))
    {
      return arg_call<T>::convert(value);
    }

    template <class T> void check_arg_call(std::string& msg, toolsbox::any& value)
    {
      if(! arg_call<T>::check(value))
      {
        msg = msg_error_invalid_tranformation;
      }
    }
  }
}

#endif

