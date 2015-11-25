#ifndef LUACXX_DETAIL_ENUM_TO_NATIVE_VALUE_HH
# define LUACXX_DETAIL_ENUM_TO_NATIVE_VALUE_HH

# include <sstream>
# include <string>

namespace luacxx
{
  namespace detail
  {
    template <class E, class C> struct enum_to_native_value
    {
      static C convert(const E &input)
      {
        return static_cast<C>(input);
      }
    };

    template <class E> struct enum_to_native_value<E, std::string>
    {
      static std::string convert(const E &input)
      {
        std::ostringstream stream;
        stream << input;
        return stream.str();
      }
    };
  }
}

#endif
