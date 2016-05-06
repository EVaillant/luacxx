#ifndef LUACXX_BINDING_STACK_HPP
# define LUACXX_BINDING_STACK_HPP

# include <luacxx/core/type_traits.hpp>

# include <vector>
# include <map>
# include <sstream>
# include <memory>

# include <lua.hpp>

namespace luacxx
{
  enum class stack_item_type
  {
    string,
    boolean,
    number,
    integer,
    table,
    root,
    unknown
  };

  class stack_tree
  {
    public:
      inline virtual ~stack_tree()
      {
      }

      virtual stack_item_type get_type() const = 0;
      virtual void to_string(std::ostream& stream) const = 0;

      inline std::string to_string() const
      {
        std::ostringstream stream;
        this->to_string(stream);
        return stream.str();
      }
  };
  typedef std::unique_ptr<stack_tree> stack_tree_ptr;

  namespace detail
  {    
    template <class S, stack_item_type T> class stack_tree_impl : public stack_tree
    {
      public:
        typedef S store_type;

        stack_tree_impl(store_type&& data)
          : store_(std::forward<store_type&&>(data))
        {
        }

        virtual stack_item_type get_type() const override { return T; }

        virtual void to_string(std::ostream& stream) const override
        {
          switch(T)
          {
            case stack_item_type::string:
              stream << 's';
              break;

            case stack_item_type::boolean:
              stream << 'b';
              break;

            case stack_item_type::number:
              stream << 'n';
              break;

            case stack_item_type::integer:
              stream << 'i';
              break;

            case stack_item_type::table:
              stream << 't';
              break;

            case stack_item_type::root:
              break;

            case stack_item_type::unknown:
              stream << '?';
              break;
          };
          if(T != stack_item_type::root) stream << '\'';
          to_string_(stream, store_);
          if(T != stack_item_type::root) stream << '\'';
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

      private:
        store_type  store_;
    };

    typedef std::vector<std::pair<stack_tree_ptr, stack_tree_ptr>> stack_items;

    typedef stack_tree_impl<std::string, stack_item_type::string>  stack_string_item;
    typedef stack_tree_impl<bool,        stack_item_type::boolean> stack_bool_item;
    typedef stack_tree_impl<lua_Number,  stack_item_type::number>  stack_number_item;
    typedef stack_tree_impl<lua_Integer, stack_item_type::integer> stack_integer_item;
    typedef stack_tree_impl<stack_items, stack_item_type::table>   stack_table_item;
    typedef stack_tree_impl<stack_items, stack_item_type::root>    stack_root_item;
    typedef stack_tree_impl<void*,       stack_item_type::unknown> stack_unknown_item;

    inline stack_tree_ptr make_stack_item(state_type state, int i)
    {
      int t = lua_type(state, i);
      switch (t)
      {
        case LUA_TSTRING:  /* strings */
          return std::make_unique<stack_string_item>(lua_tostring(state, i));

        case LUA_TBOOLEAN:  /* booleans */
          return std::make_unique<stack_bool_item>(lua_toboolean(state, i));

        case LUA_TNUMBER:  /* numbers or integer */
          if(lua_isinteger(state, i))
          {
            return std::make_unique<stack_integer_item>(lua_tointeger(state, i));
          }
          else if(lua_isnumber(state, i))
          {
            return std::make_unique<stack_number_item>(lua_tonumber(state, i));
          }

        case LUA_TTABLE:  /* table */
          {
            std::map<std::string, std::pair<stack_tree_ptr, stack_tree_ptr>> ordered;
            lua_pushnil(state);
            while(lua_next(state, i))
            {
              lua_pushvalue(state, -2);
              std::string key;
              stack_tree_ptr first  = make_stack_item(state, lua_gettop(state));
              if(first)
              {
                key = first->to_string();
              }
              stack_tree_ptr second = make_stack_item(state, lua_gettop(state) - 1);
              if(second)
              {
                key = "|" + second->to_string();
              }
              ordered[key] = std::make_pair(std::move(first), std::move(second));
              lua_pop(state, 2);
            }

            stack_items items;
            items.reserve(ordered.size());
            for(auto& elt : ordered)
            {
              items.push_back(std::move(elt.second));
            }
            return std::make_unique<stack_table_item>(std::move(items));
          }

        default:  /* other values */
          return std::make_unique<stack_unknown_item>(nullptr);
      }
    }
  }

  inline stack_tree_ptr make_stack_tree(state_type state)
  {
    detail::stack_items items;

    int i;
    int top = lua_gettop(state);

    items.reserve(top);

    for (i = 1; i <= top; i++)
    {
      stack_tree_ptr item = detail::make_stack_item(state, i);
      items.push_back(std::make_pair(nullptr, std::move(item)));
    }

    stack_tree_ptr table = std::make_unique<detail::stack_root_item>(std::move(items));
    return table;
  }

  inline std::string dump_stack(state_type state)
  {
    return make_stack_tree(state)->to_string();
  }
}

#endif
