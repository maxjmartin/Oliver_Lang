#pragma once

/*****************************************************************************************/
//
//                           Copyright(C) 2023 Max J Martin
//
//                            This file is part of Oliver.
//                      Oliver is program language interpreter. 
//    
//        This program is free software : you can redistribute it and /or modify
//        it under the terms of the GNU Affero General Public License as published by
//        the Free Software Foundation, either version 3 of the License, or
//        (at your option) any later version.
//    
//        This program is distributed in the hope that it will be useful,
//        but WITHOUT ANY WARRANTY; without even the implied warranty of
//        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
//        GNU Affero General Public License for more details.
//    
//        You should have received a copy of the GNU Affero General Public License
//        along with this program.If not, see <https://www.gnu.org/licenses/>.
//    
//        The author can be reached at: maxjmartin@gmail.com
//
/*****************************************************************************************/

#include "Var.h"
#include "Expression.h"
#include "Number.h"
#include "Text.h"
#include "Symbol.h"
#include "OpCall.h"
#include "Error.h"
#include "Boolean.h"
#include "Error.h"
//#include "Function.h"
#include "List.h"
//#include "Object.h"
#include "Format.h"


namespace Oliver {

	//using order = std::partial_ordering;

 //   const enum class op_code;

 //   class var_test {
 //       struct interface_type;
 //       template<typename T> struct data_type;

 //   public:

 //       var_test();
 //       template <typename T> var_test(T  x);

 //       constexpr std::string    str(const Format_Args& fmt)  const;  // String representation of the object, with FMT.
 //       std::string             type()                        const;  // The class generated type name.
 //       op_code              op_call()                        const;  // Get the operator code from the operator class.
 //       std::size_t        size_type()                        const;  // Convert the object to a size_type.
 //       std::int64_t    integer_type()                        const;  // Convert the object to a 64 bit integer.

 //       operator bool()                                       const;  // Get the boolean conversion of the object.

 //       bool        is_nothing()                              const;
 //       bool      is_something()                              const;
 //       bool       is_function()                              const;

 //       bool       operator ==(const autp&& other)                  const;
 //       order      operator<=>(const autp&& other)                  const;

 //       var_test          operator&(var_test n)                              ;
 //       var_test          operator|(var_test n)                              ;
 //       var_test          operator^(var_test n)                              ;
 //       var_test          operator~()                                   ;

 //       var_test          operator+()                                   ;
 //       var_test          operator-()                                   ;

 //       var_test          operator+(var_test n)                              ;
 //       var_test          operator-(var_test n)                              ;
 //       var_test          operator*(var_test n)                              ;
 //       var_test          operator/(var_test n)                              ;
 //       var_test          operator%(var_test n)                              ;

 //       var_test                pow(var_test n)                              ;  // Raise to the power of.
 //       var_test               root(var_test n)                              ;  // Reduce to the root of.
 //       var_test               real()                                   ;  // The real value of a number
 //       var_test               imag()                                   ;  // The imaginary value of a number.
 //       var_test                abs()                                   ;  // Absolute value of an object.

 //       var_test               lead()                                   ;  // Lead element of an object.
 //       var_test               push(var_test n)                              ;  // Place an object as the lead element.
 //       var_test               drop()                                   ;  // Remove the lead element from an object.
 //       var_test              shift()                                   ;  // Get and drop the lead element of a sequence.  
 //       var_test            reverse()                                   ;  // Reverse the order of an object's elements.

 //       var_test         get(var_test index)                                 ;  // Get the value of an index.
 //       var_test         set(var_test index, var_test value)                      ;  // Set the value of an index.
 //       var_test         del(var_test index)                                 ;  // Delete the value of an index.
 //       var_test         has(var_test index)                                 ;  // Determine if index is a member of the object.

 //       //std::string              help()                      const;  // Define a string description of the object.

 //       //// var_test operator()(var_test a, var_test b, var_test c, var_test d, var_test e, var_test f) const;
 //       //var_test operator()(std::initializer_list<var_test> l) const;
 //   private:

 //       struct Base {
 //           void name(this auto&& self) {
 //               self.impl();
 //           }

 //           int add(this auto&& self, auto&& other) {
 //               return self.add(other);
 //           }
 //       };

 //       std::unique_ptr<interface_type> self;

 //   };

 //   template <typename T>
 //   struct data_type {

 //       /******************************************************************************************/
 //       //
 //       //                                 'data_type' Class Definition
 //       //
 //       //             The interface implementation of the 'interface_type' data type.
 //       //             Defining a shared const pointer of the data type passed to it.
 //       //
 //       /******************************************************************************************/

 //       bool            is(this auto&& self)            const;
 //       std::string     type(this auto&& self)                         const;
 //       std::size_t     size_type(this auto&& self)                    const;
 //       std::int64_t    integer_type(this auto&& self)                 const;

 //       bool            is(this auto&& self)                           const;
 //       std::string     str(const Format_Args& fmt)    const;

 //       bool            is_nothing(this auto&& self)                   const;
 //       bool            is_function(this auto&& self)                  const;

 //       order         comp(this auto&& self, const auto&& other)               const;

 //       var_test             l_and(this auto&& self, auto&& other);
 //       var_test             l_or(this auto&& self, auto&& other);
 //       var_test             l_xor(this auto&& self, auto&& other);
 //       var_test             l_neg(this auto&& self);

 //       var_test             u_add(this auto&& self);

 //       var_test             add(this auto&& self, auto&& other);
 //       var_test             sub(this auto&& self, auto&& other);
 //       var_test             mul(this auto&& self, auto&& other);
 //       var_test             div(this auto&& self, auto&& other);
 //       var_test             mod(this auto&& self, auto&& other);

 //       var_test             pow(this auto&& self, auto&& other);
 //       var_test             root(this auto&& self, auto&& other);
 //       var_test             real(this auto&& self);
 //       var_test             imag(this auto&& self);
 //       var_test             abs(this auto&& self);

 //       var_test             lead(this auto&& self);
 //       var_test             push(this auto&& self, auto&& other);
 //       var_test             drop(this auto&& self);
 //       var_test             shift(this auto&& self);
 //       var_test             reverse(this auto&& self);

 //       var_test             get(this auto&& self, auto&& other);
 //       var_test             set(this auto&& self, auto&& index, autp&& other);
 //       var_test             del(this auto&& self, auto&& other);
 //       var_test             has(this auto&& self, auto&& other);

 //       op_code         op_call(this auto&& self)                      const;

 //       T data;

 //       std::unique_ptr<interface_type> clone() const;
 //   };
}