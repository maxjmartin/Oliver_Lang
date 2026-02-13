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
add_library(App STATIC app.h app.cpp )

# Find packages via vcpkg
find_package(Boost REQUIRED COMPONENTS locale)
find_package(fmt CONFIG REQUIRED)

target_include_directories(App
                           INTERFACE
                             $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                             $<INSTALL_INTERFACE:include>
                           )

# link App to oliver_compiler_flags
target_link_libraries(App
                     PUBLIC 
                       Boost::boost
                       Boost::locale
                       fmt::fmt
                       oliver_compiler_flags
                     )

# install libs
set(installable_libs App oliver_compiler_flags)
if(TARGET App)
  list(APPEND installable_libs App)
endif()
install(TARGETS ${installable_libs}
        EXPORT App_Targets
        DESTINATION lib)

# install include headers
install(FILES app.h DESTINATION include)
