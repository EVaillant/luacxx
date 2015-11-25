#ifndef LUACXX_REGISTER_TYPE_HH
# define LUACXX_REGISTER_TYPE_HH

# include <unordered_map>
# include <toolsbox/type_uid.hpp>
# include <toolsbox/any.hpp>

namespace luacxx
{
  struct TypeInfo
  {
    enum class Type
    {
      Enum,
      Class,
      Unknown
    };

    TypeInfo()
    {
      reset();
    }

    bool valid() const
    {
      return !converter.empty();
    }

    bool is_enum() const
    {
      return type == Type::Enum;
    }

    bool is_class() const
    {
      return type == Type::Class;
    }

    void reset()
    {
      type = Type::Unknown;
      converter.reset();
    }

    Type type;
    toolsbox::any converter;
  };

  class RegisterType
  {
    public:
      typedef toolsbox::type_uid::id_type type_id_t;

      template<class T> type_id_t get_type_id() const
      {
        return toolsbox::type_uid::get<T>();
      }

      template<class T> TypeInfo& get_type_info()
      {
        type_id_t id = get_type_id<T>();
        return type_info_[id];
      }

      template<class T> const TypeInfo& get_type_info() const
      {
        static TypeInfo null_type_info;
        type_id_t id = get_type_id<T>();
        auto it_type_info = type_info_.find(id);
        return ((it_type_info == type_info_.end()) ? null_type_info : it_type_info->second);
      }

    private:
      typedef std::unordered_map<type_id_t, TypeInfo> type_info_t;
      type_info_t type_info_;
  };
}

#endif
