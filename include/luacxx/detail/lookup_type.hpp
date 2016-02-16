#ifndef LUACXX_DETAIL_LOOKUP_TYPE_HPP
# define LUACXX_DETAIL_LOOKUP_TYPE_HPP

# include <toolsbox/type_uid.hpp>
# include <toolsbox/any.hpp>

# include <luacxx/parameter_policy.hpp>
# include <luacxx/error_msg.hpp>

# include <unordered_map>

namespace luacxx
{
  namespace detail
  {
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

        common_type_info(underlying_type ut, bool valid)
          : underlying_type_(ut)
          , valid_(valid)
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
          return valid_;
        }

        virtual void to_lua(lua_State *state, const variable_type& var, std::string &error_msg, const policy_node& policy) const = 0;
        virtual void from_lua(lua_State *state, std::size_t idx, variable_type& var, std::string& error_msg, const policy_node& policy) const = 0;
        virtual variable_type default_initializer() const = 0;

      protected:
        underlying_type underlying_type_;
        bool valid_;
    };

    template <class T> class type_info : public common_type_info
    {
      public:
        typedef T type;

        type_info(underlying_type ut, bool valid)
          : common_type_info(ut, valid)
        {
        }
    };

    template <class T> class empty_type_info : public type_info<T>
    {
      public:
        empty_type_info()
          : type_info<T>(common_type_info::underlying_type::Unknown, true)
        {
        }

        virtual void to_lua(lua_State*, const common_type_info::variable_type&, std::string &error_msg, const policy_node&) const override
        {
          error_msg = msg_error_type_not_supported;
        }

        virtual void from_lua(lua_State*, std::size_t, common_type_info::variable_type& , std::string& error_msg, const policy_node&) const override
        {
          error_msg = msg_error_type_not_supported;
        }

        virtual common_type_info::variable_type default_initializer() const
        {
          return common_type_info::variable_type();
        }
    };

    class lookup_type
    {
      public:
        template <class T> const type_info<T>& get() const
        {
          static empty_type_info<T> empty;
          id_type id = toolsbox::type_uid::get<T>();
          infos_type::const_iterator it = infos_.find(id);
          return (it == infos_.end() ? empty : *static_cast<type_info<T>*>(it->second.get()));
        }

        template <class T> void set(std::unique_ptr<type_info<T>> ti)
        {
          infos_[toolsbox::type_uid::get<T>()] = std::move(ti);
        }

        template <class T> void reset()
        {
          infos_.erase(toolsbox::type_uid::get<T>());
        }

      private:
        typedef toolsbox::type_uid::id_type id_type;
        typedef std::unordered_map<id_type, std::unique_ptr<common_type_info>> infos_type;

        infos_type infos_;
    };
  }
}

#endif

