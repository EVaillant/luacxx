#include "incomplete_data.hpp"

struct opaque_data
{
  int value;
};

opaque_type create_opaque_data()
{
  return new opaque_data{0};
}

void delate_paque_data(opaque_type t)
{
  delete t;
}

void inc_opaque_data(opaque_type t)
{
  t->value += 1;
}

void dec_opaque_data(opaque_type t)
{
  t->value -= 1;
}

int  get_opaque_data(opaque_type t)
{
  return t->value;
}
