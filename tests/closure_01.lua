function fct(x)
  assert(x == "ooo");
  return 5;
end

--assert(fct1(fct));

local p = fct2("uiuiu");

assert( not p("ee") );
assert(     p("uiuiu") );
