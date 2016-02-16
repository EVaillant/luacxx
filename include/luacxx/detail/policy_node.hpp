#ifndef LUACXX_DETAIL_POLICY_NODE_HPP
# define LUACXX_DETAIL_POLICY_NODE_HPP

# include <unordered_map>
# include <string>
# include <memory>

# include <toolsbox/any.hpp>

# include <luacxx/parameter_policy.hpp>

namespace luacxx
{
  namespace detail
  {
    class policy_node
    {
      public:
        typedef std::unordered_map<std::string, std::unique_ptr<policy_node>> sub_node_type;
        typedef toolsbox::any                                                 value_type;

        const policy_node& get_sub_node(const std::string& name) const
        {
          sub_node_type::const_iterator it = nodes_.find(name);
          assert(it != nodes_.end());
          return *it->second;
        }

        template <class T> const parameter_policy<T>& get_parameter() const
        {
          typedef parameter_policy<T> parameter_policy_type;
          assert(data_.is<parameter_policy_type>());
          return data_.as<parameter_policy_type>();
        }

        policy_node& get_or_create_sub_node(const std::string& name)
        {
          sub_node_type::iterator it = nodes_.find(name);
          if(it == nodes_.end())
          {
            it = nodes_.insert(std::make_pair(name, std::unique_ptr<policy_node>(new policy_node))).first;
          }
          return *it->second;
        }

        template <class T> parameter_policy<T>& get_or_create_parameter()
        {
          if(data_.empty())
          {
            data_ = make_parameter_policy<T>();
          }
          return data_.as<parameter_policy<T>>();
        }

      private:
        sub_node_type nodes_;
        value_type    data_;
    };
  }
}

#endif

