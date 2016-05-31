#ifndef LUACXX_BINDING_ENGINE_HPP
# define LUACXX_BINDING_ENGINE_HPP

# include <luacxx/binding/module.hpp>
# include <luacxx/core/lookup_type.hpp>
# include <luacxx/core/basic_type_info.hpp>

namespace luacxx
{
  class engine
  {
    public:
      enum lua_lib {
        lt_none      = 0,
        lt_coroutine = (1 << 0),
        lt_table     = (1 << 1),
        lt_io        = (1 << 2),
        lt_os        = (1 << 3),
        lt_string    = (1 << 4),
        lt_utf8      = (1 << 5),
        lt_bit32     = (1 << 6),
        lt_math      = (1 << 7),
        lt_debug     = (1 << 8),
        lt_package   = (1 << 9),
        lt_all       = lt_coroutine | lt_table | lt_io | lt_os | lt_string | lt_utf8 | lt_bit32 | lt_math | lt_debug | lt_package
      };
      typedef std::underlying_type_t <lua_lib> lua_lib_underlying_type;

      inline engine()
        : state_(nullptr)
        , flags_(lt_all)
      {
      }

      inline ~engine()
      {
        if(state_)
        {
          lua_close(state_);
        }
      }

      inline void set_std_lib(lua_lib_underlying_type flags)
      {
        flags_ = flags;
      }

      inline module& get_module(const std::string& name)
      {
        return module_.get_module(name);
      }

      inline std::vector<std::string> need_to_bind()
      {
        return module_.need_to_bind();
      }

      inline lookup_type& get_lookup_type()
      {
        return lookup_type_;
      }

      inline bool bind()
      {
        if(!state_)
        {
          state_ = luaL_newstate();

          fill_lookup_type_();
          open_lib_();
        }
        bool status = true;
        std::vector<std::string> before_binding = need_to_bind();
        while(status && !before_binding.empty())
        {
          module_.bind(state_);
          std::vector<std::string> after_binding = need_to_bind();
          status = (after_binding != before_binding);
          before_binding = std::move(after_binding);
        }
        return status;
      }

      inline std::pair<bool, std::string> do_file(const std::string& name)
      {
        assert(state_);
        std::string msg;
        bool     status = (luaL_dofile(state_, name.c_str()) == LUA_OK);
        if(!status)
        {
          msg = lua_tostring(state_, -1);
        }
        return std::make_pair(status, msg);
      }

      inline std::pair<bool, std::string> do_string(const std::string& string)
      {
        assert(state_);
        std::string msg;
        bool     status = (luaL_dostring(state_, string.c_str()) == LUA_OK);
        if(!status)
        {
          msg = lua_tostring(state_, -1);
        }
        return std::make_pair(status, msg);
      }

      inline state_type get_state()
      {
        return state_;
      }

    protected:
      template <class T0, class ... T> void fill_lookup_type_integer_(typename std::enable_if<(sizeof...(T) != 0), int>::type = 0)
      {
        fill_lookup_type_integer_<T0>();
        fill_lookup_type_integer_<T...>();
      }

      template <class T> void fill_lookup_type_integer_()
      {
        lookup_type_.set<T>(std::make_shared<integer_type_info<T>>());
      }

      inline void fill_lookup_type_()
      {
        lookup_type_.set<bool>(std::make_shared<bool_type_info>());
        lookup_type_.set<std::string>(std::make_shared<string_type_info<std::string>>());
        lookup_type_.set<const char*>(std::make_shared<string_type_info<const char*>>());
        lookup_type_.set<float>(std::make_shared<number_type_info<float>>());
        lookup_type_.set<double>(std::make_shared<number_type_info<double>>());
        lookup_type_.set<long double>(std::make_shared<number_type_info<long double>>());
        fill_lookup_type_integer_<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>();
        lookup_type_.set<state_type>(std::make_unique<state_type_type_info>());
      }

      inline void open_lib_()
      {
        struct table_lib
        {
          bool          load;
          const char*   name;
          lua_CFunction func;
        };

        const table_lib libs[] = {
          {true, "_G", luaopen_base},
          {(lt_package & flags_),   LUA_LOADLIBNAME, luaopen_package},
          {(lt_coroutine & flags_), LUA_COLIBNAME,   luaopen_coroutine},
          {(lt_table & flags_),     LUA_TABLIBNAME,  luaopen_table},
          {(lt_io & flags_),        LUA_IOLIBNAME,   luaopen_io},
          {(lt_os & flags_),        LUA_OSLIBNAME,   luaopen_os},
          {(lt_string & flags_),    LUA_STRLIBNAME,  luaopen_string},
          {(lt_math & flags_),      LUA_MATHLIBNAME, luaopen_math},
          {(lt_utf8 & flags_),      LUA_UTF8LIBNAME, luaopen_utf8},
          {(lt_debug & flags_),     LUA_DBLIBNAME,   luaopen_debug},
          #if defined(LUA_COMPAT_BITLIB)
          {(lt_bit32 & flags_),     LUA_BITLIBNAME,  luaopen_bit32},
          #endif
          {false, nullptr, nullptr}
        };

        for(const table_lib& lib : libs)
        {
          if(lib.load)
          {
            luaL_requiref(state_, lib.name, lib.func, 1);
            lua_pop(state_, 1);
          }
        }
      }

    private:
      state_type              state_;      
      lookup_type             lookup_type_;
      global_module           module_;
      lua_lib_underlying_type flags_;
  };
}

#endif
