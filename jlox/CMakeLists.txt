cmake_minimum_required(VERSION 3.28)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(jlox VERSION 1.0)

set(CLOX_SOURCES
    src/main.cpp;
    src/token.cpp;
    src/scanner.cpp;
	src/object_utils.cpp;
    src/lox.cpp;
    src/expr.cpp;
    src/ast_printer.cpp;
    src/parser.cpp;
    src/interpreter.cpp;
    src/runtime_error.cpp;
    src/stmt.cpp;
    src/environment.cpp;
    src/lox_function.cpp;
	src/return.cpp;
	src/resolver.cpp;
	src/lox_class.cpp;
	src/lox_instance.cpp;
)

option(VERBOSE_DEBUG_OPTION "Enable verbose mode" OFF)
option(TOKEN_DEBUG_OPTION "Enable token debugging destructor" OFF)
option(ENVIRONMENT_DEBUG_OPTION "Enable environment debugging functions" OFF)

if (${TOKEN_DEBUG_OPTION})
    set(TOKEN_DEBUG 1)
else()
    set(TOKEN_DEBUG 0)
endif()

if (${ENVIRONMENT_DEBUG_OPTION})
	set(ENVIRONMENT_DEBUG 1)
else()
    set(ENVIRONMENT_DEBUG 0)
endif()

if (${VERBOSE_DEBUG_OPTION})
	set(VERBOSE_DEBUG 1)
else()
	set(VERBOSE_DEBUG 0)
endif()


add_executable(${PROJECT_NAME} ${CLOX_SOURCES})
target_compile_definitions(${PROJECT_NAME} PRIVATE TOKEN_DEBUG=${TOKEN_DEBUG} ENVIRONMENT_DEBUG=${ENVIRONMENT_DEBUG} VERBOSE_DEBUG=${VERBOSE_DEBUG})

target_include_directories(${PROJECT_NAME} PRIVATE include)
