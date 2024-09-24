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

# add the library that runs
add_library(oliver_lang STATIC oliver_lang.cpp )

# Configure Boost
#set(BOOST_INCLUDEDIR    "<path to>") # Manualy configure the path.
#set(BOOST_LIBRARYDIR    "<path to>") # Manualy configure the path.
set(Boost_USE_STATIC_LIBS        ON)  # only find static libs
set(Boost_USE_DEBUG_LIBS         ON)  # ignore debug libs and
set(Boost_USE_RELEASE_LIBS       ON)  # only find release libs
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     ON)
#set(Boost_NO_BOOST_CMAKE       TRUE)
#set(Boost_NO_SYSTEM_PATHS      TRUE)
option(Boost_DEBUG "Set to ON to enable debug output from FindBoost." OFF)
option(Boost_DETAILED_FAILURE_MSG "Set to ON to get detailed information" OFF)
find_package(Boost 1.85 REQUIRED nowide)
if (Boost_FOUND)
    include_directories(${Boost_INCLUDE_DIR})
    message(STATUS "Boost version: ${Boost_VERSION}")
endif()

# link TextSupport to oliver_compiler_flags
#find_package(fmt::fmt CONFIG REQUIRED)
# include(FetchContent)

# FetchContent_Declare(
#   fmt
#   GIT_REPOSITORY https://github.com/fmtlib/fmt
#   GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281  # 10.2.1
# )
# FetchContent_MakeAvailable(fmt)

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(gmp REQUIRED IMPORTED_TARGET gmp)

# find_package(PkgConfig REQUIRED)
# pkg_check_modules(gmpxx REQUIRED IMPORTED_TARGET gmpxx)

find_package(fmt CONFIG REQUIRED)

# state that anybody linking to us needs to include the current source dir
# to find oliver_lang.h, while we don't.
target_include_directories(oliver_lang
                           INTERFACE
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                             $<INSTALL_INTERFACE:include>
                           )

# link oliver_lang to oliver_compiler_flags
target_link_libraries(oliver_lang
                     PUBLIC 
                         fmt::fmt-header-only     # Add '-header-only' for header only support.
                         Boost::boost
                         oliver_compiler_flags
                         ${Boost_LIBRARIES}
                     )

# install libs
set(installable_libs oliver_lang oliver_compiler_flags)
if(TARGET oliver_lang)
  list(APPEND installable_libs oliver_lang)
endif()
install(TARGETS ${installable_libs}
        EXPORT oliver_lang_Targets
        DESTINATION lib)

# install include headers
install(FILES oliver_lang.h DESTINATION include)
