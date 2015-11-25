local p = Cls01();

assert(p:get_e() == -1, "failed");

p:set_enum1(Enum1.VAL1);
assert(p:get_e() == 0, "failed");

p:set_enum2(Enum1.VAL2);
assert(p:get_e() == 1, "failed");

p:set_enum3(Enum1.VAL3);
assert(p:get_e() == 2, "failed");
