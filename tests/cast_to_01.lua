local l1 = make(1);
local l2 = make(2);

assert(Father.instance_of(l1));
assert(Father.instance_of(l2));

assert(l1:get_id() == 1);
assert(l2:get_id() == 2);

local l4 = Child1.cast_to(l1);
local l5 = Child2.cast_to(l2);

assert(l4 ~= nil);
assert(l5 ~= nil);
assert(l4:get_id() == 1);
assert(l5:get_id() == 2);
assert(Child1.instance_of(l4));
assert(Child2.instance_of(l5));
