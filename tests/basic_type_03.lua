local s, v, i = fct3();
assert(table.unpack(s) == table.unpack({1, 9, 10, 50}));
assert(table.unpack(v) == table.unpack({2, 3}));
assert(table.unpack(i) == table.unpack({1, 2, 5, 9}));
