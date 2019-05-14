# Macros

There are many different macros that can determine how MACE works.

## Macros that affect how MACE works

| *Macro* | *Effect* |
|--------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| MACE_DEBUG | Does more error checking and more verbose error messages. Automatically defined in Debug configurations. If defined to be 0 before including any MACE headers, will never be defined. |
| MACE_DEBUG_OPENGL | Does extensive and detailed error checking of OpenGL calls. This greatly decreases performance of the renderer, but allows for debugging of graphics routines. Automatically defined when MACE_DEBUG is 1. |
| MACE_DEBUG_CHECK_ARGS | Do additional argument checking in functions where users may misuse an API. Automatically defined when MACE_DEBUG is 1. |
| MACE_DEBUG_CHECK_NULLPTR | Do additional nullptr checks internally. In a production scenario, this should never raise an error, but this macro can be useful for the initial debug phase of an application. Automatically defined when MACE_DEBUG is 1. |
| MACE_DEBUG_INTERNAL_ERRORS | Check for any internal state mismatches or errors. Automatically defined when MACE_DEBUG is 1. |
| MACE_EXPOSE_WINAPI | If defined, classes that use Winapi will have functions to allow for direct access to any Winapi variables. Will not work if MACE_WINAPI is not defined. |
| MACE_EXPOSE_POSIX | If defined, classes that use POSIX will have functions to allow for direct access to any POSIX variables. Will not work if MACE_POSIX is not defined |
| MACE_EXPOSE_OPENGL | If defined, the OpenGL abstraction layer and renderer will be exposed by including MACE.h |
| MACE_EXPOSE_GLFW | If defined, `os::WindowModule` will expose the underlying GLFWwindow |
| MACE_EXPOSE_OPENAL | If defined, `AudioModule` will expose functions to return the OpenAL context |
| MACE_EXPOSE_ALL | If defined, defined all macros starting with MACE_EXPOSE_* |


### Libraries

These macros are automatically defined if the specified library is found. Otherwise, you can manually define them.

* MACE_WINAPI
* MACE_POSIX

#### OpenCV

MACE has optional interoptibility with OpenCV. To link OpenCV with CMake, set the OpenCV_* cache variables when running CMake.

MACE will attempt to automatically detect OpenCV and defined MACE_OPENCV if found.

To manually link OpenCV yourself, define MACE_OPENCV to be 1 before including any MACE header file:

```c++
//specify that OpenCV is linked
#define MACE_OPENCV 1
#include <MACE/MACE.h.
```

MACE will include `<opencv2/opencv.hpp>` if MACE_OPENCV is defined.

Additionally, if CV_VERSION is defined, MACE_OPENCV will also be defined automatically.

To request to never use OpenCV, even if it is included, define MACE_OPENCV to be 0 or false.

```c++
//both of the following lines tell MACE to never use OpenCV
#define MACE_OPENCV 0
#define MACE_OPENCV false
#include <MACE/MACE.h>
```

### Operating Systems

One of these macros will be defined based on the operating system.

* MACE_OSX
* MACE_WINDOWS
* MACE_UNIX

### Compilers

One of the following macros will be defined to their respective compiler versions:

*  MACE_BORLAND 
*  MACE_MINGW 
*  MACE_CLANG 
*  MACE_GCC
*  MACE_HP 
*  MACE_INTEL 
*  MACE_SOLARIS
*  MACE_WATCOM 
*  MACE_CRAY 
*  MACE_MIPSPRO 
*  MACE_PORTLAND 
*  MACE_PATHSCALE 
*  MACE_IBM 

Additionally, if the compiler supports GNU C, `MACE_GNU` will be defined.

## Macros defined by MACE

### Informational macros

| *Macro* | *Meaning* |
|-----------------------------|----------------------------------------------------------------------------------------------------------------|
| MACE_VERSION | Always defined to be the current version of MACE, in the format major.minor.patch |
| MACE_VERSION_MAJOR | Major version number |
| MACE_VERSION_MINOR | Minor version number |
| MACE_VERSION_PATCH | Minor version number |
| MACE_TESTS | Defined if CMake built the testing suite. |
| MACE_DEMOS | Defined if CMake built the demos |
| MACE_POINTER_SIZE | Size of a void* on this system. |
| MACE_32_BIT | Whether this system is 32 bit. Defined if MACE_POINTER_SIZE == 4. |
| MACE_64_BIT | Whether this system is 64 bit. Defined if MACE_POINTER_SIZE == 8. |
| MACE_DYNAMIC_LIBRARY_PREFIX | The prefix for dynamic libraries (such as lib.) Some systems may not have a prefix. |
| MACE_DYNAMIC_LIBRARY_SUFFIX | The suffix for dynamic libraries (such as .so, .dll, or .a.) |
| MACE_LITTLE_ENDIAN | Whether the system uses little endian (significant bit/byte last) |
| MACE_BIG_ENDIAN | Whether the system uses big endian (significant bit/byte first) |


### Utility macros

