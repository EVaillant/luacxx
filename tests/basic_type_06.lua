local m = fct6();
local count = 0
for _ in pairs(m) do count = count + 1 end
assert(count == 2)
assert(m[4] == "aaa")
assert(m[9] == "yy")
