#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.30)
project(EASTL CXX)

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/scripts/CMake")
include(CommonCppFlags)

#-------------------------------------------------------------------------------------------
# Options
#-------------------------------------------------------------------------------------------
option(EASTL_BUILD_STATIC "Build EASTL as static library" ON)
option(EASTL_BUILD_SHARED "Build EASTL as shared library" ON)
option(EASTL_ENABLE_MIMALLOC "Enable external mimalloc allocator" ON)

#-------------------------------------------------------------------------------------------
# Library definition
#-------------------------------------------------------------------------------------------
file(GLOB EASTL_SOURCES "source/*.cpp")

# Interface library for common settings
add_library(EASTL-interface INTERFACE)
target_compile_definitions(EASTL-interface INTERFACE
        EA_PRAGMA_ONCE_SUPPORTED=1
        EA_HAVE_CPP11_CONTAINERS=1
        EA_HAVE_CPP11_ATOMIC=1
        EA_HAVE_CPP11_CONDITION_VARIABLE=1
        EA_HAVE_CPP11_MUTEX=1
        EA_HAVE_CPP11_THREAD=1
        EA_HAVE_CPP11_FUTURE=1
        EA_HAVE_CPP11_TYPE_TRAITS=1
        EA_HAVE_CPP11_TUPLES=1
        EA_HAVE_CPP11_REGEX=1
        EA_HAVE_CPP11_RANDOM=1
        EA_HAVE_CPP11_CHRONO=1
        EA_HAVE_CPP11_SCOPED_ALLOCATOR=1
        EA_HAVE_CPP11_INITIALIZER_LIST=1
        EA_HAVE_CPP11_SYSTEM_ERROR=1
        EA_HAVE_CPP11_TYPEINDEX=1
        EASTL_USER_LITERALS_ENABLED=0
        EASTL_STD_ITERATOR_CATEGORY_ENABLED=1
        EASTL_STD_TYPE_TRAITS_AVAILABLE=1
        EASTL_MOVE_SEMANTICS_ENABLED=1
        EASTL_VARIADIC_TEMPLATES_ENABLED=1
        EASTL_VARIABLE_TEMPLATES_ENABLED=1
        EASTL_INLINE_VARIABLE_ENABLED=1
        EASTL_HAVE_CPP11_TYPE_TRAITS=1
        EASTL_INLINE_NAMESPACES_ENABLED=1
        EASTL_ALLOCATOR_EXPLICIT_ENABLED=1
        EASTL_USER_DEFINED_ALLOCATOR=1
        _CHAR16T
        _CRT_SECURE_NO_WARNINGS
        _SCL_SECURE_NO_WARNINGS
        EASTL_OPENSOURCE=1)

target_include_directories(EASTL-interface INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" AND
        CMAKE_CXX_COMPILER_VERSION VERSION_GREATER_EQUAL 15.0)
    target_compile_options(EASTL-interface INTERFACE -Wno-deprecated-builtins)
endif ()

#-------------------------------------------------------------------------------------------
# Dependencies
#-------------------------------------------------------------------------------------------
add_subdirectory(packages/EABase)
target_link_libraries(EASTL-interface INTERFACE EABase)

# External mimalloc support (provided by parent project)
if (EASTL_ENABLE_MIMALLOC)
    if (TARGET mimalloc-static)
        set(EASTL_MIMALLOC_TARGET mimalloc-static)
    elseif(TARGET mimalloc)
        set(EASTL_MIMALLOC_TARGET mimalloc)
    else()
        message(FATAL_ERROR "EASTL_ENABLE_MIMALLOC is ON but no mimalloc target was found. Please include mimalloc before EASTL.")
    endif()
    target_compile_definitions(EASTL-interface INTERFACE EASTL_MIMALLOC_ENABLED=1)
endif()

#-------------------------------------------------------------------------------------------
# Static Library
#-------------------------------------------------------------------------------------------
if (EASTL_BUILD_STATIC)
    add_library(EASTL-static STATIC ${EASTL_SOURCES})
    set_target_properties(EASTL-static PROPERTIES 
        OUTPUT_NAME "EASTL"
        POSITION_INDEPENDENT_CODE ON
        UNITY_BUILD ON)
    
    target_link_libraries(EASTL-static PUBLIC EASTL-interface)
    target_compile_definitions(EASTL-static PRIVATE EASTL_PROJECT=1)

    if (MSVC)
        target_link_options(EASTL-static PRIVATE "/DISALLOWLIB:libcmt.lib" "/DISALLOWLIB:libcmtd.lib")
    endif()
    
    if (EASTL_ENABLE_MIMALLOC)
        target_link_libraries(EASTL-static PUBLIC ${EASTL_MIMALLOC_TARGET})
    endif()
    
    if (WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/doc/EASTL.natvis")
        target_link_options(EASTL-static PRIVATE "LINKER:/NATVIS:${CMAKE_CURRENT_SOURCE_DIR}/doc/EASTL.natvis")
    endif()
    
    add_library(EASTL::EASTL-static ALIAS EASTL-static)
endif()

#-------------------------------------------------------------------------------------------
# Shared Library
#-------------------------------------------------------------------------------------------
if (EASTL_BUILD_SHARED)
    add_library(EASTL-shared SHARED ${EASTL_SOURCES})
    set_target_properties(EASTL-shared PROPERTIES 
        OUTPUT_NAME "EASTL"
        POSITION_INDEPENDENT_CODE ON
        UNITY_BUILD ON
        WINDOWS_EXPORT_ALL_SYMBOLS ON)
    
    target_link_libraries(EASTL-shared PUBLIC EASTL-interface)
    target_compile_definitions(EASTL-shared PRIVATE EASTL_PROJECT=1)

    if (MSVC)
        target_link_options(EASTL-shared PRIVATE "/DISALLOWLIB:libcmt.lib" "/DISALLOWLIB:libcmtd.lib")
    endif()
    
    if (EASTL_ENABLE_MIMALLOC)
        target_link_libraries(EASTL-shared PUBLIC ${EASTL_MIMALLOC_TARGET})
    endif()
    
    if (WIN32 AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/doc/EASTL.natvis")
        target_link_options(EASTL-shared PRIVATE "LINKER:/NATVIS:${CMAKE_CURRENT_SOURCE_DIR}/doc/EASTL.natvis")
    endif()
    
    add_library(EASTL::EASTL-shared ALIAS EASTL-shared)
endif()

#-------------------------------------------------------------------------------------------
# Default target alias
#-------------------------------------------------------------------------------------------
if (EASTL_BUILD_STATIC)
    add_library(EASTL::EASTL ALIAS EASTL-static)
elseif(EASTL_BUILD_SHARED)
    add_library(EASTL::EASTL ALIAS EASTL-shared)
endif()
