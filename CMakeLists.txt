cmake_minimum_required(VERSION 4.0)
project(learning LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_COMPILE_WARNING_AS_ERROR ON)


# ---------------------------------------------------------------------------
# Compiler Warnings Configuration
# ---------------------------------------------------------------------------
add_library(project_warnings INTERFACE)

if(MSVC)
    target_compile_options(project_warnings INTERFACE
        /W4          # Baseline reasonable warnings
        /w14242      # 'identifier': conversion from 'type1' to 'type2', possible loss of data
        /w14254      # 'operator': conversion from 'type1:bitfield' to 'type2:bitfield'
        /w14263      # 'function': member function does not override any base class virtual member function
        /w14265      # 'class': class has virtual functions, but destructor is not virtual
        /w14287      # 'operator': unsigned/negative constant mismatch
        /w14296      # 'operator': expression is always false
        /w14311      # 'variable': pointer truncation from 'type1' to 'type2'
        /w14545      # expression before comma evaluates to a function which is missing an argument list
        /w14546      # function call before comma missing argument list
        /w14547      # 'operator': avant-propos evaluation before comma has no effect
        /w14549      # 'operator': avant-propos evaluation before comma has no effect
        /w14555      # expression has no effect; expected expression with side-effect
        /w14619      # pragma warning: there is no warning number 'number'
        /w14640      # Enable warning on thread-unsafe local static initialization
        /w14996      # Severe security/deprecation warnings
        /permissive- # Enforce standards conformance
    )
elseif(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(project_warnings INTERFACE
        -Wall
        -Wextra
        -Wpedantic          # Ensure strict ISO C++ compliance
        -Wshadow            # Warn when a variable shadows another one
        -Wnon-virtual-dtor  # Warn the user if a class with virtual functions has a non-virtual destructor
        -Wold-style-cast    # Warn for C-style casts in C++ code
        -Wcast-align        # Warn for potential performance bottlenecks in type casting
        -Wunused            # Warn on anything unused
        -Woverloaded-virtual # Warn if a derived class function hides a base class virtual function
        -Wconversion        # Warn for implicit type conversions that may lose data
        -Wsign-conversion   # Warn for implicit sign conversions
        -Wnull-dereference  # Warn if a null dereference is detected
        -Wdouble-promotion  # Warn if implicit conversion from float to double occurs
        -Wformat=2          # Security check on printf-like format strings
    )
    
    # GCC specific flags that Clang doesn't always support
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
        target_compile_options(project_warnings INTERFACE
            -Wmisleading-indentation # Warn if indentation implies blocks that don't exist
            -Wduplicated-cond        # Warn if if-else chain has duplicated conditions
            -Wduplicated-branches    # Warn if if-else branches have duplicated code
            -Wlogical-op             # Warn for suspicious logical operators
        )
    endif()
endif()

add_executable(ray-tracer main.cpp)

target_link_libraries(ray-tracer PRIVATE
    project_warnings
)