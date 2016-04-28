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
    template <class T> bool check_table(const T& tables)
    {
      for(size_t i = 0 ; i < tables.size() ; ++i)
      {
        for(size_t j = i + 1 ; j < tables.size() ; ++j)
        {
          if(tables[i].id == tables[j].id)
          {
            return false;
          }
        }
      }
      return true;
    }

    template <class T, class C = void> class arg_call
    {
      static_assert(sizeof(T) > 0, "only specialize must be instanced");
    };

    template <class T> class arg_call<T*, typename std::enable_if<std::is_const<T>::value>::type>
    {
      public:
        typedef std::decay_t<T*>                 type;
        typedef arg_call                         self_type;
        typedef T*                               return_type;
        typedef std::decay_t<T>                  underlying_type;
        typedef std::shared_ptr<underlying_type> shared_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = get_functor_(value.get_id());
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        template <class U> static return_type convert_underlying_(toolsbox::any& any)
        {
          return &any.as<U>();
        }

        template <class U> static return_type convert_shared_(toolsbox::any& any)
        {
          return any.as<U>().get();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 12> convert_tables = {{
            {toolsbox::type_uid::get<underlying_type*>(),              self_type::convert_type_<underlying_type*>},
            {toolsbox::type_uid::get<const underlying_type*>(),        self_type::convert_type_<const underlying_type*>},
            {toolsbox::type_uid::get<const underlying_type*&>(),       self_type::convert_type_<const underlying_type*&>},
            {toolsbox::type_uid::get<const underlying_type* const&>(), self_type::convert_type_<const underlying_type* const&>},
            {toolsbox::type_uid::get<underlying_type*&>(),             self_type::convert_type_<underlying_type*&>},
            {toolsbox::type_uid::get<underlying_type* const&>(),       self_type::convert_type_<underlying_type* const&>},
            {toolsbox::type_uid::get<underlying_type>(),               self_type::convert_underlying_<underlying_type>},
            {toolsbox::type_uid::get<underlying_type&>(),              self_type::convert_underlying_<underlying_type&>},
            {toolsbox::type_uid::get<const underlying_type&>(),        self_type::convert_underlying_<const underlying_type&>},
            {toolsbox::type_uid::get<shared_type>(),                   self_type::convert_shared_<shared_type>},
            {toolsbox::type_uid::get<const shared_type&>(),            self_type::convert_shared_<const shared_type&>},
            {toolsbox::type_uid::get<shared_type&>(),                  self_type::convert_shared_<shared_type&>}
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };

    template <class T> class arg_call<T*, typename std::enable_if<!std::is_const<T>::value>::type>
    {
      public:
        typedef std::decay_t<T*>                 type;
        typedef arg_call                         self_type;
        typedef T*                               return_type;
        typedef std::remove_pointer_t<type>      underlying_type;
        typedef std::shared_ptr<underlying_type> shared_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = get_functor_(value.get_id());
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        template <class U> static return_type convert_underlying_(toolsbox::any& any)
        {
          return &any.as<U>();
        }

        template <class U> static return_type convert_shared_(toolsbox::any& any)
        {
          return any.as<U>().get();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 8> convert_tables = {{
            {toolsbox::type_uid::get<type>(),               self_type::convert_type_<type>},
            {toolsbox::type_uid::get<type&>(),              self_type::convert_type_<type&>},
            {toolsbox::type_uid::get<const type&>(),        self_type::convert_type_<const type&>},
            {toolsbox::type_uid::get<underlying_type>(),    self_type::convert_underlying_<underlying_type>},
            {toolsbox::type_uid::get<underlying_type&>(),   self_type::convert_underlying_<underlying_type&>},
            {toolsbox::type_uid::get<shared_type>(),        self_type::convert_shared_<shared_type>},
            {toolsbox::type_uid::get<const shared_type&>(), self_type::convert_shared_<const shared_type&>},
            {toolsbox::type_uid::get<shared_type&>(),       self_type::convert_shared_<shared_type&>}
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };

    template <class T> class arg_call<T, typename std::enable_if<std::is_const<T>::value && !std::is_pointer<T>::value && !is_shared_ptr<T>::value>::type>
    {
      public:
        typedef T                                         type;
        typedef std::decay_t<type>                        underlying_type;
        typedef arg_call                                  self_type;
        typedef type&                                     return_type;
        typedef std::add_pointer_t<underlying_type>       ptr_type;
        typedef std::add_pointer_t<const underlying_type> const_ptr_type;
        typedef std::shared_ptr<underlying_type>          shared_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = get_functor_(value.get_id());
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        template <class U> static return_type convert_ptr_(toolsbox::any& any)
        {
          return *any.as<U>();
        }

        template <class U> static return_type convert_shared_(toolsbox::any& any)
        {
          return *any.as<U>().get();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 14> convert_tables = {{
            {toolsbox::type_uid::get<underlying_type>(),        self_type::convert_type_<underlying_type>},
            {toolsbox::type_uid::get<underlying_type&>(),       self_type::convert_type_<underlying_type&>},
            {toolsbox::type_uid::get<const underlying_type>(),  self_type::convert_type_<const underlying_type>},
            {toolsbox::type_uid::get<const underlying_type&>(), self_type::convert_type_<const underlying_type&>},
            {toolsbox::type_uid::get<ptr_type>(),               self_type::convert_ptr_<ptr_type>},
            {toolsbox::type_uid::get<const ptr_type>(),         self_type::convert_ptr_<const ptr_type>},
            {toolsbox::type_uid::get<ptr_type&>(),              self_type::convert_ptr_<ptr_type&>},
            {toolsbox::type_uid::get<const ptr_type&>(),        self_type::convert_ptr_<const ptr_type&>},
            {toolsbox::type_uid::get<const_ptr_type>(),         self_type::convert_ptr_<const_ptr_type>},
            {toolsbox::type_uid::get<const_ptr_type&>(),        self_type::convert_ptr_<const_ptr_type&>},
            {toolsbox::type_uid::get<const const_ptr_type&>(),  self_type::convert_ptr_<const const_ptr_type&>},
            {toolsbox::type_uid::get<shared_type>(),            self_type::convert_shared_<shared_type>},
            {toolsbox::type_uid::get<const shared_type&>(),     self_type::convert_shared_<const shared_type&>},
            {toolsbox::type_uid::get<shared_type&>(),           self_type::convert_shared_<shared_type&>}
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };

    template <class T> class arg_call<T, typename std::enable_if<!std::is_const<T>::value && !std::is_pointer<T>::value && !is_shared_ptr<T>::value>::type>
    {
      public:
        typedef T                                   type;
        typedef std::decay_t<type>                  underlying_type;
        typedef arg_call                            self_type;
        typedef type&                               return_type;
        typedef std::add_pointer_t<underlying_type> ptr_type;
        typedef std::shared_ptr<underlying_type>    shared_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = get_functor_(value.get_id());
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        template <class U> static return_type convert_ptr_(toolsbox::any& any)
        {
          return *any.as<U>();
        }

        template <class U> static return_type convert_shared_(toolsbox::any& any)
        {
          return *any.as<U>().get();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 8> convert_tables = {{
            {toolsbox::type_uid::get<underlying_type>(),    self_type::convert_type_<underlying_type>},
            {toolsbox::type_uid::get<underlying_type&>(),   self_type::convert_type_<underlying_type&>},
            {toolsbox::type_uid::get<ptr_type>(),           self_type::convert_ptr_<ptr_type>},
            {toolsbox::type_uid::get<ptr_type&>(),          self_type::convert_ptr_<ptr_type&>},
            {toolsbox::type_uid::get<ptr_type const&>(),    self_type::convert_ptr_<ptr_type const&>},
            {toolsbox::type_uid::get<shared_type>(),        self_type::convert_shared_<shared_type>},
            {toolsbox::type_uid::get<const shared_type&>(), self_type::convert_shared_<const shared_type&>},
            {toolsbox::type_uid::get<shared_type&>(),       self_type::convert_shared_<shared_type&>}
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };

    template <class T> class arg_call<T, typename std::enable_if<is_shared_ptr<T>::value>::type>
    {
      public:
        typedef std::decay_t<T>  type;
        typedef arg_call         self_type;
        typedef T                return_type;

        static bool check(const toolsbox::any& value)
        {
          functor_type fct = get_functor_(value.get_id());
          return fct != nullptr;
        }

        static return_type convert(toolsbox::any& any)
        {
          assert(check(any));
          functor_type fct = get_functor_(any.get_id());
          return (*fct)(any);
        }

      private:
        typedef return_type (*functor_type)(toolsbox::any&);

        template <class U> static return_type convert_type_(toolsbox::any& any)
        {
          return any.as<U>();
        }

        static functor_type get_functor_(toolsbox::type_uid::id_type id)
        {
          struct convert_table {
              toolsbox::type_uid::id_type id;
              functor_type                convert;
          };

          static const std::array<convert_table, 3> convert_tables = {{
            {toolsbox::type_uid::get<type>(),        self_type::convert_type_<type>},
            {toolsbox::type_uid::get<type&>(),       self_type::convert_type_<type&>},
            {toolsbox::type_uid::get<const type&>(), self_type::convert_type_<const type&>}
          }};

          assert(check_table(convert_tables));

          for(const convert_table& table : convert_tables)
          {
            if(id == table.id)
            {
              return table.convert;
            }
          }

          return nullptr;
        }
    };
  }
}

#endif

