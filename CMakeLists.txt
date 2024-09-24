##############################################################################################
# 
#                            Copyright(C) 2023 Max J Martin
# 
#                             This file is part of Oliver.
#                       Oliver is program language interpreter. 
#     
#           This program is free software : you can redistribute it and /or modify
#           it under the terms of the GNU Affero General Public License as published by
#           the Free Software Foundation, either version 3 of the License, or
#           (at your option) any later version.
#     
#           This program is distributed in the hope that it will be useful,
#           but WITHOUT ANY WARRANTY; without even the implied warranty of
#           MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
#           GNU Affero General Public License for more details.
#     
#           You should have received a copy of the GNU Affero General Public License
#           along with this program.If not, see <https:# www.gnu.org/licenses/>.
#     
#           The author can be reached at: maxjmartin@gmail.com
# 
##############################################################################################

cmake_minimum_required(VERSION 3.28)

# set the project name and version
project(Oliver VERSION 0.1.0)

set(CMAKE_DEBUG_POSTFIX d)

add_library(oliver_compiler_flags INTERFACE)
target_compile_features(oliver_compiler_flags INTERFACE cxx_std_23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)  # Make sure the stl is available.
set(CMAKE_CXX_EXTENSIONS ON)         # Set this to off to make code as portable as possible.  
set(WARNINGS_AS_ERRORS ON)           # Treat warnings as errors.  

# Add compiler warning flags just when building this project via
# the BUILD_INTERFACE genex.  
set(gcc_like_cxx "$<COMPILE_LANG_AND_ID:CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
set(msvc_cxx "$<COMPILE_LANG_AND_ID:CXX,MSVC>")
target_compile_options(oliver_compiler_flags INTERFACE
  "$<${gcc_like_cxx}:$<BUILD_INTERFACE:-Wall;-Wextra;-Wshadow;-Wformat=2;-Wunused>>"
  "$<${msvc_cxx}:$<BUILD_INTERFACE:-W3>>"
)

# Control where the static and shared libraries are built so that on windows
# we don't need to tinker with the path to run the executable.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")

option(BUILD_SHARED_LIBS "Build using shared libraries" OFF)

if(APPLE)
  set(CMAKE_INSTALL_RPATH "@executable_path/../lib")
elseif(UNIX)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
endif()

# Configure a header file to pass the version number only.
configure_file(OliverConfig.h.in OliverConfig.h)

# Add the project app directory.
add_subdirectory(lib_oliver_lang)

# Add the executable.
add_executable(Oliver main.cpp)
set_target_properties(Oliver PROPERTIES DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX})

target_link_libraries(Oliver PRIVATE
                      oliver_lang
                      oliver_compiler_flags
                     )

# Add the binary tree to the search path for include files
# so that we will find OliverConfig.h.
target_include_directories(Oliver PUBLIC
                           "${PROJECT_BINARY_DIR}"
                           )

# Add the install targets.
install(TARGETS Oliver DESTINATION bin)
install(FILES "${PROJECT_BINARY_DIR}/OliverConfig.h"
  DESTINATION include
  )

# Setup installer.
include(InstallRequiredSystemLibraries)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${Oliver_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Oliver_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${Oliver_VERSION_PATCH}")
set(CPACK_SOURCE_GENERATOR "TGZ")
include(CPack)