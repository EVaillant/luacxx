#ifndef LUACXX_DETAIL_FWD_HH
# define LUACXX_DETAIL_FWD_HH

namespace luacxx
{
  template <class T> class Class;

  template <class T, class R, class ... ARGS> class Policy;

  template <class P, class T> class output_parameter_policy;
  template <class P, class T> class input_parameter_policy;
}

#endif
