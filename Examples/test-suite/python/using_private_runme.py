from using_private import *

f = FooBar()
f.x = 3

if f.blah(4) != 4:
    raise RuntimeError, "blah(int)"
