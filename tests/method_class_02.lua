local l1 = ClassA();
assert(l1:methdA() == 0);

local l2 = ClassB();
assert(l2:methdA() == 1);
assert(l2:methdB() == 0);

local l3 = ClassC();
assert(l3:methdA() == 2);
assert(l3:methdB() == 1);
assert(l3:methdC() == 0);
