#ifndef LUACXX_BINDING_DTOR_METHOD_HPP
# define LUACXX_BINDING_DTOR_METHOD_HPP

# include <luacxx/binding/callable.hpp>
# include <luacxx/binding/utility.hpp>

namespace luacxx
{
  template <class C> class dtor_method : public callable
  {
    public:
      typedef C                           class_type;
      typedef std::shared_ptr<class_type> smart_type;

      dtor_method()
      {
      }

    protected:
      virtual int invoke_(state_type state) override
      {
        std::pair<bool, detail::class_ptr> ret = detail::get_class_ptr<class_type>(state, 1);
        if(ret.first && ret.second.id == toolsbox::type_uid::get<class_type>())
        {
          if(ret.second.ptr)
          {
            switch(ret.second.type)
            {
              case detail::class_ptr_type::smart_ptr:
                ((smart_type*) ret.second.ptr)->~smart_type();
                break;

              case detail::class_ptr_type::raw_ptr_with_delegate_owner:
                delete *(class_type**) ret.second.ptr;
                break;

              case detail::class_ptr_type::raw_ptr:
                // nothing to do
                break;
            }
          }
        }
        else
        {
          luaL_error(state, "unable to invoke destructor because private field is corrupted.");
        }
        return 0;
      }
  };
}


#endif
