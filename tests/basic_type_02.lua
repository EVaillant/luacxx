local f, d, i, s, b = fct2();

assert( not b );
assert( s == "fct2" );
assert( i == 7 );
assert( math.abs(d - 5.6) < 1e-6 );
assert( math.abs(f - 8.9) < 1e-6 );
