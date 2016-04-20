#ifndef LUACXX_LOOKUP_TYPE_HPP
# define LUACXX_LOOKUP_TYPE_HPP

# include <toolsbox/type_uid.hpp>
# include <luacxx/type_info.hpp>
# include <unordered_map>

namespace luacxx
{
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

      template <class T> void set(const std::shared_ptr<type_info<T>>& ti)
      {
        infos_[toolsbox::type_uid::get<T>()] = ti;
      }

      template <class T> void reset()
      {
        infos_.erase(toolsbox::type_uid::get<T>());
      }

      template <class T> bool exist() const
      {
        id_type id = toolsbox::type_uid::get<T>();
        return infos_.find(id) != infos_.end();
      }

    private:
      typedef toolsbox::type_uid::id_type                                    id_type;
      typedef std::unordered_map<id_type, std::shared_ptr<common_type_info>> infos_type;

      infos_type infos_;
  };
}

#endif
