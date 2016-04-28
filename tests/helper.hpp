#ifndef LUACXX_TEST_HELPER_HPP
# define  LUACXX_TEST_HELPER_HPP

# include <lua.hpp>

# include <sstream>
# include <memory>
# include <vector>
# include <map>

class lua_state_guard
{
  public:
    inline lua_state_guard()
    {
      state_ = luaL_newstate();
    }

    inline operator lua_State*()
    {
      return state_;
    }

    inline ~lua_state_guard()
    {
      lua_close(state_);
    }
  private:
    lua_State* state_;
};

enum class lua_stack_item_type
{
  string,
  boolean,
  number,
  integer,
  table,
  root,
  unknown
};

class lua_stack_item
{
  public:
    virtual ~lua_stack_item() {}

    virtual lua_stack_item_type get_type() const = 0;
    virtual bool equals(const lua_stack_item& item) const = 0;
    virtual void to_string(std::ostream& stream) const = 0;

    std::string to_string() const
    {
      std::ostringstream stream;
      this->to_string(stream);
      return stream.str();
    }
};

template <class S, lua_stack_item_type T> class lua_stack_item_impl : public lua_stack_item
{
  public:
    typedef S store_type;

    lua_stack_item_impl(store_type&& data)
      : store_(std::forward<store_type&&>(data))
    {
    }

    virtual lua_stack_item_type get_type() const override { return T; }

    virtual bool equals(const lua_stack_item& item) const override
    {
      if(get_type() != item.get_type())
      {
        return false;
      }

      const lua_stack_item_impl& item_impl = static_cast<const lua_stack_item_impl&>(item);
      return equals_(store_, item_impl.store_);
    }

    virtual void to_string(std::ostream& stream) const override
    {
      switch(T)
      {
        case lua_stack_item_type::string:
          stream << 's';
          break;

        case lua_stack_item_type::boolean:
          stream << 'b';
          break;

        case lua_stack_item_type::number:
          stream << 'n';
          break;

        case lua_stack_item_type::integer:
          stream << 'i';
          break;

        case lua_stack_item_type::table:
          stream << 't';
          break;

        case lua_stack_item_type::root:
          break;

        case lua_stack_item_type::unknown:
          stream << '?';
          break;
      };
      if(T != lua_stack_item_type::root) stream << '\'';
      to_string_(stream, store_);
      if(T != lua_stack_item_type::root) stream << '\'';
    }

  protected:
    template <class I> void to_string_(std::ostream& stream, const std::vector<I>& items) const
    {
      for(const auto& item : items)
      {
        if(item.first && item.second)
        {
          stream << '(';
          item.first->to_string(stream);
          stream << ',';
          item.second->to_string(stream);
          stream << ')';
        }
        else if(item.second)
        {
          item.second->to_string(stream);
        }
        else if(item.first)
        {
          item.first->to_string(stream);
        }
      }
    }

    void to_string_(std::ostream& stream, const bool& item) const
    {
      stream << (item ? "true" : "false");
    }

    template <class I> void to_string_(std::ostream& stream, const I& item) const
    {
      stream << item;
    }

    template <class I> bool equals_(const I& left, const I& right) const
    {
      return left == right;
    }

    template <class I> bool equals_(const std::vector<I>& left, const std::vector<I>& right) const
    {
      if(left.size() != right.size())
      {
        return false;
      }

      for(const auto& item1 : left)
      {
        bool found = false;
        for(const auto& item2 : right)
        {
          bool found_first = false;
          if(item1.first && item2.first)
          {
            found_first = item1.first->equals(*item2.first);
          }
          else if(!item1.first && !item2.first)
          {
            found_first = true;
          }

          bool found_second = false;
          if(item1.second && item2.second)
          {
            found_second = item1.second->equals(*item2.second);
          }
          else if(!item1.second && !item2.second)
          {
            found_second = true;
          }

          found = found_first && found_second;
          if(found)
          {
            break;
          }
        }
        if(!found)
        {
          return false;
        }
      }
      return true;
    }

  private:
    store_type  store_;
};

typedef std::unique_ptr<lua_stack_item> lua_stack_item_ptr;
typedef std::vector<std::pair<lua_stack_item_ptr, lua_stack_item_ptr>> lua_stack_items;

typedef lua_stack_item_impl<std::string,     lua_stack_item_type::string>  lua_stack_string_item;
typedef lua_stack_item_impl<bool,            lua_stack_item_type::boolean> lua_stack_bool_item;
typedef lua_stack_item_impl<lua_Number,      lua_stack_item_type::number>  lua_stack_number_item;
typedef lua_stack_item_impl<lua_Integer,     lua_stack_item_type::integer> lua_stack_integer_item;
typedef lua_stack_item_impl<lua_stack_items, lua_stack_item_type::table>   lua_stack_table_item;
typedef lua_stack_item_impl<lua_stack_items, lua_stack_item_type::root>    lua_stack_root_item;
typedef lua_stack_item_impl<void*,           lua_stack_item_type::unknown> lua_stack_unknown_item;

inline lua_stack_item_ptr make_lua_stack_item(lua_State *state, int i)
{
  int t = lua_type(state, i);
  switch (t)
  {
    case LUA_TSTRING:  /* strings */
      return std::make_unique<lua_stack_string_item>(lua_tostring(state, i));

    case LUA_TBOOLEAN:  /* booleans */
      return std::make_unique<lua_stack_bool_item>(lua_toboolean(state, i));

    case LUA_TNUMBER:  /* numbers or integer */
      if(lua_isinteger(state, i))
      {
        return std::make_unique<lua_stack_integer_item>(lua_tointeger(state, i));
      }
      else if(lua_isnumber(state, i))
      {
        return std::make_unique<lua_stack_number_item>(lua_tonumber(state, i));
      }

    case LUA_TTABLE:  /* table */
      {
        std::map<std::string, std::pair<lua_stack_item_ptr, lua_stack_item_ptr>> ordered;
        lua_pushnil(state);
        while(lua_next(state, i))
        {
          lua_pushvalue(state, -2);
          std::string key;
          lua_stack_item_ptr first  = make_lua_stack_item(state, lua_gettop(state));
          if(first)
          {
            key = first->to_string();
          }
          lua_stack_item_ptr second = make_lua_stack_item(state, lua_gettop(state) - 1);
          if(second)
          {
            key = "|" + second->to_string();
          }
          ordered[key] = std::make_pair(std::move(first), std::move(second));
          lua_pop(state, 2);
        }

        lua_stack_items items;
        items.reserve(ordered.size());
        for(auto& elt : ordered)
        {
          items.push_back(std::move(elt.second));
        }
        return std::make_unique<lua_stack_table_item>(std::move(items));
      }

    default:  /* other values */
      return std::make_unique<lua_stack_unknown_item>(nullptr);
  }
}

inline lua_stack_item_ptr make_lua_stack_item(lua_State *state)
{
  lua_stack_items    items;

  int i;
  int top = lua_gettop(state);

  items.reserve(top);

  for (i = 1; i <= top; i++)
  {
    lua_stack_item_ptr item = make_lua_stack_item(state, i);
    items.push_back(std::make_pair(nullptr, std::move(item)));
  }

  lua_stack_item_ptr table = std::make_unique<lua_stack_root_item>(std::move(items));
  return table;
}

inline std::string lua_stack_dump(lua_State *state)
{
  return make_lua_stack_item(state)->to_string();
}

#endif