| *Macro* | *Meaning* |
|---------------------------------|---------------------------------------------------------------------------------------------------------|
| MACE_MAKE_VERSION_NUMBER(major, minor, patch) | Creates a single version number from 3 version numbers. Encodes all 3 numbers into a single 32 bit integer. |
| MACE_MAKE_VERSION_STRING(major, minor, patch) | Creates a token that combines the 3 version numbers into the format "Major.minor.patch". This is not stringified. |
| MACE_FUNCTION_EXPORT | Modifier that tells the compiler that this function should be exported to the global symbol table. Could be empty |
| MACE_FUNCTION_IMPORT | Modifier that tells the compiler that this function will be imported from an exported symbol. Could be empty. |
| MACE_HAS_ATTRIBUTE(attr) | If not defined previously, returns 0 if specified attribute is supported, 1 otherwise. If attributes are not supported by the compiler, always returns 0  |
| MACE_HAS_INCLUDE(incl) | If not defined previously, returns 1 if the system has the specified include file, 0 otherwise. Not all compilers support this. If unsupported by the compiler, always returns 0. |
| MACE_FALLTHROUGH | If not defined previously, if the compiler supports the fallthrough attribute, MACE_FALLTHROUGH is defined to that. Otherwise, defined to nothing  |
| MACE_NODISCARD | If not defined previously, if the compiler supports the nodiscard attribute, MACE_NODISCARD is defined to that. Otherwise, defined to nothing  |
| MACE_NORETURN | If not defined previously, if the compiler supports the noreturn attribute, MACE_NORETURN is defined to that. Otherwise, defined to nothing  |
| MACE_NOVTABLE | If not defined previously, if the compiler supports the novtable declspec, MACE_NOVTABLE is defined to that. Otherwise, defined to nothing  |
| MACE_UNREACHABLE | If not defined previously, if the compiler supports it, MACE_UNREACHABLE is defined to an attribute which signals to a compiler that a code path is unreachable at runtime. Otherwise, defined to nothing  |
| MACE_UNREACHABLE_RETURN | If not defined previously, if the compiler supports it, MACE_UNREACHABLE_RETURN is defined to an attribute which signals to a compiler that a return statement is unreachable at runtime. Otherwise, defined to nothing  |
| MACE_LIKELY | If not defined previously, if the compiler supports the likely attribute, MACE_LIKELY is defined to that. Otherwise, defined to nothing  |
| MACE_UNLIKELY | If not defined previously, if the compiler supports the unlikely attribute, MACE_UNLIKELY is defined to that. Otherwise, defined to nothing  |
| MACE_EXPECTS_MODIFIER(cond, mod) | If not defined previously, if the compiler supports the expects attribute, MACE_EXPECTS_MODIFIER is defined to that, in the form `expects <mod>: <cond>`. Otherwise, defined to nothing  |
| MACE_EXPECTS(cond) | Overload of MACE_EXPECS_MODIFIER where `<mod>` is `default`  |
| MACE_ENSURES_MODIFIER(ret, cond, mod) | If not defined previously, if the compiler supports the ensures attribute, MACE_ENSURES_MODIFIER is defined to that, in the form `ensures <mod> <ret>: <cond>`. Otherwise, defined to nothing  |
| MACE_ENSURES(ret, cond) | Overload of MACE_ENSURES_MODIFIER where `<mod>` is `default`  |
| MACE_CONSTEXPR | If not defined previously, defined to be `constexpr` on compilers that `constexpr` is supported, otherwise becomes nothing |
| MACE_IF_CONSTEXPR(expr) | If not defined previously, defined to be `if constexpr(<expr>)` on compilers that `if constexpr` is supported, otherwise becomes a normal if statement. |
| MACE_EXPLICIT(cond) | If not defined previously, defined to be `explicit(<cond>)` on compilers that conditional `explicit` is supported, otherwise becomes `explicit` without the conditional |
| MACE_DEPRECATED | If not defined previously, Marks a function that is deprecated. Based on the compiler, this is defined to the respective attribute to generate the proper warnings. If not supported, empty. |
| MACE_STATIC_ASSERT(condition, message) | If not defined, defined to be `static_assert(<condition>, <message>)`. Can be defined before including MACE headers to change the static assert used by MACE |
| MACE_STRINGIFY(name) | Directly stringifies a macro  |
| MACE_STRINGIFY_NAME(name) | Stringifies a macro name. If the expanded macro is empty, this function returns "" |
| MACE_STRINGIFY_DEFINITION(name) | Stringifies the definition of a macro. If the macro doesn't have a definition, this function returns "" |
| MACE_GETTER_SETTER_DEC_BASE(name, inType, outType) | Utility macro to automatically create getter and setter declarations, where the setter is called set`<name>` and takes in a value of `<inType>` while the getter is called get`<name>` and returns a value of `<outType>`. |
| MACE_GETTER_SETTER_DEC(name, type) | Overload of MACE_GETTER_SETTER_DEC_BASE where `<inType>` and `<outType>` are both `<type>` |
| MACE_GETTER_SETTER_DEF_BASE(className, funcName, varName, inType, outType) | Utility macro to automatically create getter and setter definitions, where the setter is called `<className>`::set`<funcName>` and sets `<varName` to a value of `inType>` while the getter is called `<className>`::get`<name>` and returns a value of `<outType>`. |
| MACE_GETTER_SETTER_DEF(className, funcName, varName, type) | Overload of MACE_GETTER_SETTER_DEC_BASE where `<inType>` and `<outType>` are both `<type>` |