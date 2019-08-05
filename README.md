# rooki
A stupid simple script runner supporting c, c++, rust, haskell and virtually anything

`Rooki` is written in less than 20 lines of `bash`, and has a very flexible
functionality.

# How does it work?

You just write comments as `rooki` instructions, the only instruction that
you have to define is the `rooki:spell`, which will be run in the shell.
Otherwise you can define any flag you want with the name you want for instance
in a `c` file, the following
```c
// rooki:someflag -I$(pwd)
// rooki:someflag -I$HOME
```
will define a shell variable `someflag` with the value
```
someflag="-I$(pwd) -I$HOME"
```
which later can be used in the `rooki:spell` section as
```c
// rooki:someflag -I$(pwd)
// rooki:someflag -I$HOME
// rooki:spell cc $someflag $f -o $bin
```

There are some general variables already defined:

| Variable | Explanation | Example |
| --- | --- | :---: |
| `bin` | Path to the created binary | `rooki:spell gcc $f -o $bin` |
| `f` | Path to the temporary source file that will be compiled | `// rooki:spell gcc $f -o $bin` |
| `src` | Path to the source file, the caller | `rooki:include -I$(dirname $src)/include`|
| `config_folder` | Path to rooki config folder ||

What `rooki` does when you do `rooki yourscript` is the following
- Create a temporal file and save the path in the variable `f`.
- Create a `bin` path out of the `md5` hash of the source script, the bin
  will be stored in `$XDG_CONFIG_HOME/rooki/`. In the case that your
  `XDG_CONFIG_HOME` is not defined, it will be stored in
  `~/.config/rooki/`.
- Remove the shbang (`#!/usr/bin/env rooki`) on top of your script (if there is
  any) and copy the script to `$f` without the shbang.
- Read the `rooki:` flags stored in text in your script.
- Expand shell constructs within these flags, environment variables and
  general shell commands within.
- Run the `rooki:spell` construct, which should be creating a binary
  in the path `$bin`.

# Quick start

For instance to write a `c++` script called `hello.cxx`
just create an executable file `hello.cxx` and write


```c++
#!/usr/bin/env rooki
// rooki:flags -pedantic -std=c++11
// rooki:flags -x c++
// rooki:include -I/usr/include
// rooki:include -I$HOME/.local/include
// rooki:spell g++ $flags $include $f -o $bin

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

