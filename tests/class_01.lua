local l = Cls01()
assert(l:get_x() == 0, "failed")
l:set_x(5)
assert(l:get_x() == 5, "failed")
