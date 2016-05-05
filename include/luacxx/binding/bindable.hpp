#ifndef LUACXX_BINDING_BINDABLE_HPP
# define LUACXX_BINDING_BINDABLE_HPP

# include <luacxx/core/type_traits.hpp>

namespace luacxx
{
  class bindable
  {
    public:
      inline bindable()
      {
      }

      inline virtual ~bindable()
      {
      }

      bindable(const bindable&) = delete;
      bindable(bindable&&) = delete;
      bindable& operator=(const bindable&) = delete;
      bindable& operator=(bindable&&) = delete;

      virtual bool bind(state_type state) = 0;
  };
}

#endif
