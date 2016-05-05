# Luacxx 

It is c++ lua binding (c++11 / c++14)
* depends on lua and toolsbox
* without rtti
* without exception

## What can i do with this library ?

TODO

## Required

* c++ compiler with support c++11 / c++14
* [toolsbox](https://github.com/EVaillant/toolsbox)
* [cmake](https://cmake.org/) 2.6 or highter
* [boost](http://boost.org) unit test (not mandatory, to run unit test)
* [lua](http://www.lua.org/) 5.3 or highter

## Build (Unix)

By default :
* unit test is builded (if there are boost unit test)
* in debug mode

Common cmake option (to add on common line) :

 Option | Value | Default | Description
--------| ------|---------|------------
CMAKE_BUILD_TYPE | Debug or Release | Debug | Select build type
CMAKE_INSTALL_PREFIX | path | /usr/local | Prefix installation

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
