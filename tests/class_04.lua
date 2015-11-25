local cls1_1 = Cls01();
local cls1_2 = Cls01();
local cls1_3 = Cls01();

local cls3 = Cls03();

assert(not cls3:is_ref(cls1_1), "failed")

cls3:in_ptr1(cls1_1);
assert(    cls3:is_ref(cls1_1), "failed")

cls3:in_ptr2(cls1_2);
assert(    cls3:is_ref(cls1_2), "failed")

cls3:in_ptr3(cls1_3);
assert(    cls3:is_ref(cls1_3), "failed")
