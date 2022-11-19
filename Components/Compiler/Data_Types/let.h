#ifndef LET_H
#define LET_H

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

#include "base_configuration/system_fundamentals.h"
#include "../../string_support_functions.h"
#include "base_configuration/op_codes.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                                'let' Class Definition
    //
    //          The 'let' class represents an immutable object wrapper.  It will accept
    //          any object assigned to by the assignment operator '='.
    //              Example:  let a = 42;
    //
    //          The 'let' class supports both a functional and object oriented API.
    //          But it requires that any object utilizing its API override a series of
    //          functions described below, in order to utilize run-time polymorphism.
    //
    //          The 'let' class also supports both pass by reference and pass by value.
    //          Any object it holds can be safely cast to a pointer of the object type.
    //          If the type cast is made to an incorrect data type then a std::nothing_ptr
    //          is returned instead.
    //
    //          The fundamental structure of the 'let' data type was inspired and extended
    //          from a presentation entitled:
    //                 Title: Value Semantics and Concept-based Polymorphism
    //                 By - Sean Parent
    //                 (http://sean-parent.stlab.cc/papers-and-presentations)
    //
    //          The class is defined using explicit inclusion instantiation, a class
    //          declaration is defined, followed by the class definition.
    //
    /********************************************************************************************/
    const enum class OP_CODE;

    class let {
        struct interface_type;

    public:

        let();
        template <typename T>          let(T  x);
        template <typename T>          let(T* x);

        template <typename T> const T* cast()                              const;  // Cast the object as an instance of the specified type.
        template <typename T>       T  copy()                              const;  // Get a copy of the specified type.

        str_type             id()                                          const;  // Return the typeid of the object.
        bool_type       is_type(const let& other)                          const;  // Compair two objects by typeid.
        size_type          hash()                                          const;  // Get the hash of an object.

        str_type           type()                                          const;  // The class generated type name.
        bool_type            is()                                          const;  // Is or is not the object defined.
        void                str(stream_type& out)                          const;  // String representation of the object.
        void               repr(stream_type& out)                          const;

        real_type          comp(const let& other)                          const;  // Compare two objects. 0 = equality, > 0 = grater than, < 0 = less than.
        bool_type            eq(const let& other)                          const;  // Equal to.
        bool_type            ne(const let& other)                          const;  // Not equal to.
        bool_type            ge(const let& other)                          const;  // Greater than equal to.
        bool_type            le(const let& other)                          const;  // Less than equal to.
        bool_type            gt(const let& other)                          const;  // Greater than.
        bool_type            lt(const let& other)                          const;  // Less than.

        let               l_and(const let& other)                          const;  // Logical and.
        let                l_or(const let& other)                          const;  // Logical or.
        let               l_xor(const let& other)                          const;  // Logical exlusive or.
        let                 neg()                                          const;  // Negation.

        let                 add(const let& other)                          const;  // Addition.
        let                 sub(const let& other)                          const;  // Subtraction.
        let                 mul(const let& other)                          const;  // Multiplication.
        let                 div(const let& other)                          const;  // Division.
        let                 mod(const let& other)                          const;  // Modulus.

        let               f_div(const let& other)                          const;  // Floor divide.
        let                 rem(const let& other)                          const;  // Remainder.
        let                 pow(const let& other)                          const;  // Raise to the power of.

        bool_type           has(const let& other)                          const;  // Determine if an object has an element.
        size_type          size()                                          const;  // Size of an object.
        let                lead()                                          const;  // Lead element of an object.
        let                join(const let& other)                          const;  // Place an object as the lead element.
        let                drop()                                          const;  // Remove the lead element from an object.

        let             reverse()                                          const;  // Reverse the order of an object's elements.
        let               clear()                                          const;  // Reverse the order of an object's elements.

        let                 get(const let& key)                            const;  // Get an element from a collection.
        let                 set(const let& key, const let& val)            const;  // Set the value of an element in a collection.
        let                 del(const let& key)                            const;  // Delete an element from a collection.

        let            get_pair()                                          const;  // Get a pair of elements from an object.
        int_type    get_integer()                                          const;  // Get an integer representation of an object.
        let      get_expression()                                          const;  // Convert an object to an expression.

        bool_type      iterable()                                          const;
        bool_type    is_nothing()                                          const;
        bool_type  is_something()                                          const;
        bool_type        is_set()                                          const;
        bool_type        atomic()                                          const;
        bool_type           nan()                                          const;
        bool_type       complex()                                          const;

        OP_CODE         op_code()                                          const;

        str_type           help()                                          const;  // Define a string description of the object.

        // General object operator overloads.

        bool_type    operator==(const let& other)                          const;
        bool_type    operator!=(const let& other)                          const;
        bool_type    operator>=(const let& other)                          const;
        bool_type    operator> (const let& other)                          const;
        bool_type    operator<=(const let& other)                          const;
        bool_type    operator< (const let& other)                          const;

        let           operator+(const let& other)                          const;
        let           operator-(const let& other)                          const;
        let           operator*(const let& other)                          const;
        let           operator/(const let& other)                          const;
        let           operator%(const let& other)                          const;

    private:

        struct interface_type {

            /********************************************************************************************/
            //
            //                              'interface_type' Class Definition
            //
            //       A simple interface description allowing redirection of the 'let' data type.
            //
            /********************************************************************************************/

            virtual  ~interface_type()                                              = default;
            virtual operator bool()                                                 const = 0;

            virtual void* _vptr()                                                         = 0;
            virtual str_type         _id()                                          const = 0;
            virtual std::size_t      _hash()                                        const = 0;

            virtual str_type         _type()                                        const = 0;
            virtual bool_type        _is()                                          const = 0;
            virtual void             _str(stream_type& out)                         const = 0;
            virtual void             _repr(stream_type& out)                        const = 0;

            virtual real_type        _comp(const let& other)                        const = 0;

            virtual let             _l_and(const let& other)                        const = 0;
            virtual let             _l_or(const let& other)                         const = 0;
            virtual let             _l_xor(const let& other)                        const = 0;
            virtual let             _neg()                                          const = 0;

            virtual let             _add(const let& other)                          const = 0;
            virtual let             _sub(const let& other)                          const = 0;
            virtual let             _mul(const let& other)                          const = 0;
            virtual let             _div(const let& other)                          const = 0;
            virtual let             _mod(const let& other)                          const = 0;

            virtual let             _f_div(const let& other)                        const = 0;
            virtual let             _rem(const let& other)                          const = 0;
            virtual let             _pow(const let& other)                          const = 0;

            virtual bool_type       _has(const let& other)                          const = 0;
            virtual size_type       _size()                                         const = 0;
            virtual let             _lead()                                         const = 0;
            virtual let             _join(const let& other)                         const = 0;
            virtual let             _drop()                                         const = 0;

            virtual let             _reverse()                                      const = 0;
            virtual let             _clear()                                        const = 0;

            virtual let             _get(const let& key)                            const = 0;
            virtual let             _set(const let& key, const let& val)            const = 0;
            virtual let             _del(const let& key)                            const = 0;

            virtual let             _get_pair()                                     const = 0;
            virtual int_type        _get_integer()                                  const = 0;
            virtual let             _get_expression()                               const = 0;

            virtual bool_type       _iterable()                                     const = 0;
            virtual bool_type       _is_nothing()                                   const = 0;
            virtual bool_type       _is_something()                                 const = 0;
            virtual bool_type       _is_set()                                       const = 0;
            virtual bool_type       _atomic()                                       const = 0;
            virtual bool_type       _nan()                                          const = 0;
            virtual bool_type       _complex()                                      const = 0;

            virtual str_type        _help()                                         const = 0;

            virtual OP_CODE         _op_code()                                      const = 0;
        };

        template <typename T>
        struct data_type : interface_type {

            /******************************************************************************************/
            //
            //                                 'data_type' Class Definition
            //
            //             The int_typeerface implementation of the 'interface_type' data type.
            //             Defining a shared const point_typeer of the data type passed to it.
            //
            /******************************************************************************************/

            data_type(T val);
            operator bool()                                                 const;

            void* _vptr();
            str_type        _id()                                           const;
            std::size_t     _hash()                                         const;

            str_type        _type()                                         const;
            bool_type       _is()                                           const;
            void            _str(stream_type& out)                          const;
            void            _repr(stream_type& out)                         const;

            real_type       _comp(const let& other)                         const;

            let             _l_and(const let& other)                        const;
            let             _l_or(const let& other)                         const;
            let             _l_xor(const let& other)                        const;
            let             _neg()                                          const;

            let             _add(const let& other)                          const;
            let             _sub(const let& other)                          const;
            let             _mul(const let& other)                          const;
            let             _div(const let& other)                          const;
            let             _mod(const let& other)                          const;

            let             _f_div(const let& other)                        const;
            let             _rem(const let& other)                          const;
            let             _pow(const let& other)                          const;

            bool_type       _has(const let& other)                          const;
            size_type       _size()                                         const;
            let             _lead()                                         const;

            let             _join(const let& other)                         const;
            let             _drop()                                         const;

            let             _reverse()                                      const;
            let             _clear()                                        const;

            let             _get(const let& key)                            const;
            let             _set(const let& key, const let& val)            const;
            let             _del(const let& key)                            const;

            let             _get_pair()                                     const;
            int_type        _get_integer()                                  const;
            let             _get_expression()                               const;

            bool_type       _iterable()                                     const;
            bool_type       _is_nothing()                                   const;
            bool_type       _is_something()                                 const;
            bool_type       _is_set()                                       const;
            bool_type       _atomic()                                       const;
            bool_type       _nan()                                          const;
            bool_type       _complex()                                      const;

            str_type        _help()                                         const;

            OP_CODE         _op_code()                                      const;

            T               _data;
        };

        std::shared_ptr<const interface_type> _self;
    };

    /********************************************************************************************/
    //
    //                                 'nothing' Class Definition
    //
    //          A basic class definition of the value of nothing.  This is used within
    //          'let' class implementation to return a result of nothing for results
    //          which have either conflicting types, or in some cases as the default to
    //          return unless overridden.
    //
    //          This class also demonstrates the basic function methods that should be
    //          over written for proper object behavior.
    //
    /********************************************************************************************/

    class nothing {

    public:

        nothing();
        virtual ~nothing();

        nothing(nothing&& n)                  = default;
        nothing(const nothing& n)             = default;
        nothing& operator= (nothing&& n)      = default;
        nothing& operator= (const nothing& n) = default;

        friend str_type         _type_(const nothing& self);
        friend bool_type          _is_(const nothing& self);
        friend real_type        _comp_(const nothing& self, const let& other);

        friend void              _str_(stream_type& out, const nothing& self);
        friend void             _repr_(stream_type& out, const nothing& self);

        friend bool_type  _is_nothing_(const nothing& self);
    };

    /********************************************************************************************/
    //
    //                                Basic Primitive Declarations
    //
    //          These definitions add a few useful and some necessary support functions.
    //
    /********************************************************************************************/

    void print();                             // Simply print a new line.
    void print(const str_type& str);          // Accept any single string and print it with a std::endl.
    void print(const let& a);                 // Accept any single 'let' and print it with a std::endl.

    str_type  str(const let& a);             // Convert any 'let' to a str_type.
    str_type repr(const let& a);              // Convert any 'let' to a str_type representation of the 'let'.

    bool_type expression_is_empty(let expr);  // Determine if an expression is empty.
    bool_type object_is_empty(let expr);      // Determine if an object is empty.

    let pop_lead(let& expr);                  // Return the lead element of an object and shift it.

    let make_pair(let key, let val);          // Define a pair of two elements.

    let  first(let p);                        // Return the first element of an expression.
    let second(let p);                        // Return the second element of an expression.
    let  third(let p);                        // Return the third element of an expression.
    let fourth(let p);                        // Return the fourth element of an expression.
    let  fifth(let p);                        // Return the fifth element of an expression.

    let unwrap_expresion(let exp);            // Remove unecessary surounding expressions.  

    template <typename N> std::vector<int_type> sieve(N n);         // Perform a primality sieve to 'n' primes.
    template <typename I, typename N> void sift(I first, N n);
    template <typename I, typename N> void mark_sieve(I first, I last, N factor);
    template <typename I, typename N> std::vector<N> get_primes(I first, N n);

    /********************************************************************************************/
    //
    //                      'let' Class Function Default Template API.
    //
    //           The following function templates define the over-ridable functions which
    //           may be used to tailor the behavior of any object held within a 'let'.
    //
    //           Each function defined here defines the default behavior of each function
    //           which is invoked if a function is not overwritten for a defined class.
    //
    /********************************************************************************************/


    template<typename T>            /****  Hash Value  ****/
    std::size_t _hash_(const T& self);

    template<typename T>
    inline std::size_t _hash_(const T& self) {
        return DEFAULT_HASH_FUNCTION(repr(self));
    }


    template<typename T>            /****  Type Name  ****/
    str_type _type_(const T& self);

    template<typename T>
    inline str_type _type_(const T& self) {
        return typeid(self).name();
    }


    template<typename T>            /****  Boolean Conversion  ****/
    bool_type _is_(const T& self);

    template<typename T>
    inline bool_type _is_(const T& self) {
        return false;
    }


    template<typename T>            /****  String Conversion  ****/
    void _str_(stream_type& out, const T& self);

    template<typename T>
    inline void _str_(stream_type& out, const T& self) {
        out << "object<" << &self << "," << _type_(self) << ">";
    }


    template<typename T>            /****  String Representation  ****/
    void _repr_(stream_type& out, const T& self);

    template<typename T>
    inline void _repr_(stream_type& out, const T& self) {
        out << str(nothing());
    }


    template<typename T>            /****  Comparison Between Variables  ****/
    real_type _comp_(const T& self, const let& other);

    template<typename T>
    inline real_type _comp_(const T& self, const let& other) {
        return NOT_A_NUMBER;
    }


    template<typename T>            /****  Logical Conjunction  ****/
    let _l_and_(const T& self, const let& other);

    template<typename T>
    inline let _l_and_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Logical Inclusive Disjunction  ****/
    let _l_or_(const T& self, const let& other);

    template<typename T>
    inline let _l_or_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Logical Exclusive Disjunction  ****/
    let _l_xor_(const T& self, const let& other);

    template<typename T>
    inline let _l_xor_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Negation  ****/
    let _neg_(const T& self);

    template<typename T>
    inline let _neg_(const T& self) {
        return self;
    }


    template<typename T>            /****  Addition or Concatenation  ****/
    let _add_(const T& self, const let& other);

    template<typename T>
    inline let _add_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Subtraction or Deletion  ****/
    let _sub_(const T& self, const let& other);

    template<typename T>
    inline let _sub_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Multiplication  ****/
    let _mul_(const T& self, const let& other);

    template<typename T>
    inline let _mul_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Division  ****/
    let _div_(const T& self, const let& other);

    template<typename T>
    inline let _div_(const T& self, const let& other) {
        return nothing();
    }

    template<typename T>            /****  Modulation  ****/
    let _mod_(const T& self, const let& other);

    template<typename T>
    inline let _mod_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Floor Division  ****/
    let _f_div_(const T& self, const let& other);

    template<typename T>
    inline let _f_div_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Remainder  ****/
    let _rem_(const T& self, const let& other);

    template<typename T>
    inline let _rem_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Raise to Power of  ****/
    let _pow_(const T& self, const let& other);

    template<typename T>
    inline let _pow_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Check if an object has an element  ****/
    bool_type _has_(const T& self, const let& other);

    template<typename T>
    inline bool_type _has_(const T& self, const let& other) {
        return false;
    }


    template<typename T>            /****  Length Of  ****/
    size_type _size_(const T& self);

    template<typename T>
    inline size_type _size_(const T& self) {
        return 0;
    }


    template<typename T>            /****  Lead Element Of  ****/
    let _lead_(const T& self);

    template<typename T>
    inline let _lead_(const T& self) {
        return nothing();
    }


    template<typename T>            /**** Perpend Lead Element Of  ****/
    let _join_(const T& self, const let& other);

    template<typename T>
    inline let _join_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Drop The Leading Element  ****/
    let _drop_(const T& self);

    template<typename T>
    inline let _drop_(const T& self) {
        return nothing();
    }


    template<typename T>            /****  Reverse The Elements Of  ****/
    let _reverse_(const T& self);

    template<typename T>
    inline let _reverse_(const T& self) {
        return nothing();
    }


    template<typename T>            /****  Clear The Elements From  ****/
    let _clear_(const T& self);

    template<typename T>
    inline let _clear_(const T& self) {
        return nothing();
    }


    template<typename T>            /****  Retrieve A Selection From  ****/
    let _get_(const T& self, const let& other);

    template<typename T>
    inline let _get_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Set A Selection Of  ****/
    let _set_(const T& self, const let& other, const let& val);

    template<typename T>
    inline let _set_(const T& self, const let& other, const let& val) {
        return nothing();
    }


    template<typename T>            /****  Remove A Selection From  ****/
    let _del_(const T& self, const let& other);

    template<typename T>
    inline let _del_(const T& self, const let& other) {
        return nothing();
    }


    template<typename T>            /****  Get A Pair Of Values From  ****/
    let _get_pair_(const T& self);

    template<typename T>
    inline let _get_pair_(const T& self) {
        return nothing();
    }


    template<typename T>            /**** Get An Integer Value From Object  ****/
    int_type _get_integer_(const T& self);

    template<typename T>
    inline int_type _get_integer_(const T& self) {
        return 0;
    }


    template<typename T>            /**** Get An Expression Value From Object  ****/
    let _get_expression_(const T& self);

    template<typename T>
    inline let _get_expression_(const T& self) {
        return nothing();
    }


    template<typename T>            /****  Bind Variable Scope To An Object  ****/
    let _bind_scope_(const T& self);

    template<typename T>
    inline let _bind_scope_(const T& self) {
        return nothing();
    }


    template<typename T>            /****  Is Something iterabley Ordered  ****/
    bool_type _iterable_(const T& self);

    template<typename T>
    inline bool_type _iterable_(const T& self) {
        return false;
    }


    template<typename T>            /****  Is Nothing Type  ****/
    bool_type _is_nothing_(const T& self);

    template<typename T>
    inline bool_type _is_nothing_(const T& self) {
        return false;
    }


    template<typename T>            /****  Is A Defined Type  ****/
    bool_type _is_something_(const T& self);

    template<typename T>
    inline bool_type _is_something_(const T& self) {
        return true;
    }


    template<typename T>            /****  Is Set Type  ****/
    bool_type _is_set_(const T& self);

    template<typename T>
    inline bool_type _is_set_(const T& self) {
        return false;
    }


    template<typename T>            /****  Is Atomic Type  ****/
    bool_type _atomic_(const T& self);

    template<typename T>
    inline bool_type _atomic_(const T& self) {
        return false;
    }


    template<typename T>            /****  Is Not A Number  ****/
    bool_type _nan_(const T& self);

    template<typename T>
    inline bool_type _nan_(const T& self) {
        return false;
    }


    template<typename T>            /****  Is A Complex Number  ****/
    bool_type _complex_(const T& self);

    template<typename T>
    inline bool_type _complex_(const T& self) {
        return false;
    }


    template<typename T>            /****  Return A Help String ****/
    str_type _help_(const T& self);

    template<typename T>
    inline str_type _help_(const T& self) {
        return "No object documentation available.";
    }


    template<typename T>            /****  Return An Operation Code ****/
    OP_CODE _op_code_(const T& self);

    template<typename T>
    inline OP_CODE _op_code_(const T& self) {
        return OP_CODE::nothing_op;
    }


    /********************************************************************************************/
    //
    //                                 'nothing' Class Implimentation
    //
    /********************************************************************************************/

    inline nothing::nothing() {
    }

    inline nothing::~nothing() {
    }

    inline str_type _type_(const nothing& self) {
        return "nothing";
    }

    inline bool_type _is_(const nothing& self) {
        return false;
    }

    inline real_type _comp_(const nothing& self, const let& other) {
        return NOT_A_NUMBER;
    }

    inline void _str_(stream_type& out, const nothing& self) {
        out << "nothing";
    }

    inline void _repr_(stream_type& out, const nothing& self) {
        _str_(out, self);
    }

    inline bool_type _is_nothing_(const nothing& self) {
        return true;
    }

    inline bool_type _is_something_(const nothing& self) {
        return false;
    }



    /********************************************************************************************/
    //
    //                                'let' Class Implementation
    //
    /********************************************************************************************/

    inline let::let() : _self(std::make_shared<data_type<Olly::nothing>>(Olly::nothing())) {
    }

    template <typename T>
    inline let::let(T x) : _self(std::make_shared<data_type<T>>(std::move(x))) {
    }

    template <typename T>
    inline let::let(T* x) : _self(std::make_shared<data_type<T>>(x)) {
    }

    template <typename T> const inline T* let::cast() const {

        let n = T();

        if (is_type(n)) {

            const T* p = static_cast<T*>(const_cast<interface_type*>(_self.get())->_vptr());

            if (p) {
                return p;
            }
        }

        return nullptr;
    }

    template <typename T> inline T let::copy() const {

        T n = T();

        if (is_type(n)) {

            const T* p = static_cast<T*>(const_cast<interface_type*>(_self.get())->_vptr());

            if (p) {
                n = *p;
            }
        }

        return n;
    }

    inline str_type let::id() const {
        return _self->_id();
    }

    inline bool_type let::is_type(const let& other) const {
        return _self->_id() == other._self->_id();
    }

    inline std::size_t let::hash() const {
        return _self->_hash();
    }

    inline str_type let::type() const {
        return _self->_type();
    }

    inline bool_type let::is() const {
        return const_cast<interface_type*>(_self.get())->_is();
    }

    inline void let::str(stream_type& out) const {
        _self->_str(out);
    }

    inline void let::repr(stream_type& out) const {
        _self->_repr(out);
    }

    inline real_type let::comp(const let& other) const {
        return _self->_comp(other);
    }

    inline bool_type let::eq(const let& other) const {
        return (comp(other) == 0.0 ? true : false);
    }

    inline bool_type let::ne(const let& other) const {
        return (comp(other) != 0.0 ? true : false);
    }

    inline bool_type let::ge(const let& other) const {
        return (comp(other) >= 0.0 ? true : false);
    }

    inline bool_type let::le(const let& other) const {
        return (comp(other) <= 0.0 ? true : false);
    }

    inline bool_type let::gt(const let& other) const {
        return (comp(other) > 0.0 ? true : false);
    }

    inline bool_type let::lt(const let& other) const {
        return (comp(other) < 0.0 ? true : false);
    }

    inline let let::l_and(const let& other) const {
        return _self->_l_and(other);
    }

    inline let let::l_or(const let& other) const {
        return _self->_l_or(other);
    }

    inline let let::l_xor(const let& other) const {
        return _self->_l_xor(other);
    }

    inline let let::neg() const {
        return _self->_neg();
    }

    inline let let::add(const let& other) const {
        return _self->_add(other);
    }

    inline let let::sub(const let& other) const {
        return _self->_sub(other);
    }

    inline let let::mul(const let& other) const {
        return _self->_mul(other);
    }

    inline let let::div(const let& other) const {
        return _self->_div(other);
    }

    inline let let::mod(const let& other) const {
        return _self->_mod(other);
    }

    inline let let::f_div(const let& other) const {
        return _self->_f_div(other);
    }

    inline let let::rem(const let& other) const {
        return _self->_rem(other);
    }

    inline let let::pow(const let& other) const {
        return _self->_pow(other);
    }

    inline bool_type let::has(const let& other) const {
        return _self->_has(other);
    }

    inline size_type let::size() const {
        return _self->_size();
    }

    inline let let::lead() const {
        return _self->_lead();
    }

    inline let let::join(const let& other) const {
        return _self->_join(other);
    }

    inline let let::drop() const {
        return _self->_drop();
    }

    inline let let::reverse() const {
        return _self->_reverse();
    }

    inline let let::clear() const {
        return _self->_clear();
    }

    inline let let::get(const let& other) const {
        return _self->_get(other);
    }

    inline let let::set(const let& other, const let& val) const {
        return _self->_set(other, val);
    }

    inline let let::del(const let& other) const {
        return _self->_del(other);
    }

    inline let let::get_pair() const {
        return _self->_get_pair();
    }

    inline int_type let::get_integer() const {
        return _self->_get_integer();
    }

    inline let let::get_expression() const {
        return _self->_get_expression();
    }

    inline bool_type let::iterable() const {
        return _self->_iterable();
    }

    inline bool_type let::is_nothing() const {
        return _self->_is_nothing();
    }

    inline bool_type let::is_something() const {
        return _self->_is_something();
    }

    inline bool_type let::is_set() const {
        return _self->_is_set();
    }

    inline bool_type let::atomic() const {
        return _self->_atomic();
    }

    inline bool_type let::nan() const {
        return _self->_nan();
    }

    inline bool_type let::complex() const {
        return _self->_complex();
    }

    inline OP_CODE let::op_code() const {
        return _self->_op_code();
    }

    inline str_type let::help() const {
        return _self->_help();
    }

    inline bool_type let::operator==(const let& other) const {
        return eq(other);
    }

    inline bool_type let::operator!=(const let& other) const {
        return ne(other);
    }

    inline bool_type let::operator>=(const let& other) const {
        return ge(other);
    }

    inline bool_type let::operator> (const let& other) const {
        return gt(other);
    }

    inline bool_type let::operator<=(const let& other) const {
        return le(other);
    }

    inline bool_type let::operator< (const let& other) const {
        return lt(other);
    }

    inline let let::operator+(const let& other) const {
        return add(other);
    }

    inline let let::operator-(const let& other) const {
        return sub(other);
    }

    inline let let::operator*(const let& other) const {
        return mul(other);
    }

    inline let let::operator/(const let& other) const {
        return div(other);
    }

    inline let let::operator%(const let& other) const {
        return mod(other);
    }

    /********************************************************************************************/
    //
    //                                'data_type' Class Implementation
    //
    /********************************************************************************************/

    template <typename T>
    inline let::data_type<T>::data_type(T val) : _data(std::move(val)) {
    }

    template <typename T>
    inline let::data_type<T>::operator bool() const {
        return _is();
    }

    template <typename T>
    inline void* let::data_type<T>::_vptr() {
        return &_data;
    }

    template <typename T>
    inline str_type let::data_type<T>::_id() const {
        return typeid(_data).name();
    }

    template <typename T>
    inline std::size_t let::data_type<T>::_hash() const {
        return _hash_(_data);
    }

    template <typename T>
    inline str_type let::data_type<T>::_type() const {
        return _type_(_data);
    }

    template <typename T>
    inline bool_type let::data_type<T>::_is() const {
        return _is_(_data);
    }

    template <typename T>
    inline void let::data_type<T>::_str(stream_type& out) const {
        _str_(out, _data);
    }

    template <typename T>
    inline void let::data_type<T>::_repr(stream_type& out) const {
        _repr_(out, _data);
    }

    template <typename T>
    inline real_type let::data_type<T>::_comp(const let& other) const {
        return _comp_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_l_and(const let& other) const {
        return _l_and_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_l_or(const let& other) const {
        return _l_or_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_l_xor(const let& other) const {
        return _l_xor_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_neg() const {
        return _neg_(_data);
    }

    template <typename T>
    inline let let::data_type<T>::_add(const let& other) const {
        return _add_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_sub(const let& other) const {
        return _sub_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_mul(const let& other) const {
        return _mul_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_div(const let& other) const {
        return _div_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_mod(const let& other) const {
        return _mod_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_f_div(const let& other) const {
        return _f_div_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_rem(const let& other) const {
        return _rem_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_pow(const let& other) const {
        return _pow_(_data, other);
    }

    template <typename T>
    inline bool_type let::data_type<T>::_has(const let& other) const {
        return _has_(_data, other);
    }

    template <typename T>
    inline size_type let::data_type<T>::_size() const {
        return _size_(_data);
    }

    template <typename T>
    inline let let::data_type<T>::_lead() const {
        return _lead_(_data);
    }

    template <typename T>
    inline let let::data_type<T>::_join(const let& other) const {
        return _join_(_data, other);
    }

    template <typename T>
    inline let let::data_type<T>::_drop() const {
        return _drop_(_data);
    }

    template <typename T>
    inline let let::data_type<T>::_reverse() const {
        return _reverse_(_data);
    }

    template <typename T>
    inline let let::data_type<T>::_clear() const {
        return _clear_(_data);
    }


    template <typename T>
    inline let let::data_type<T>::_get(const let& key) const {
        return _get_(_data, key);
    }

    template <typename T>
    inline let let::data_type<T>::_set(const let& key, const let& val) const {
        return _set_(_data, key, val);
    }

    template <typename T>
    inline let let::data_type<T>::_del(const let& key) const {
        return _del_(_data, key);
    }

    template <typename T>
    inline let let::data_type<T>::_get_pair() const {
        return _get_pair_(_data);
    }

    template <typename T>
    inline int_type let::data_type<T>::_get_integer() const {
        return _get_integer_(_data);
    }

    template <typename T>
    inline let let::data_type<T>::_get_expression() const {
        return _get_expression_(_data);
    }

    template <typename T>
    inline bool_type let::data_type<T>::_iterable() const {
        return _iterable_(_data);
    }

    template <typename T>
    inline bool_type let::data_type<T>::_is_nothing() const {
        return _is_nothing_(_data);
    }

    template <typename T>
    inline bool_type let::data_type<T>::_is_something() const {
        return _is_something_(_data);
    }

    template <typename T>
    inline bool_type let::data_type<T>::_is_set() const {
        return _is_set_(_data);
    }

    template <typename T>
    inline bool_type let::data_type<T>::_atomic() const {
        return _atomic_(_data);
    }

    template <typename T>
    inline bool_type let::data_type<T>::_nan() const {
        return _nan_(_data);
    }

    template <typename T>
    inline bool_type let::data_type<T>::_complex() const {
        return _complex_(_data);
    }

    template <typename T>
    inline str_type let::data_type<T>::_help() const {
        return _help_(_data);
    }

    template <typename T>
    inline OP_CODE let::data_type<T>::_op_code() const {
        return _op_code_(_data);
    }

    /********************************************************************************************/
    //
    //                            Basic Primitive Implementations
    //
    /********************************************************************************************/

    inline void print() {
        std::cout << std::endl;
    }

    inline void print(const str_type& str) {
        std::cout << str << std::endl;
    }

    inline void print(const let& a) {
        print(str(a));
    }

    inline str_type str(const let& a) {
        /*
            Convert a 'let' to its string representation.
        */

        stream_type stream;

        stream << std::boolalpha;

        if (a.type() == "format") {
            /*
                The 'format' data type must be printed using
                its string representation, else it would only
                impart its formating to the stream instead of
                being printed to it.
            */
            a.repr(stream);
        }
        else {
            a.str(stream);
        }

        return stream.str();
    }

    inline str_type repr(const let& a) {
        /*
            Convert a 'let' to its representation as a string.
        */

        stream_type stream;

        stream << std::boolalpha;

        a.repr(stream);

        return stream.str();
    }

    inline bool_type expression_is_empty(let expr) {
        /*
            Return true if the expresion is empty.
        */
        return !expr.is();
    }

    inline bool_type object_is_empty(let expr) {
        /*
            Return true if the expresion is empty.
        */
        return !expr.is();
    }

    inline let pop_lead(let& expr) {

        let a = expr.lead();

        expr = expr.drop();

        return a;
    }

    inline let first(let p) {
        return p.lead();
    }

    inline let second(let p) {
        return p.drop().lead();
    }

    inline let third(let p) {
        return p.drop().drop().lead();
    }

    inline let fourth(let p) {
        return p.drop().drop().drop().lead();
    }

    inline let fifth(let p) {
        return p.drop().drop().drop().drop().lead();
    }

    inline let unwrap_expresion(let exp) {

        while (exp.is() && exp.size() == 1) {

            if (exp.lead().type() != "expression") {
                return exp;
            }
            exp = exp.lead();
        }

        return exp;
    }

    template <typename N>
    std::vector<int_type> sieve(N n) {

        n = n % 2 ? (n / 2) + 1 : n / 2;

        std::vector<int_type> r(n);

        sift(begin(r), n);

        r = get_primes(begin(r), n);

        return r;
    }

    template <typename I, typename N>
    void sift(I first, N n) {

        I last = first + n;
        // Define the last index to sift through.

        std::fill(first, last, true);
        // Fill the container and default each index to true.

        N i(0);
        N index_square(3);
        N factor(3);

        while (index_square < n) {
            // invariant: index_square = 2i^2 + 6i + 3, factor = 2i + 3

            if (first[i]) {
                // On any index which is prime sift out its factors.
                mark_sieve(first + index_square, last, factor);
            }

            i += 1;
            index_square += factor;
            factor += N(2);
            index_square += factor;
        }
    }

    template <typename I, typename N>
    void mark_sieve(I first, I last, N factor) {

        if (first == last) {
            return;
        }

        *first = false;
        // Set the current value of the iterator to false.
        // Then loop through each additional index and set
        // their value to flase.

        while (last - first > factor) {
            first = first + factor;
            *first = false;
        }
    }

    template <typename I, typename N>
    std::vector<N> get_primes(I first, N n) {

        std::vector<N> r;

        N i(0);
        N t(0);

        // std::cout << 2;

        while (i < n) {

            if (*first) {
                r.push_back(i);
                t = i;
            }

            ++first;
            ++i;
        }
        r.push_back(t);

        return r;
    }

}
#endif // LET_H