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
add_library(TextReader STATIC text_reader.cpp)

# state that anybody linking to us needs to include the current source dir
# to find TextReader.h, while we don't.
target_include_directories(TextReader INTERFACE
                            #"../lib_text_support/"
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
                            $<INSTALL_INTERFACE:include>
                           )

# link TextReader to oliver_compiler_flags
target_link_libraries(TextReader PUBLIC 
                      TextSupport
                      oliver_compiler_flags
                     )

# install libs
set(installable_libs TextReader oliver_compiler_flags)
if(TARGET TextReader)
  list(APPEND installable_libs TextReader)
endif()
install(TARGETS ${installable_libs}
        EXPORT TextReaderTargets
        DESTINATION lib)

# install include headers
install(FILES text_reader.h DESTINATION include)
