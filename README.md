# rooki
A stupid simple script runner supporting c, c++, rust, haskell and virtually anything

`Rooki` is written in less than 20 lines of `bash`, and has a very flexible
functionality.

# Quick start

For instance to write a `c++` script called `hello.cxx`
just create an executable file `hello.cxx` and write


```c++
#!/usr/bin/env rooki
// rooki:flags -pedantic -std=c++11
// rooki:flags -x c++
// rooki:spell g++ $flags $f -o $bin

#include <iostream>

int main(int argc, char *argv[])
{
  std::cout << "Hello world" << std::endl;
  return 0;
}
```

then do

```
./test.cxx
# or
rooki test.cxx
```

if you want to see exactly what rooki is doing then set the environment variable
`ROOKI_DEBUG` like this

```
ROOKI_DEBUG=1 ./test.cxx
```

