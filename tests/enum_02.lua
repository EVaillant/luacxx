assert(get_e() == -1, "failed");

set_enum1(Enum1.VAL1);
assert(get_e() == 0, "failed");

set_enum2(Enum1.VAL2);
assert(get_e() == 1, "failed");

set_enum3(Enum1.VAL3);
assert(get_e() == 2, "failed");
