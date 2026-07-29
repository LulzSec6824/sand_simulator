cmake_minimum_required (VERSION 3.11)
project ("particlesimulator")

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141 AND MSVC)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Enable LTO/IPO if supported by the compiler.
include(CheckIPOSupported)
check_ipo_supported(RESULT ipo_supported OUTPUT ipo_error)
if(ipo_supported)
    # Automatically triggers LTO only during Release and MinSizeRel builds
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_RELEASE ON)
    set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_MINSIZEREL ON)
else()
    message(STATUS "LTO/IPO is not supported by your compiler: ${ipo_error}")
endif()

set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
add_subdirectory(raylib)

# Add source to this project's executable.
add_executable (particlesimulator "src/main.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE raylib)