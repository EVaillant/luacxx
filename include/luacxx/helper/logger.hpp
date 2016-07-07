#ifndef LUACXX_HELPER_LOGGER_HPP
# define LUACXX_HELPER_LOGGER_HPP

# include <luacxx/binding/function.hpp>
# include <luacxx/core/type_traits.hpp>
# include <lua.hpp>

namespace luacxx
{
  namespace detail
  {
    template <class F> void logger(state_type state, F functor, const std::string& message)
    {
      lua_Debug info;
      if(lua_getstack(state, 1, &info))
      {
        lua_getinfo(state, "nSl", &info);
        functor(info.short_src, info.currentline, message);
      }
      else
      {
        functor(nullptr, -1, message);
      }
    }

    template <class Stream, class Level> class basic_stream_logger
    {
      public:
        typedef Level  level_type;
        typedef Stream stream_type;

        basic_stream_logger(stream_type& stream, level_type level)
          : level_(level)
          , stream_(stream)
        {
        }

        void operator()(const char* filename, int line, const std::string& message)
        {
          stream_ << level_;
          if(filename)
          {
            stream_ << ":" << filename << "(" << line << "):";
          }
          stream_ << message << std::endl;
        }

      private:
        level_type   level_;
        stream_type& stream_;
    };
  }

  template <class Stream, class Level> auto make_basic_stream_logger(Stream& stream, Level level)
  {
    return detail::basic_stream_logger<Stream, Level>(stream, level);
  }

  template <class F> auto& make_logger(engine& e, const std::string& module_name, const std::string& name, F functor)
  {
    using namespace std::placeholders;
    std::function<void (state_type, const std::string&)> wrapper = std::bind(detail::logger<F>, _1, std::forward<F>(functor), _2);
    return make_function(e, module_name, name, wrapper);
  }

  template <class F> auto& make_logger(engine& e, const std::string& name, F functor)
  {
    return make_logger(e, root_module_name, name, std::forward<F>(functor));
  }
}

#endif
