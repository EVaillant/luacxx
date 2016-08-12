## Basic Types

c++         | lua     | Remarque
------------| --------|---------
bool        | boolean |
std::string | string  |
const char* | string  | only c++ to lua
float       | number  |
double      | number  |
long double | number  |
uint8_t     | integer |
int8_t      | integer |
uint16_t    | integer |
int16_t     | integer |
uint32_t    | integer |
int32_t     | integer |
uint64_t    | integer |
int64_t     | integer |

### <a name="containers"></a> STL Containers

STL Containers are converted to native Lua container.

c++         | lua     
------------| ------------------
std::vector | Table as array
std::list   | Table as array
std::set    | Table as array
std::map    | Table as associated array

**Warning**, all elements in the container will be copied. So if the container contains 100...00 elements, it can be long.

### <a name="enum"></a> Enum

All enum (c or c++11) style are supported.

my_script.cc
```c++
#include <iostream> 

#include <luacxx/binding/function.hpp>
#include <luacxx/binding/engine.hpp>
#include <luacxx/binding/enum.hpp>

enum class Enum1
{
  VAL1,
  VAL2,
  VAL3
};

enum Enum2
{
  e2VAL1,
  e2VAL2,
  e2VAL3   
};

int main()
{
  luacxx::engine engine;

  luacxx::make_enum<Enum1>(engine, "Enum1")
      .value("VAL1", Enum1::VAL1)
      .value("VAL2", Enum1::VAL2)
      .value("VAL3", Enum1::VAL3);

  luacxx::make_enum<Enum2>(engine, "some.thing", "Enum2")
      .value("VAL1", e2VAL1)
      .value("VAL2", e2VAL2)
      .value("VAL3", e2VAL3);

  engine.bind();
  engine.do_file("my_script.lua");
  return 0;
}

```

my_script.lua
```lua
local v1 = Enum1.VAL1;
local v2 = Enum1.VAL2;
local v3 = Enum1.VAL3;

print(v1);
print(v2);
print(v3);

local l = require("some.thing");
local v4 = l.Enum2.VAL1;
local v5 = l.Enum2.VAL2;
local v6 = l.Enum2.VAL3;

print(v4);
print(v5);
print(v6);
```

output
```shell
0
1
2
0
1
2
```

By default underlying type of *Enum1.VAL1* is returned by 
[std::underlying_type_t](http://en.cppreference.com/w/cpp/types/underlying_type). It possible to override it :
```c++
#include <iostream> 

#include <luacxx/binding/function.hpp>
#include <luacxx/binding/engine.hpp>
#include <luacxx/binding/enum.hpp>

enum class Enum1
{
  VAL1,
  VAL2,
  VAL3
};

std::ostream& operator<<(std::ostream& stream, const Enum1 &p)
{
  switch(p)
  {
    case Enum1::VAL1:
      stream << "VAL1";
      break;

    case Enum1::VAL2:
      stream << "VAL2";
      break;

    case Enum1::VAL3:
     stream << "VAL3";
     break;
  }
  return stream;
}

int main()
{
  luacxx::engine engine;

  luacxx::make_enum<Enum1, std::string>(engine, "Enum1")
      .value("VAL1", Enum1::VAL1)
      .value("VAL2", Enum1::VAL2)
      .value("VAL3", Enum1::VAL3);

  engine.bind();
  engine.do_file("my_script.lua");
  return 0;
}

```

my_script.lua
```lua
local v1 = Enum1.VAL1;
local v2 = Enum1.VAL2;
local v3 = Enum1.VAL3;

print(v1);
print(v2);
print(v3);
```

output
```shell
VAL1
VAL2
VAL3
```

### <a name="special"></a> Special

#### luacxx::state_type / lua_State

Sometime, you need to have lua state machine in the binding method :
```c++
void my_function(lua_State* state, int o)
{
  // do lot of complex stuff :)
}

// binding
{
  luacxx::make_function(engine, "my_function", my_function);
}
```

lua:
```lua
my_function(5);
```