local l1 = Data();
assert(l1:get_ref() == 0);
assert(l1.field1 == 0);
assert(l1.field2 == 0);
assert(l1.field3 == 0);
assert(l1.field4 == 0);
l1.field1 = 5;
assert(l1:get_ref() == 5);
assert(l1.field1 == 5);
assert(l1.field2 == 5);
assert(l1.field3 == 5);
assert(l1.field4 == 5);

local l2 = Data();
assert(l2:get_ref() == 0);
assert(l2.field1 == 0);
assert(l2.field2 == 0);
assert(l2.field3 == 0);
assert(l2.field4 == 0);
l2.field2 = 5;
assert(l2:get_ref() == 5);
assert(l2.field1 == 5);
assert(l2.field2 == 5);
assert(l2.field3 == 5);
assert(l2.field4 == 5);

local l3 = Data();
assert(l3:get_ref() == 0);
assert(l3.field1 == 0);
assert(l3.field2 == 0);
assert(l3.field3 == 0);
assert(l3.field4 == 0);
l3.field3 = 5;
assert(l3:get_ref() == 5);
assert(l3.field1 == 5);
assert(l3.field2 == 5);
assert(l3.field3 == 5);
assert(l3.field4 == 5);

local l4 = Data();
assert(l4:get_ref() == 0);
assert(l4.field1 == 0);
assert(l4.field2 == 0);
assert(l4.field3 == 0);
assert(l4.field4 == 0);
l4.field4 = 5;
assert(l4:get_ref() == 5);
assert(l4.field1 == 5);
assert(l4.field2 == 5);
assert(l4.field3 == 5);
assert(l4.field4 == 5);

