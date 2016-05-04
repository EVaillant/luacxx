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

      inline lookup_type& get_lookup_type()
      {
        return lookup_type_;
      }

      inline void bind()
      {
        assert(!state_);
        state_ = luaL_newstate();
        fill_lookup_type_();
        open_lib_();
        module_.bind(state_);
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

    protected:
      template <class T0, class ... T> void fill_lookup_type_integer_(typename std::enable_if<(sizeof...(T) != 0), int>::type = 0)
      {
        fill_lookup_type_integer_<T0>();
        fill_lookup_type_integer_<T...>();
      }

      template <class T> void fill_lookup_type_integer_()
      {
        lookup_type_.set<T>(std::make_shared<luacxx::integer_type_info<T>>());
      }

      inline void fill_lookup_type_()
      {
        lookup_type_.set<bool>(std::make_shared<luacxx::bool_type_info>());
        lookup_type_.set<std::string>(std::make_shared<luacxx::string_type_info<std::string>>());
        lookup_type_.set<const char*>(std::make_shared<luacxx::string_type_info<const char*>>());
        lookup_type_.set<float>(std::make_shared<luacxx::number_type_info<float>>());
        lookup_type_.set<double>(std::make_shared<luacxx::number_type_info<double>>());
        lookup_type_.set<long double>(std::make_shared<luacxx::number_type_info<long double>>());
        fill_lookup_type_integer_<uint8_t, int8_t, uint16_t, int16_t, uint32_t, int32_t, uint64_t, int64_t>();
      }

      inline void open_lib_()
      {
        luaopen_base(state_);
        if(lt_coroutine & flags_) luaopen_coroutine(state_);
        if(lt_table & flags_)     luaopen_table(state_);
        if(lt_io & flags_)        luaopen_io(state_);
        if(lt_os & flags_)        luaopen_os(state_);
        if(lt_string & flags_)    luaopen_string(state_);
        if(lt_utf8 & flags_)      luaopen_utf8(state_);
        if(lt_bit32 & flags_)     luaopen_bit32(state_);
        if(lt_math & flags_)      luaopen_math(state_);
        if(lt_debug & flags_)     luaopen_debug(state_);
        if(lt_package & flags_)   luaopen_package(state_);
      }

    private:
      state_type              state_;
      global_module           module_;
      lookup_type             lookup_type_;
      lua_lib_underlying_type flags_;
  };
}

#endif
