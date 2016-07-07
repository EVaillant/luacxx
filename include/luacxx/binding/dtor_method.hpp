#ifndef LUACXX_BINDING_DTOR_METHOD_HPP
# define LUACXX_BINDING_DTOR_METHOD_HPP

# include <luacxx/binding/callable.hpp>
# include <luacxx/core/type_info.hpp>

namespace luacxx
{
  template <class C> class dtor_method : public callable
  {
    public:
      typedef C                           class_type;
      typedef std::shared_ptr<class_type> smart_type;

      dtor_method(const common_class_type_info::self_smart_type& helper)
        : helper_(helper)
      {
      }

    protected:
      virtual int invoke_(state_type state) override
      {        
        std::pair<bool, common_class_type_info::class_field> ret = common_class_type_info::get_class_field(state, 1);
        if(ret.first && ret.second.id == toolsbox::type_uid::get<class_type>())
        {
          if(ret.second.ptr)
          {
            switch(ret.second.type)
            {
              case common_class_type_info::class_ptr_type::smart_ptr:
                ((smart_type*) ret.second.ptr)->~smart_type();
                break;

              case common_class_type_info::class_ptr_type::raw_ptr:
                // nothing to do
                break;
            }
          }
        }
        else
        {
          luaL_error(state, msg_error_object_corrupted);
        }
        return 0;
      }

    private:
      common_class_type_info::self_smart_type helper_;
  };
}


#endif
