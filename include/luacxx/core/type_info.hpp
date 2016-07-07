#ifndef LUACXX_CORE_TYPE_INFO_HPP
# define LUACXX_CORE_TYPE_INFO_HPP

# include <toolsbox/any.hpp>
# include <luacxx/core/constant.hpp>
# include <luacxx/core/type_traits.hpp>

# include <lua.hpp>

# include <vector>
# include <map>

namespace luacxx
{
  class policy_node;

  class common_type_info
  {
    public:
      typedef toolsbox::any variable_type;

      enum class underlying_type
      {
        Class,
        Enum,
        Native,
        Unknown
      };

      common_type_info(underlying_type ut)
        : underlying_type_(ut)
      {
      }

      virtual ~common_type_info()
      {
      }

      underlying_type get_underlying_type() const
      {
        return underlying_type_;
      }

      bool valid() const
      {
        return underlying_type_ != underlying_type::Unknown;
      }

      virtual void to_lua(state_type state, variable_type& var, std::string &error_msg, const policy_node& policy) const = 0;
      virtual void from_lua(state_type state, std::size_t idx, variable_type& var, std::string& error_msg, const policy_node& policy) const = 0;

    protected:
      underlying_type underlying_type_;
  };

  class common_class_type_info : public common_type_info
  {
    public:
      typedef common_class_type_info      self_type;
      typedef std::shared_ptr<self_type>  self_smart_type;
      typedef toolsbox::type_uid::id_type class_id_type;

      enum class class_ptr_type
      {
        smart_ptr = 0,
        raw_ptr   = 1
      };

      struct class_field
      {
        void*          ptr;
        class_ptr_type type;
        class_id_type  id;
      };

      typedef toolsbox::any (*to_any_type)(const class_field& c);
      typedef void (*cast_type)(toolsbox::any& c);

      inline common_class_type_info(underlying_type ut, class_id_type id, to_any_type to_cast)
        : common_type_info(ut)
        , class_id_(id)
        , to_cast_(to_cast)
      {
      }

      inline ~common_class_type_info()
      {
      }

      static inline std::pair<bool, class_field> get_class_field(state_type state, std::size_t idx)
      {
        class_field ptr {nullptr, class_ptr_type::raw_ptr, 0};
        bool status = false;

        if(lua_istable(state, idx))
        {
          int nb_pop = 0;

          lua_pushstring(state, detail::lua_field_id);
          lua_gettable(state, idx);
          ++nb_pop;

          if(lua_isinteger(state, -1))
          {
            ptr.id = lua_tointeger(state, -1);

            lua_pushstring(state, detail::lua_field_type);
            lua_gettable(state, idx);
            ++nb_pop;

            if(lua_isinteger(state, -1))
            {
              bool error = false;
              switch(lua_tointeger(state, -1))
              {
                case 0:
                  ptr.type =  class_ptr_type::smart_ptr;
                  break;

                case 1:
                  ptr.type =  class_ptr_type::raw_ptr;
                  break;

                default:
                  error = true;
                  break;
              }

              if(!error)
              {
                lua_pushstring(state, detail::lua_field_ptr);
                lua_gettable(state, idx);
                ++nb_pop;

                if(lua_isuserdata(state, -1))
                {
                  ptr.ptr = lua_touserdata(state, -1);
                  status  = true;
                }
              }
            }
          }
          lua_pop(state, nb_pop);
        }
        else if(lua_isnil(state, idx))
        {
          status = true;
        }

        return std::make_pair(status, ptr);
      }

      inline toolsbox::any get_instance(const class_field& c) const
      {
        toolsbox::any ret;
        if(c.id != class_id_ && c.ptr != nullptr)
        {
          convert_mappings::iterator it = convert_mappings_.find(c.id);
          if(it == convert_mappings_.end())
          {
            convert_mapping map;
            build_mapping_(c, map);
            if(map.to_any_)
            {
              it = convert_mappings_.insert(std::make_pair(c.id, std::move(map))).first;
            }
          }

          if(it != convert_mappings_.end())
          {
            convert_mapping& map = it->second;
            ret = (*map.to_any_)(c);
            for(cast_type cast : map.casts_)
            {
              (*cast)(ret);
            }
          }
        }
        else
        {
          ret = (*to_cast_)(c);
        }
        return ret;
      }

