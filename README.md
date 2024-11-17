# Function caller (funcall)

Funcaller is a lightweight C++ library designed to simplify thread context switching through your own interfaces. It enables safe and flexible function invocation while automatically managing thread context transitions, making it ideal for multithreaded applications, plugin architectures, and event-driven systems. With Funcaller, you can easily dispatch functions, methods, and functors across different threads with type safety and minimal overhead.

## Features
- **Type-Safe Invocation**: Ensures correct argument types at compile-time.
- **Automatic Thread Context Switching**: Enables seamless thread transitions when calling functions through interfaces.
- **Supports Various Callables**: Each context switch can have its own thread context or use a global thread context.
- **Flexible API**: Easily integrates into existing projects with minimal changes since it uses interfaces.
- **Cross-Platform**: Compatible with major C++ compilers on Windows, macOS, and Linux.
- **No External Dependencies**: Pure C++ solution with no third-party dependencies.

## Example

### An interface
```cpp
class IFoo
{
public:
    virtual ~IFoo() = default;

    virtual void foo1() = 0;
    virtual void foo2(int a) = 0;
    virtual void foo3(std::shared_ptr<int> ptr) = 0;
};
```

### Interface implementation
```cpp
class Foo : public IFoo
{
public:
    void foo1() override
    {
        spdlog::info("Function foo1 called");
    }

    void foo2(int a) override
    {
        spdlog::info("Function foo2 called with argument: {}", a);
    }

    void foo3(std::shared_ptr<int> ptr) override
    {
        spdlog::info("Function foo3 called with argument: {}", *ptr);
    }
};
```

### A class that uses the interface
```cpp
class Bar {
public:
    explicit Bar(std::shared_ptr<IFoo> foo)
        : foo(std::move(foo))
    {
    }

    void bar1() const {
        foo->foo1();
    }

    void bar2(int a) const {
        foo->foo2(a);
    }

    void bar3(std::shared_ptr<int> ptr) const {
        foo->foo3(std::move(ptr));
    }

private:
    std::shared_ptr<IFoo> foo;
};
```

### Declare a context switch class
```cpp
class FooContextSwitch : public util::ContextSwitch<IFoo>
{
public:
    void foo1() override QUEUE(foo1)
    void foo2(int a) override QUEUE(foo2, a)
    void foo3(std::shared_ptr<int> ptr) override QUEUE(foo3, ptr)
};
```

### Usage
```cpp
    // create concrete Foo object
    auto foo = std::make_shared<Foo>();

    // create a Foo context switch object and configure it
    auto fooContextSwitch = std::make_shared<FooContextSwitch>();
    fooContextSwitch->configure(MainQueue, foo);

    // inject the context switch instead of the actual object
    auto bar = std::make_shared<Bar>(fooContextSwitch);
    
    // Trigger the calls to foo inside bar
    bar->bar1();
    bar->bar2(42);
    bar->bar3(std::make_shared<int>(55));
```

# Run the example

## Using dev container
```
Should work out of the box
```

## Using conan
[Install conan version 2.0.0 or higher](https://conan.io/downloads)
```
conan profile new default --detect
```

### MSVC

````
conan install . --build missing --settings build_type=Debug
cmake --preset conan-default
cmake --build --preset conan-debug
.\build\Debug\funcall.exe
````

### gcc

```
conan install . --build missing --settings "&:build_type=Debug" --settings build_type=Release
cmake --preset conan-debug
cmake --build --preset conan-debug
.\build\Debug\funcall
```

