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
#           along with this program.If not, see < https:# www.gnu.org/licenses/>.
#     
#           The author can be reached at: maxjmartin@gmail.com
# 
##############################################################################################

# add the library that runs
add_library(TextSupport STATIC text_support.cpp)

# Configure Boost
set(BOOST_INCLUDEDIR    "C:/local/boost_1_83_0/")
set(BOOST_LIBRARYDIR    "C:/local/boost_1_83_0/lib64-msvc-14.3")
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         ON)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME    OFF)
#set(Boost_NO_BOOST_CMAKE       TRUE)
#set(Boost_NO_SYSTEM_PATHS      TRUE)
option(Boost_DEBUG "Set to ON to enable debug output from FindBoost." OFF)
option(Boost_DETAILED_FAILURE_MSG "Set to ON to get detailed information" OFF)
find_package(Boost 1.82 REQUIRED nowide)
message(STATUS "Boost version: ${Boost_VERSION}")

# state that anybody linking to us needs to include the current source dir
# to find TextSupport.h, while we don't.
target_include_directories(TextSupport
                           INTERFACE
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                            $<INSTALL_INTERFACE:include>
                           )

# link TextSupport to oliver_compiler_flags
find_package(fmt CONFIG REQUIRED)
target_link_libraries(TextSupport PUBLIC 
                        fmt::fmt-header-only
                        Boost::boost
                        oliver_compiler_flags
                        ${Boost_LIBRARIES}
                     )

# install libs
set(installable_libs TextSupport oliver_compiler_flags)
if(TARGET TextSupport)
  list(APPEND installable_libs TextSupport)
endif()
install(TARGETS ${installable_libs}
        EXPORT TextSupportTargets
        DESTINATION lib)

# install include headers
install(FILES text_support.h DESTINATION include)
