# ZinoEngine

A custom C++ 3d vulkan only (for now) game engine that I make for fun.

## How to build

For now only VS2019 is supported, but if you want you can try to build it for other compilers.

- Run BuildLibs.bat to compile 3rd party libs
- Run GenerateVS2019.bat to generate the solution and project files
- Open the solution in Build/ProjectFiles and build it

Only Debug is implemented for now.

## My coding standard

I personally love explicit code. So don't except too much `auto` for example. But I hate very verbose code.

### T*

**All** raw pointers are implicitly non owning. 

Use `TOwnerPtr` for raw pointers actually owning the resource.
(I also added a `TNonOwningPtr` type but I think I will abandon it.)

### Naming

Use pascal case everywhere.

- `void MyFunction()`
- `int MyInt`

Always prefix function parameters with `In`: `void MyFunction(int InMyVal, bool bInMyBool)`

Don't prefix struct/class member variables with `m_`,`m_p`, etc


Type names are prefixed with a letter:

- `E` for enums: `EMyEnum`
- `S`for structs: `SMyStruct`
- `U` for unions : `UMyUnion`
- `C` for classes : `CMyClass`
- `I` for interfaces : `IMyInterface`
- `T` for templated structs/classes : `template<typename T> class TMyTemplatedClass`


Prefix all booleans with `b`: `bool bMyBool`.

For macros, write them `LIKE_THAT`

### Namespaces
Namespaces should be used whenever possible, as they increase code readability. Try to have no more than 3 depths of namespace.


### Smart pointers

Always use smart pointers whenever possible instead of raw pointers.

### auto
Only use `auto` for replacing long types or in loops.

    for(const auto& Elem : Vector)
    {
        ...
    }

    auto It = MyMap.find(MyElem);


