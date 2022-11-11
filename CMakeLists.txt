#######################################################################
#
#			Copyright 2022 Max J. Martin
#
#			This file is part of Oliver.
# 
#			Oliver is free software : you can redistribute it
#			and /or modify it under the terms of the GNU General
#			Public License as published by the Free Software
#			Foundation, either version 3 of the License, or
#			(at your option) any later version.
#			
#			Oliver is distributed in the hope that it will be
#			useful, but WITHOUT ANY WARRANTY; without even the
#			implied warranty of MERCHANTABILITY or FITNESS FOR
#			A PARTICULAR PURPOSE.See the GNU General Public
#			
#			You should have received a copy of the GNU General
#			Public License along with Oliver.If not, see
#			< https://www.gnu.org/licenses/>.
#			
#######################################################################

##################################################
#
#   Define project main function configuration.
#
##################################################
add_executable (Oliver "Oliver.cpp" "Oliver.h")

##################################################
#
#     Add tests and install targets if needed.
#
##################################################
enable_testing()

add_test(NAME Runs COMMAND Oliver 25)                   # Does the program run?

add_test(NAME HelpDef COMMAND Oliver)                   # Does the help message work?
set_tests_properties(HelpDef
    PROPERTIES PASS_REGULAR_EXPRESSION "Usage:.*number"
)

function(do_test target arg result)                     # Define a helper function to
  add_test(NAME Comp${arg} COMMAND ${target} ${arg})    # test input text strings to an
  set_tests_properties(Comp${arg}                       # output result string.
    PROPERTIES PASS_REGULAR_EXPRESSION ${result}
    )
endfunction()

##################################################
#
#        Define 'do_test' invocations.
#
##################################################
do_test(Oliver '4' "4")