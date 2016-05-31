local l = create_opaque_data();
assert(get_opaque_data(l) == 0);
inc_opaque_data(l);
assert(get_opaque_data(l) == 1);
dec_opaque_data(l);
assert(get_opaque_data(l) == 0);
delate_paque_data(l);
