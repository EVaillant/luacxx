local l1 = DerivatedA();
assert(fct1(l1));
assert(fct2(l1));

local l2 = DerivatedDerivatedA();
assert(fct1(l2));
assert(fct2(l2));
assert(fct3(l2));

local l3 = DerivatedDerivatedAB();
assert(fct1(l2));
assert(fct2(l2));
assert(fct4(l2));


