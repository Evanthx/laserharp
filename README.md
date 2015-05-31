## Laser Harp Code 

This is the code the drives the Arduino for my laser harp!

I wanted to unit test the code, but couldn't really do that in the Arduino editor. So I broke several of the methods out into individual files. I then made those a C++ library in Visual Studio 2013 and unit tested them. The library layout is thus slightly weird, but it worked.

The sln file is in the unit test directory - I wanted everything tucked away if possible. But the library files themselves had to be in the same directory as the sketch, so they are. Again, slightly weird, but it worked.