      template <class T> T* get_instance(state_type state, std::size_t idx)
      {
        typedef T                           class_type;
        typedef std::shared_ptr<class_type> smart_type;

        class_type* ret =  nullptr;
        const char* msg = nullptr;
        std::pair<bool, common_class_type_info::class_field> class_field = get_class_field(state, idx);
        if(class_field.first)
        {
          if(class_field.second.ptr)
          {
            toolsbox::any any = get_instance(class_field.second);
            if(any.empty())
            {
              msg = msg_error_invalid_object;
            }
            else
            {
              if(any.is<class_type*>())
              {
                ret = any.as<class_type*>();
              }
              else if(any.is<smart_type>())
              {
                ret = any.as<smart_type>().get();
              }
              else
              {
                msg = msg_error_invalid_object;
              }
            }
          }
          else
          {
            msg = msg_error_null_object;
          }
        }
        else
        {
          msg = msg_error_object_corrupted;
        }
        if(msg)
        {
          luaL_error(state, msg);
        }
        else
        {
          assert(ret);
          lua_remove(state, 1);
        }
        return ret;
      }

      inline void add_base(cast_type cast, const self_smart_type& base)
      {
        bases_.push_back(std::make_pair(cast, base));
      }

      virtual const std::string& get_module_name() const = 0;
      virtual const std::string& get_class_name() const = 0;

    protected:
      struct convert_mapping
      {
        inline convert_mapping()
          : to_any_(nullptr)
        {
        }

        to_any_type            to_any_;
        std::vector<cast_type> casts_;
      };

      typedef std::map<class_id_type, convert_mapping> convert_mappings;

      inline void build_mapping_(const class_field& c, convert_mapping& map) const
      {
        if(c.id == class_id_)
        {
          map.to_any_ = to_cast_;
        }
        else
        {
          for(const auto& pair : bases_)
          {
            cast_type cast              = pair.first;
            const self_smart_type& base = pair.second;

            base->build_mapping_(c, map);
            if(map.to_any_)
            {
              map.casts_.push_back(cast);
              break;
            }
          }
        }
      }

    private:
      typedef std::vector<std::pair<cast_type, self_smart_type>> bases_type;

      class_id_type            class_id_;
      mutable convert_mappings convert_mappings_;
      bases_type               bases_;
      to_any_type              to_cast_;
  };

  template <class T, class D = void> class type_info : public common_type_info
  {
    public:
      typedef T type;

      type_info(underlying_type ut)
        : common_type_info(ut)
      {
      }
  };

  template <class C> class type_info<C, typename std::enable_if<!std::is_same<std::string, C>::value && std::is_class<C>::value>::type> : public common_class_type_info
  {
    public:
      type_info(underlying_type ut, common_class_type_info::to_any_type to)
        : common_class_type_info(ut, toolsbox::type_uid::get<C>(), to)
      {
      }

      type_info(underlying_type ut)
        : type_info(ut, nullptr)
      {
      }

      type_info(common_class_type_info::to_any_type to)
        : type_info(common_type_info::underlying_type::Class, to)
      {
      }
  };

  template <class T, class D = void> class empty_type_info : public type_info<T>
  {
    public:
      typedef T type;
      typedef common_type_info::variable_type variable_type;

      empty_type_info()
        : type_info<T>(common_type_info::underlying_type::Unknown)
      {
      }

      virtual void to_lua(state_type, variable_type&, std::string &error_msg, const policy_node&) const override
      {
        error_msg = msg_error_type_not_supported;
      }

      virtual void from_lua(state_type, std::size_t, variable_type& , std::string& error_msg, const policy_node&) const override
      {
        error_msg = msg_error_type_not_supported;
      }
  };

  template <class T> class empty_type_info<T, typename std::enable_if<!std::is_same<std::string, T>::value && std::is_class<T>::value>::type> : public type_info<T>
  {
    public:
      typedef T type;
      typedef common_type_info::variable_type variable_type;

      empty_type_info()
        : type_info<T>(common_type_info::underlying_type::Unknown)
      {
      }

      virtual void to_lua(state_type, variable_type&, std::string &error_msg, const policy_node&) const override
      {
        error_msg = msg_error_type_not_supported;
      }

      virtual void from_lua(state_type, std::size_t, variable_type& , std::string& error_msg, const policy_node&) const override
      {
        error_msg = msg_error_type_not_supported;
      }

      virtual const std::string& get_module_name() const override
      {
        static std::string empty;
        return empty;
      }

      virtual const std::string& get_class_name() const override
      {
        static std::string empty;
        return empty;
      }
  };
}

#endif
