## Class
```c++
#include <luacxx/binding/class.hpp>

class Test
{
};

int main()
{
  luacxx::engine engine;
  luacxx::make_class<Test>(engine, "Test");
  engine.bind();
  return 0;
}
```
### <a name="ctor"></a> Constructor
Constructor is optional and only one can be declared but it is mandatory to make an instance of the class in lua :
```c++
#include <luacxx/binding/class.hpp>

class Test1
{
  public:
    Test1() {}
};

class Test2
{
  public:
    Test2(int, const std::string&) {}
};

int main()
{
  luacxx::engine engine;

  luacxx::make_class<Test1>(engine, "Test1")
    .ctor<>();

  luacxx::make_class<Test2>(engine, "Test2")
    .ctor<int, const std::string&>();

  engine.bind();

  return 0;
}
```
### <a name="dtor"></a> Destructor
The destructor is automaticly added... so need special declaration.
### <a name="method"></a> Method
```c++
#include <luacxx/binding/class.hpp>

class Test
{
  public:
    Test() {}

    void method1()
    {
    }
};

void extern_method2(Test&, int)
{
}

int main()
{
  luacxx::engine engine;
  luacxx::make_class<Test>(engine, "Test")
    .ctor<>()
    .method("method1", &Test::method1)
    .extern_method("method2", extern_method2)
    .extern_method("method3", std::function<void (Test&, int)>(extern_method2));
  engine.bind();
  return 0;
}
```
Two c++ methods can have same lua name. 
#### <a name="const"></a> const
```c++
#include <luacxx/binding/class.hpp>

class Test
{
  public:
    Test() {}

    void method1() const
    {
    }
};

int main()
{
  luacxx::engine engine;
  luacxx::make_class<Test>(engine, "Test")
    .ctor<>()
    .const_method("method1", &Test::method1);
  engine.bind();
  return 0;
}
```
#### <a name="static"></a> static
```c++
#include <luacxx/binding/class.hpp>

class Test
{
  public:
    Test() {}

    static void method1()
    {
    }
};

void extern_method2(int)
{
}

int main()
{
  luacxx::engine engine;
  luacxx::make_class<Test>(engine, "Test")
    .ctor<>()
    .static_method("method1", &Test::method1)
    .extern_static_method("method2", extern_method2)
    .extern_static_method("method3", std::function<void (int)>(extern_method2));
  engine.bind();
  return 0;
}
```
#### <a name="overloading"></a> overloading
To support overloading, it is needed to split call :
```c++
#include <luacxx/binding/class.hpp>

class Test
{
  public:
    Test() {}

    void method()
    {
    }

    void method(const std::string&)
    {
    }
};

int main()
{
  luacxx::engine engine;
  luacxx::make_class<Test>(engine, "Test")
    .ctor<>()
    .method<void>("method1", &Test::method)
    .method<void, const std::string&>("method2", &Test::method);
  engine.bind();
  return 0;
}
```
#### example
my_script.cc
```c++
#include <iostream>
#include <luacxx/engine.hpp>
#include <luacxx/binding/class.hpp>

class Test
{
  public:
    Test() 
    {
      std::cout << "ctor()" << std::endl;
    }

    void method1()
    {
      std::cout << "method1()" << std::endl;
    }

    void method2(const std::string& arg) const
    {
      std::cout << "method2(" << arg << ")" << std::endl;
    }

    static void method3(int arg)
    {
      std::cout << "method3(" << arg << ")" << std::endl;
    }
};

int main()
{
  luacxx::engine engine;
  luacxx::make_class<Test>(engine, "Test")
    .ctor<>()
    .method<void>("method1", &Test::method)
    .const_method("method2", &Test::method2)
    .static_method("method3", &Test::method3);
  engine.bind();
  engine.do_file("my_script.lua");
  return 0;
}
```

my_script.lua
```lua
local l = Test();
l:method1();
l:method2("blabla");
Test.method3(5);
```

output
```shell
ctor()
method1()
method2(blabla)
method3(5)
```
### <a name="property"></a> Property
### <a name="inheritance"></a> Inheritance
