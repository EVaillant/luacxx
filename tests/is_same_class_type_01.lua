local d11 = Data1();
local d12 = Data1();
local d21 = Data2();

assert(is_same_type(d11, d12));
assert(is_same_type(d11, d11));
assert(not is_same_type(d11, d21));
