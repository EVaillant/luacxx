local l  = Cls01()

local r1 = l:super_mth1()
assert(r1 == 5 + 15, "failed")
assert(l:get_str() == "blabla")

l:set_str("")
assert(l:get_str() == "")

local r2 = l:super_mth1(4)
assert(r2 == 4 + 15, "failed")
assert(l:get_str() == "blabla")

l:set_str("")
assert(l:get_str() == "")

local r3 = l:super_mth1(4, 10)
assert(r3 == 4 + 10, "failed")
assert(l:get_str() == "blabla")

l:set_str("")
assert(l:get_str() == "")

local r4 = l:super_mth1(4, 10, "rr")
assert(r4 == 4 + 10, "failed")
assert(l:get_str() == "rr")

l:set_str("")
assert(l:get_str() == "")

local r5 = l:super_mth1_wo_return(4, 10, "rr")
assert(r5 == nil, "failed")
assert(l:get_str() == "rr")

local r6, r7 = l:super_mth2(5, 6)
assert(r6 == 5 + 6, "failed")
assert(r7 == "truc", "failed")


local r8, r9 = l:super_mth3()
assert(r8 == 5, "failed")
assert(r9 == "truc", "failed")
