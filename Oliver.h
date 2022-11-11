#ifndef OLIVER_H
#define OLIVER_H

/*********************************************************************/
//
//			Copyright 2022 Max J. Martin
//
//			This file is part of Oliver.
// 
//			Oliver is free software : you can redistribute it
//			and /or modify it under the terms of the GNU General
//			Public License as published by the Free Software
//			Foundation, either version 3 of the License, or
//			(at your option) any later version.
//			
//			Oliver is distributed in the hope that it will be
//			useful, but WITHOUT ANY WARRANTY; without even the
//			implied warranty of MERCHANTABILITY or FITNESS FOR
//			A PARTICULAR PURPOSE.See the GNU General Public
//			
//			You should have received a copy of the GNU General
//			Public License along with Oliver.If not, see
//			< https://www.gnu.org/licenses/>.
//			
/*********************************************************************/

#include <iostream>

#include "../Oliver_Lang/Components/Evaluator/evaluator.h"

Olly::tokens_type parse_input(const Olly::str_type& input);

Olly::let compile_tokens(const Olly::tokens_type& tokens);

Olly::let eval_input(Olly::str_type input);

Olly::let eval(const Olly::str_type text);

#endif // OLIVER_H