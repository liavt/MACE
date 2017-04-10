#Macros

There are many different macros that can determine how MACE works.

##Macros that affect how MACE works

| *Macro* | *Effect* |
|--------------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| MACE_DEBUG | Does more error checking and more verbose error messages. Automatically defined in Debug configurations |
| MACE_EXPOSE_WINAPI | If defined, classes that use Winapi will have functions to allow for direct access to any Winapi variables. Will not work if MACE_WINAPI is not defined. |
| MACE_EXPOSE_POSIX | If defined, classes that use POSIX will have functions to allow for direct access to any POSIX variables. Will not work if MACE_POSIX is not defined |

###Libraries

These macros are automatically defined if the specified library is found. Otherwise, you can manually define them.

* MACE_OPENCV
* MACE_X11
* MACE_WINAPI
* MACE_POSIX

###Operating Systems

One of these macros will be defined based on the operating system.

* MACE_OSX
* MACE_WINDOWS
* MACE_UNIX

###Compilers

The macro MACE_COMPILER has the compiler ID. The MACE_COMPILER_VERSION macro has the compiler version.

If Microsoft Visual Studio is used, MACE_MSVC is defined to be the Visual Studio version number. Additionally, if the IDE is being used, MACE_MSVC_IDE is defined to 1.
If MSCV CL is being used, MACE_MSVC_CL is defined to 1. If MSVC 2005 compiler is used, MACE_MSVC_2005 is defined to 1.

Otherwise, one of the following macros will be defined to the compiler version:

*  MACE_BORLAND 
*  MACE_MINGW 
*  MACE_CYGWIN 
*  MACE_CLANG 
*  MACE_GNU 
*  MACE_HP 
*  MACE_INTEL 
*  MACE_ORACLE 
*  MACE_TEXAS_INSTRUMENTS 
*  MACE_WATCOM 
*  MACE_ADSP 
*  MACE_CRAY 
*  MACE_MIPSPRO 
*  MACE_PORTLAND 
*  MACE_PATHSCALE 
*  MACE_IBM 

##Macros defined by MACE

###Informational macros

| *Macro* | *Meaning* |
|-----------------------------|----------------------------------------------------------------------------------------------------------------|
| MACE_INCLUDED | Always defined to be 1 when MACE is included in a project |
| MACE_TESTS | Defined if CMake built the testing suite. |
| MACE_DEMOS | Defined if CMake built the demos |
| MACE_INCLUDES | Defined to be the directory in which the include files are located. This macro is not stringified. |
| MACE_DIRECTORY | Defined to be the directory where the MACE source came from. This macro is not stringified. |
| MACE_SYSTEM_NAME | Defined to be the name of the operating system in which MACE was configured on. This macro is not stringified. |
| MACE_SYSTEM_VERSION | The version of the operating system in which MACE was configured on. This macro is not stringified. |
| MACE_PROCESSOR_NAME | The name of the processor on which MACE was configured on. This macro is not stringified. |
| MACE_POINTER_SIZE | Size of a void* on this system. |
| MACE_32_BIT | Whether this system is 32 bit. Defined if MACE_POINTER_SIZE == 4. |
| MACE_64_BIT | Whether this system is 64 bit. Defined if MACE_POINTER_SIZE == 8. |
| MACE_DYNAMIC_LIBRARY_PREFIX | The prefix for dynamic libraries (such as lib.) Some systems may not have a prefix. |
| MACE_DYNAMIC_LIBRARY_SUFFIX | The suffix for dynamic libraries (such as .so, .dll, or .a.) |

###Utility macros

| *Macro* | *Meaning* |
|---------------------------------|---------------------------------------------------------------------------------------------------------|
| MACE_FUNCTION_EXPORT | Modifier that tells the compiler that this function should be exported to the global symbol table. |
| MACE_FUNCTION_IMPORT | Modifier that tells the compiler that this function will be imported from an exported symbol |
| MACE_STRINGIFY(name) | Directly stringifies a macro  |
| MACE_STRINGIFY_NAME(name) | Stringifies a macro name. If the expanded macro is empty, this function returns "" |
| MACE_STRINGIFY_DEFINITION(name) | Stringifies the definition of a macro. If the macro doesn't have a definition, this function returns "" |
| MACE_DECLARE_ERROR(name) | This macro should only be used internally. Creates a new Error class with the specified name. |

