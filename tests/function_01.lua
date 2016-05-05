assert(    fct1(5, "blabla"));
assert(not fct1(8, "blabla"));
assert(not fct1(5, "pouetpouet"));

local l = require("some.thing");
assert(    l.fct2(5, "blabla"));
assert(not l.fct2(8, "blabla"));
assert(not l.fct2(5, "pouetpouet"));
