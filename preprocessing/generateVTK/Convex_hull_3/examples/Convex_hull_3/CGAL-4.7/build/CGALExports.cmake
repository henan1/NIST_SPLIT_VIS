# Generated by CMake 2.8.12.2

if("${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION}" LESS 2.5)
   message(FATAL_ERROR "CMake >= 2.6.0 required")
endif()
cmake_policy(PUSH)
cmake_policy(VERSION 2.6)
#----------------------------------------------------------------
# Generated CMake target import file.
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Protect against multiple inclusion, which would fail when already imported targets are added once more.
set(_targetsDefined)
set(_targetsNotDefined)
set(_expectedTargets)
foreach(_expectedTarget CGAL::CGAL CGAL::CGAL_Core CGAL::CGAL_ImageIO)
  list(APPEND _expectedTargets ${_expectedTarget})
  if(NOT TARGET ${_expectedTarget})
    list(APPEND _targetsNotDefined ${_expectedTarget})
  endif()
  if(TARGET ${_expectedTarget})
    list(APPEND _targetsDefined ${_expectedTarget})
  endif()
endforeach()
if("${_targetsDefined}" STREQUAL "${_expectedTargets}")
  set(CMAKE_IMPORT_FILE_VERSION)
  cmake_policy(POP)
  return()
endif()
if(NOT "${_targetsDefined}" STREQUAL "")
  message(FATAL_ERROR "Some (but not all) targets in this export set were already defined.\nTargets Defined: ${_targetsDefined}\nTargets not yet defined: ${_targetsNotDefined}\n")
endif()
unset(_targetsDefined)
unset(_targetsNotDefined)
unset(_expectedTargets)


# Create imported target CGAL::CGAL
add_library(CGAL::CGAL SHARED IMPORTED)

# Create imported target CGAL::CGAL_Core
add_library(CGAL::CGAL_Core SHARED IMPORTED)

# Create imported target CGAL::CGAL_ImageIO
add_library(CGAL::CGAL_ImageIO SHARED IMPORTED)

# Import target "CGAL::CGAL" for configuration "Release"
set_property(TARGET CGAL::CGAL APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(CGAL::CGAL PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "/usr/lib64/libmpfr.so;/usr/lib64/libgmp.so;/usr/lib64/libboost_thread-mt.so;/usr/lib64/libboost_system-mt.so"
  IMPORTED_LOCATION_RELEASE "/home/davinci/Documents/henan/NIST/NIST_SPLIT/SPLIT_VIS2/preprocessing/generateVTK/Convex_hull_3/examples/Convex_hull_3/CGAL-4.7/build/lib/libCGAL.so.11.0.1"
  IMPORTED_SONAME_RELEASE "libCGAL.so.11"
  )

# Import target "CGAL::CGAL_Core" for configuration "Release"
set_property(TARGET CGAL::CGAL_Core APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(CGAL::CGAL_Core PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "/usr/lib64/libmpfr.so;/usr/lib64/libgmp.so;CGAL::CGAL;/usr/lib64/libboost_thread-mt.so;/usr/lib64/libboost_system-mt.so"
  IMPORTED_LOCATION_RELEASE "/home/davinci/Documents/henan/NIST/NIST_SPLIT/SPLIT_VIS2/preprocessing/generateVTK/Convex_hull_3/examples/Convex_hull_3/CGAL-4.7/build/lib/libCGAL_Core.so.11.0.1"
  IMPORTED_SONAME_RELEASE "libCGAL_Core.so.11"
  )

# Import target "CGAL::CGAL_ImageIO" for configuration "Release"
set_property(TARGET CGAL::CGAL_ImageIO APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(CGAL::CGAL_ImageIO PROPERTIES
  IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE "/usr/lib64/libmpfr.so;/usr/lib64/libgmp.so;CGAL::CGAL;/usr/lib64/libboost_thread-mt.so;/usr/lib64/libboost_system-mt.so;/usr/lib64/libGLU.so;/usr/lib64/libGL.so;/usr/lib64/libSM.so;/usr/lib64/libICE.so;/usr/lib64/libX11.so;/usr/lib64/libXext.so;/usr/lib64/libz.so"
  IMPORTED_LOCATION_RELEASE "/home/davinci/Documents/henan/NIST/NIST_SPLIT/SPLIT_VIS2/preprocessing/generateVTK/Convex_hull_3/examples/Convex_hull_3/CGAL-4.7/build/lib/libCGAL_ImageIO.so.11.0.1"
  IMPORTED_SONAME_RELEASE "libCGAL_ImageIO.so.11"
  )

# This file does not depend on other imported targets which have
# been exported from the same project but in a separate export set.

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
cmake_policy(POP)