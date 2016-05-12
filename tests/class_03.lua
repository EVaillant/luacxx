local l1 = nil

assert(fct1(l1));
assert(fct2(l1));
assert(fct3(l1));
assert(fct4(l1));

local l2 = fct7();

assert(fct1(l2));
assert(fct2(l2));
assert(fct3(l2));
assert(fct4(l2));
