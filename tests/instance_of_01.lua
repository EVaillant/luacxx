local c = Child();
local f = Father();
local o = Other();

assert(Child.instance_of(c));
assert(not Child.instance_of(f));
assert(not Child.instance_of(o));

assert(Father.instance_of(c));
assert(Father.instance_of(f));
assert(not Father.instance_of(o));

assert(not Other.instance_of(c));
assert(not Other.instance_of(f));
assert(Other.instance_of(o));
