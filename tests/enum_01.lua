local v1 = Enum1.VAL1
local v2 = Enum1.VAL2
local v3 = Enum1.VAL3

assert(v1 == 0, "failed")
assert(v2 == 1, "failed")
assert(v3 == 2, "failed")


local v4 = Enum2.VAL1
local v5 = Enum2.VAL2
local v6 = Enum2.VAL3

assert(v4 == "VAL1", "failed")
assert(v5 == "VAL2", "failed")
assert(v6 == "VAL3", "failed")
