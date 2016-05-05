assert(table.unpack(vec) == table.unpack({5, 9, 15}));

local count = 0
for _ in pairs(map) do count = count + 1 end
assert(count == 2);
assert(map[9] == "yhyh");
assert(map[20] == "uyztgui");
