#ifndef LUACXX_TEST_INCOMPLETE_DATA_HPP
# define  LUACXX_TEST_INCOMPLETE_DATA_HPP

struct opaque_data;
typedef opaque_data* opaque_type;

opaque_type create_opaque_data();
void delate_paque_data(opaque_type);

void inc_opaque_data(opaque_type);
void dec_opaque_data(opaque_type);
int  get_opaque_data(opaque_type);

#endif
