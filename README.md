# Luacxx 

It is c++ lua binding (c++14)
* only header
* without rtti
* without exception

## Hello World

my_script.cc
```c++
#include <iostream>

#include <luacxx/binding/function.hpp>
#include <luacxx/binding/engine.hpp>

void hello()
{
  std::cout << "Hello World" << std::endl;
}

int main()
{
  luacxx::engine engine;
  luacxx::make_function(engine, "hello", hello);
  engine.bind();
  engine.do_file("my_script.lua");
  return 0;
}
```

my_script.lua
```lua
hello();
```

output
```shell
$ g++ -I/where/luacxx/installed/root/include -I/where/lua/installed/root/include -L/where/lua/installed/root/lib -llua my_prog.cc
$ ./a.out
Hello World
```

## What is supported ?

* [basic types](./doc/basic_types.md)
	* [stl containers](./doc/basic_types.md#containers)
	* [enum](./doc/basic_types.md#enum)
	* [special](./doc/basic_types.md#special)
* [class](./doc/class.md)
	* [constructor](./doc/class.md#ctor)
	* [destructor](./doc/class.md#dtor)
	* [method](./doc/class.md#method)
		* [const](./doc/class.md#const)
		* [static](./doc/class.md#static)
		* [overloading](./doc/class.md#overloading)
	* [property](./doc/class.md#property)
	* [inheritance](./doc/class.md#inheritance)
* [function](./doc/function.md)
* [opaque](./doc/opaque.md)
* [closure & caller](./doc/closure_and_caller.md)
* [global](./doc/global.md)
* [module](./doc/module.md)
* [helper](./doc/helper.md)
	* [instance_of](./doc/helper.md#instance_of)
	* [cast_to](./doc/helper.md#cast_to)
	* [logger](./doc/helper.md#logger)
	* [is_same_class_type](./doc/helper.md#is_same_class_type)
	* [is_same_class_object](./doc/helper.md#is_same_class_object)
* [others](./doc/others.md)
	* [engine](./doc/others.md#engine)
	* [convert policy](./doc/others.md#policy)
	* [auto in/out parameter](./doc/others.md#inout)

## Required

* c++ compiler with support c++11 / c++14
* [toolsbox](https://github.com/EVaillant/toolsbox)
* [cmake](https://cmake.org) 3.0 or highter
* [boost](http://boost.org) unit test (not mandatory, to run unit test)
* [lua](http://www.lua.org) 5.3 or highter

## Build (Unix)

By default :
* unit test is builded (if there are boost unit test)
* in debug mode

Common cmake option (to add on common line) :

 Option | Value | Default | Description
--------| ------|---------|------------
CMAKE_BUILD_TYPE | Debug or Release | Debug | Select build type
CMAKE_INSTALL_PREFIX | path | /usr/local | Prefix installation
DISABLE_UNITTEST | ON or OFF | OFF | Disable unittest

run cmake :
```shell
luacxx $ mkdir build && cd build
build  $ cmake ..
```

specify tu cmake where is lua :
```shell
luacxx $ mkdir build && cd build
build  $ LUA_DIR=/lua/root/dir cmake ..
```

build :
```shell
build $ make
```

run unit test :
```shell
build $ make test
```

install :
```shell
build $ make install
```

## Licence

cf [Boost Licence](http://www.boost.org/LICENSE_1_0.txt)
