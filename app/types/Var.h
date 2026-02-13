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

#include <compare>
#include <memory>
#include <string>
#include <utility>

#include "../text_support.h"
#include "Error.h"
#include "support_types/OpCodes.h"

namespace Oliver {

    /********************************************************************************************/
    //
    //                                'var' Class Definition
    //
    //          The 'var' class represents an unique object wrapper.  It will accept
    //          any object assigned to by the assignment operator '='.
    //              Example:  var a = 42;
    //
    //          The class is designed to function within the Oliver interpreter.
    //          To facilitate a safe memory managed runtime environment.
    //
    //          Note: 'var' has no out-of-line virtual method definitions; its vtable will 
    //          be emitted in every translation unit [-Wweak-vtables]GCC
    //          Subsiquencely the 'interface_type' will also have it vtable emitted.
    //
    //          The fundamental structure of the 'var' data type was inspired and extended
    //          from a presentation entitled:
    //                 Title: Value Semantics and Concept-based Polymorphism
    //                 By - Sean Parent
    //                 (http://sean-parent.stlab.cc/papers-and-presentations)
    //
    /********************************************************************************************/

    using order = std::partial_ordering;

    enum class op_code;

    class var {
        struct interface_type;
        template<typename T> struct data_type;

    public:

        var();
        template <typename T> var(T  x);
        template <typename T> var(T* x);
        virtual ~var() noexcept;

        var(const var& other);
        var(var&& other) noexcept;
        var& operator=(const var&  other);
        var& operator=(var&& other) noexcept;

        // constexpr var* operator->() const;

        template<typename T> const T*                 cast()  const;  // Get a const pointer reference.
        template<typename T> std::unique_ptr<T>       copy()  const;  // Get a unique pointer copy of the var data.
        template<typename T> std::unique_ptr<T>       move()       ;  // Transfer ownership of the pointer.

        constexpr std::string    str(const Format_Args& fmt)  const;  // String representation of the object, with FMT.
        std::string             type()                        const;  // The class generated type name.
        op_code              op_call()                        const;  // Get the operator code from the operator class.
        std::size_t        size_type()                        const;  // Convert the object to a size_type.
        std::int64_t    integer_type()                        const;  // Convert the object to a 64 bit integer.

        operator bool()                                       const;  // Get the boolean conversion of the object.

        bool        is_nothing()                              const;
        bool      is_something()                              const;
        bool       is_function()                              const;

        bool       operator ==(const var& n)                  const;
        order      operator<=>(const var& n)                  const;

        var          operator&(var& n)                             ;
        var          operator|(var& n)                             ;
        var          operator^(var& n)                             ;
        var          operator~()                                   ;

        var          operator+()                                   ;
        var          operator-()                                   ;

        var          operator+(var& n)                             ;
        var          operator-(var& n)                             ;
        var          operator*(var& n)                             ;
        var          operator/(var& n)                             ;
        var          operator%(var& n)                             ;

        var                pow(var& n)                             ;  // Raise to the power of.
        var               root(var& n)                             ;  // Reduce to the root of.
        var               real()                                   ;  // The real value of a number
        var               imag()                                   ;  // The imaginary value of a number.
        var                abs()                                   ;  // Absolute value of an object.

        var               lead()                                   ;  // Lead element of an object.
        var               push(var& n)                             ;  // Place an object as the lead element.
        var               drop()                                   ;  // Remove the lead element from an object.
        var              shift()                                   ;  // Get and drop the lead element of a sequence.  
        var            reverse()                                   ;  // Reverse the order of an object's elements.

        var         get(var& index)                                ;  // Get the value of an index.
        var         set(var& index, var& value)                    ;  // Set the value of an index.
        var         del(var& index)                                ;  // Delete the value of an index.
        var         has(var& index)                                ;  // Determine if index is a member of the object.

        //      union -> The common elements of two sets.  Logical Or
        //      inrtersection -> All elements exclusive to both sets.  Logical And
        //      exclusion -> All elements of two set, exclusive to each set.  Logical Xor
        //      compliment -> All of the objects which do not exit in a set.  Logical negation.  
        //      diffrence - > The subtraction of a sets element from another set.  Logical Sabtraction

        //std::string              help()                      const;  // Define a string description of the object.

        //// var operator()(var& a, var& b, var& c, var& d, var& e, var& f) const;
        //var operator()(std::initializer_list<var> l) const;
    private:

        struct interface_type {

            /********************************************************************************************/
            //
            //                              'interface_type' Class Definition
            //
            //       A simple interface description allowing redirection of the 'var' data type.
            //
            /********************************************************************************************/

            virtual ~interface_type() {}

            virtual std::unique_ptr<interface_type> clone()         const = 0;

            virtual operator bool()                                 const = 0;

            virtual std::string     _type()                         const = 0;
            virtual bool            _is()                           const = 0;
            virtual std::string     _str(const Format_Args& fmt)    const = 0;
            virtual std::size_t     _size_type()                    const = 0;
            virtual std::int64_t    _integer_type()                 const = 0;

            virtual bool            _is_nothing()                   const = 0;
            virtual bool            _is_function()                  const = 0;

            virtual order           _comp(const var& n)             const = 0;

            virtual var             _and(var& n)                          = 0;
            virtual var             _or(var& n)                           = 0;
            virtual var             _xor(var& n)                          = 0;
            virtual var             _neg()                                = 0;

            virtual var             _u_add()                              = 0;

            virtual var             _add(var& n)                          = 0;
            virtual var             _sub(var& n)                          = 0;
            virtual var             _mul(var& n)                          = 0;
            virtual var             _div(var& n)                          = 0;
            virtual var             _mod(var& n)                          = 0;

            virtual var             _pow(var& n)                          = 0;
            virtual var             _root(var& n)                         = 0;
            virtual var             _real()                               = 0;
            virtual var             _imag()                               = 0;
            virtual var             _abs()                                = 0;

            virtual var             _lead()                               = 0;
            virtual var             _push(var& n)                         = 0;
            virtual var             _drop()                               = 0;
            virtual var             _shift()                              = 0;
            virtual var             _reverse()                            = 0;

            virtual var             _get(var& n)                          = 0;
            virtual var             _set(var& i, var& n)                  = 0;
            virtual var             _del(var& n)                          = 0;
            virtual var             _has(var& n)                          = 0;

            virtual op_code         _op_call()                      const = 0;
        };

        template <typename T>
        struct data_type : interface_type {

            /******************************************************************************************/
            //
            //                                 'data_type' Class Definition
            //
            //             The interface implementation of the 'interface_type' data type.
            //             Defining a shared const pointer of the data type passed to it.
            //
            /******************************************************************************************/

            data_type(T val);
            virtual ~data_type() override {}

            operator bool()                                 const override;
            std::string     _type()                         const override;
            std::size_t     _size_type()                    const override;
            std::int64_t    _integer_type()                 const override;

            bool            _is()                           const override;
            std::string     _str(const Format_Args& fmt)    const override;

            bool            _is_nothing()                   const override;
            bool            _is_function()                  const override;

            order           _comp(const var& n)             const override;

            var             _and(var& n)                          override;
            var             _or(var& n)                           override;
            var             _xor(var& n)                          override;
            var             _neg()                                override;

            var             _u_add()                              override;

            var             _add(var& n)                          override;
            var             _sub(var& n)                          override;
            var             _mul(var& n)                          override;
            var             _div(var& n)                          override;
            var             _mod(var& n)                          override;
             
            var             _pow(var& n)                          override;
            var             _root(var& n)                         override;
            var             _real()                               override;
            var             _imag()                               override;
            var             _abs()                                override;

            var             _lead()                               override;
            var             _push(var& n)                         override;
            var             _drop()                               override;
            var             _shift()                              override;
            var             _reverse()                            override;

            var             _get(var& n)                          override;
            var             _set(var& i, var& n)                  override;
            var             _del(var& n)                          override;
            var             _has(var& n)                          override;

            op_code         _op_call()                      const override;

            //template<class T>
            //class View : public std::ranges::view_interface<View<T>> {
            //public:
            //    View() = default;
            //    View(const auto& seq) : v_begin(seq.cbegin()), v_end(seq.cend()) {}

            //    auto begin() const { return v_begin; }
            //    auto   end() const { return v_end; }

            //private:
            //    typename IMPL::const_iterator v_begin{}, v_end{};
            //};

            // SeqArray<0, T>(_data);

            T _data;

            std::unique_ptr<interface_type> clone() const override;
        };

        std::unique_ptr<interface_type> _self;

        constexpr void check_is_initialized();
    };

    /********************************************************************************************/
    //
    //                                 'nothing' Class Definition
    //
    //
    //          A basic class definition of the value of nothing.  This is used within
    //          'var' class implementation to return a result of nothing for results
    //          which have either conflicting types, or in some cases as the default to
    //          return unless overridden.
    //
    //          This class also demonstrates the basic function methods that should be
    //          over written for proper object behavior.
    //
    /********************************************************************************************/

    class nothing {

    public:

        friend std::string          _type_(const nothing& self);
        friend bool                   _is_(const nothing& self);
        friend order                _comp_(const nothing& self, const var& n);
        friend std::string           _str_(const nothing& self, const Format_Args& fmt);

        friend bool           _is_nothing_(const nothing& self);
    };

    /********************************************************************************************/
    //
    //                      'var' Class Function Default Template API.
    //
    //           The following function templates define the over-ridable functions which
    //           may be used to tailor the behavior of any object held within a 'var'.
    //           This is done using static polymorphism through an interface.  
    //
    //           Each function defined here defines the default behavior of each function
    //           which is invoked if a function is not overwritten for a defined class.
    //
    /********************************************************************************************/


    template<typename T>            /****  Type Name  ****/
    std::string _type_(const T& self);

    template<typename T>
    std::string _type_(const T& self) {
        return std::format("{}", typeid(self).name());
    }


    template<typename T>            /****  Return An Operation Code ****/
    op_code _op_call_(const T& self);

    template<typename T>
    op_code _op_call_(const T& self) {
        (void)self;
        return op_code::nothing_op;
    }


    template<typename T>            /****  Convert To A std::size_t  ****/
    std::size_t _size_type_(const T& self);

    template<typename T>
    std::size_t _size_type_(const T& self) {
        (void)self;
        return std::size_t(0);
    }


    template<typename T>            /****  Convert To A Signed 64 Bit Integer  ****/
    std::int64_t _integer_type_(const T& self);

    template<typename T>
    std::int64_t _integer_type_(const T& self) {
        (void)self;
        return std::int64_t(0);
    }


    template<typename T>            /****  Boolean Conversion  ****/
    bool _is_(const T& self);

    template<typename T>
    bool _is_(const T& self) {
        (void)self;
        return false;
    }


    template<typename T>            /****  String Conversion  ****/
    std::string _str_(const T& self, const Format_Args& fmt);

    template<typename T>
    std::string _str_(const T& self, const Format_Args& fmt) {
        (void)fmt;
        return std::format("{} data type, const cast to access.", typeid(self).name());
    }


    template<typename T>            /****  Comparison Between Variables  ****/
    order _comp_(const T& self, const var& n);

    template<typename T>
    order _comp_(const T& self, const var& n) {
        (void)self;
        (void)n;
        return order::unordered;
    }


    template<typename T>            /****  Is Defined?  ****/
    bool _is_nothing_(const T& self);

    template<typename T>
    bool _is_nothing_(const T& self) {
        (void)self;
        return false;
    }


    template<typename T>            /****  Is A Function?  ****/
    bool _is_function_(T& self);

    template<typename T>
    bool _is_function_(T& self) {
        (void)self;
        return false;
    }


    template<typename T>            /****  Logical Conjunction  ****/
    var _and_(T& self, var& n);

    template<typename T>
    var _and_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  Logical Inclusive Disjunction  ****/
    var _or_(T& self, var& n);

    template<typename T>
    var _or_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  Logical Exclusive Disjunction  ****/
    var _xor_(T& self, var& n);

    template<typename T>
    var _xor_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  Unary Addition  ****/
    var _u_add_(T& self);

    template<typename T>
    var _u_add_(T& self) {
        return var(self);
    }


    template<typename T>            /****  Unary Negation  ****/
    var _neg_(T& self);

    template<typename T>
    var _neg_(T& self) {
        (void)self;
        return var();
    }


    template<typename T>            /****  Addition or Concatenation  ****/
    var _add_(T& self, var& n);

    template<typename T>
    var _add_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  Subtraction ****/
    var _sub_(T& self, var& n);

    template<typename T>
    var _sub_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  Multiplication ****/
    var _mul_(T& self, var& n);

    template<typename T>
    var _mul_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  Division  ****/
    var _div_(T& self, var& n);

    template<typename T>
    var _div_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  Modulation  ****/
    var _mod_(T& self, var& n);

    template<typename T>
    var _mod_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  To Power Of  ****/
    var _pow_(T& self, var& n);

    template<typename T>
    var _pow_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  To Root Of  ****/
    var _root_(T& self, var& n);

    template<typename T>
    var _root_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  Real Value  ****/
    var _real_(T& self);

    template<typename T>
    var _real_(T& self) {
        (void)self;
        return var();
    }


    template<typename T>            /****  Imaginary Value  ****/
    var _imag_(T& self);

    template<typename T>
    var _imag_(T& self) {
        (void)self;
        return var();
    }


    template<typename T>            /****  Absolute Value  ****/
    var _abs_(T& self);

    template<typename T>
    var _abs_(T& self) {
        (void)self;
        return var();
    }


    template<typename T>            /****  Lead Element Of  ****/
    var _lead_(T& self);

    template<typename T>
    var _lead_(T& self) {
        auto fa = Format_Args{};
        return var(error("Invalid operation on type: " + _type_(self) + "value: " + _str_(self, fa)));
    }


    template<typename T>            /**** Prepend Lead Element Of  ****/
    var _push_(T& self, var& n);

    template<typename T>
    var _push_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  Drop The Leading Element  ****/
    var _drop_(T& self);

    template<typename T>
    var _drop_(T& self) {
        (void)self;
        return var();
    }


    template<typename T>            /****  Get And Drop The Leading Element  ****/
    var _shift_(T& self);

    template<typename T>
    var _shift_(T& self) {
        (void)self;
        return var();
    }


    template<typename T>            /****  Reverse The Elements Of  ****/
    var _reverse_(T& self);

    template<typename T>
    var _reverse_(T& self) {
        (void)self;
        return var();
    }


    template<typename T>            /****  Get Object Index  ****/
    var _get_(T& self, var& n);

    template<typename T>
    var _get_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  Set Object Index  ****/
    var _set_(T& self, var& i, var& n);

    template<typename T>
    var _set_(T& self, var& i, var& n) {
        (void)self;
        (void)i;
        (void)n;
        return var();
    }


    template<typename T>            /****  Delete Object Index  ****/
    var _del_(T& self, var& n);

    template<typename T>
    var _del_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }


    template<typename T>            /****  Does Object Have Index  ****/
    var _has_(T& self, var& n);

    template<typename T>
    var _has_(T& self, var& n) {
        (void)self;
        (void)n;
        return var();
    }

    /********************************************************************************************/
    //
    //                               'nothing' Class Implementation
    //
    /********************************************************************************************/

    std::string _type_(const nothing& self) {
        (void)self;
        return "nothing"s;
    }

    bool _is_(const nothing& self) {
        (void)self;
        return false;
    }

    order _comp_(const nothing& self, const var& n) {
        (void)self;
        (void)n;
        return order::unordered;
    }

    std::string _str_(const nothing& self, const Format_Args& fmt) {
        (void)self;
        (void)fmt;
        return "nothing"s;
    }

    bool _is_nothing_(const nothing& self) {
        (void)self;
        return true;
    }

    /********************************************************************************************/
    //
    //                                'var' Class Implementation
    //
    /********************************************************************************************/

    var::var() : _self(std::make_unique<data_type<nothing>>(nothing())) {
    }

    var::~var() noexcept {
    }

    template <typename T>
    var::var(T x) : _self(std::make_unique<data_type<T>>(std::move(x))) {
        if (type() == "std::nullptr_t") {
            _self = std::make_unique<data_type<nothing>>(nothing());
        }
    }

    template <typename T>
    var::var(T* x) : _self(std::make_unique<data_type<T>>(*x)) {
        if (type() == "std::nullptr_t") {
            _self = std::make_unique<data_type<nothing>>(nothing());
        }
    }

    var::var(const var& other) : _self(other.is_something() ? other._self->clone() : std::make_unique<data_type<nothing>>(nothing())) {
        //auto temp = std::move(other);
        //other = std::move(*this);
        //*this = std::move(temp);
    }

    var::var(var&& other) noexcept : _self(std::move(other._self)) {
        other._self = std::make_unique<data_type<nothing>>(nothing());  // TODO:  Double check if this is needed.  I don't think so by C++ guidlines indicated it was.  
    }

    var& var::operator=(const var& other) {
        if (this != &other) {
            _self = other._self->clone();
        }
        return *this;
    }

    var& var::operator=(var&& other) noexcept {
        if (this != &other) {
            _self = std::move(other._self);
            other._self = std::make_unique<data_type<nothing>>(nothing());  // TODO:  Double check if this is needed.  I don't think so by C++ guidlines indicated it was.  
        }
        return *this;
    }

    template<typename T>
    std::unique_ptr<T> var::move() {
        if (_self) {
            data_type<T>* p = dynamic_cast<typename std::unique_ptr<data_type<T>>::element_type*>(_self.release());

            if (p) {
                auto result = std::make_unique<T>(std::move(p->_data));
                check_is_initialized();
                return result;
            }
        }
        return nullptr;
    }

    template <typename T>
    const T* var::cast() const {
        if (_self) {
            const auto p = dynamic_cast<data_type<T>*>(_self.get());

            if (p) {
                return std::addressof(p->_data);
            }
        }
        return nullptr;
    }

    template <typename T>
    std::unique_ptr<T> var::copy() const {

        const data_type<T>* p = dynamic_cast<typename std::unique_ptr<data_type<T>>::element_type*>(_self.get());

        if (p) {
            return std::make_unique<T>(p->_data);
        }
        return nullptr;
    }

    std::string var::type() const {
        return _self ? _self->_type() : "nothing"s;
    }

    op_code var::op_call() const {
        return _self ? _self->_op_call() : op_code::nothing_op;
    }

    std::size_t var::size_type() const {
        return _self ? _self->_size_type() : std::size_t{ 0 };
    }

    std::int64_t var::integer_type() const {
        return _self ? _self->_integer_type() : int64_t{ 0 };
    }

    constexpr std::string var::str(const Format_Args& fmt) const {
        return _self != nullptr ? _self->_str(fmt) : "nothing"s;
    }

    var::operator bool() const {
        return _self ? _self->_is() : false;
    }

    bool var::is_nothing() const {
        return _self ? _self->_is_nothing() : true;
    }

    bool var::is_something() const {
        return _self ? !_self->_is_nothing() : false;
    }

    bool var::is_function() const {
        return _self ? _self->_is_function() : false;
    }

    bool var::operator==(const var& n) const {
        return _self ? _self->_comp(n) == order::equivalent: false;
    }

    order var::operator<=>(const var& n) const {
        return _self ? _self->_comp(n) : order::unordered;
    }

    var var::operator&(var& n) {
        check_is_initialized();
        return _self->_and(n);
    }

    var var::operator|(var& n) {
        check_is_initialized();
        return _self->_or(n);
    }

    var var::operator^(var& n) {
        check_is_initialized();
        return _self->_xor(n);
    }

    var var::operator+() {
        check_is_initialized();
        return _self->_u_add();
    }

    var var::operator-() {
        check_is_initialized();
        return _self->_neg();
    }

    var var::operator+(var& n) {
        check_is_initialized();
        return _self->_add(n);
    }

    var var::operator-(var& n) {
        check_is_initialized();
        return _self->_sub(n);
    }

    var var::operator*(var& n) {
        check_is_initialized();
        return _self->_mul(n);
    }

    var var::operator/(var& n) {
        check_is_initialized();
        return _self->_div(n);
    }

    var var::operator%(var& n) {
        check_is_initialized();
        return _self->_mod(n);
    }

    var var::pow(var& n) {
        check_is_initialized();
        return _self->_pow(n);
    }

    var var::root(var& n) {
        check_is_initialized();
        return _self->_root(n);
    }

    var var::real() {
        check_is_initialized();
        return _self->_real();
    }

    var var::imag() {
        check_is_initialized();
        return _self->_imag();
    }

    var var::abs() {
        check_is_initialized();
        return _self->_abs();
    }

    var var::lead() {
        check_is_initialized();
        return _self->_lead();
    }

    var var::push(var& n) {
        check_is_initialized();
        return _self->_push(n);
    }

    var var::shift() {
        check_is_initialized();
        return _self->_shift();
    }

    var var::drop() {
        check_is_initialized();
        return _self->_drop();
    }

    var var::reverse() {
        check_is_initialized();
        return _self->_reverse();
    }

    var var::get(var& n) {
        check_is_initialized();
        return _self->_get(n);
    }

    var var::set(var& i, var& n) {
        check_is_initialized();
        return _self->_set(i, n);
    }

    var var::del(var& n) {
        check_is_initialized();
        return _self->_del(n);
    }

    var var::has(var& n) {
        check_is_initialized();
        return _self->_has(n);
    }

     constexpr void var::check_is_initialized() {
        if (not _self) {
            _self = std::make_unique<data_type<nothing>>(nothing());
        }
    }

    /********************************************************************************************/
    //
    //                                'data_type' Class Implementation
    //
    /********************************************************************************************/

    template <typename T>
    var::data_type<T>::data_type(T val) : _data(std::move(val)) {
    }

    template <typename T>
    op_code var::data_type<T>::_op_call() const {
        return _op_call_(_data);
    }

    template <typename T>
    std::size_t var::data_type<T>::_size_type() const {
        return _size_type_(_data);
    }

    template <typename T>
    std::int64_t var::data_type<T>::_integer_type() const {
        return _integer_type_(_data);
    }

    template <typename T>
    var::data_type<T>::operator bool() const {
        return _is();
    }

    template <typename T>
    std::string var::data_type<T>::_type() const {
        return _type_(_data);
    }

    template <typename T>
    bool var::data_type<T>::_is() const {
        return _is_(_data);
    }

    template<typename T>
    std::string var::data_type<T>::_str(const Format_Args& fmt) const {
        return _str_(_data, fmt);
    }

    template <typename T>
    order var::data_type<T>::_comp(const var& n) const {
        return _comp_(_data, n);
    }

    template <typename T>
    bool var::data_type<T>::_is_nothing() const {
        return _is_nothing_(_data);
    }

    template <typename T>
    bool var::data_type<T>::_is_function() const {
        return _is_function_(_data);
    }

    template <typename T>
    var var::data_type<T>::_and(var& n) {
        return _and_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_or(var& n) {
        return _or_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_xor(var& n) {
        return _xor_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_neg() {
        return _neg_(_data);
    }

    template <typename T>
    var var::data_type<T>::_u_add() {
        return _u_add_(_data);
    }

    template <typename T>
    var var::data_type<T>::_add(var& n) {
        return _add_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_sub(var& n) {
        return _sub_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_mul(var& n) {
        return _mul_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_div(var& n) {
        return _div_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_mod(var& n) {
        return _mod_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_pow(var& n) {
        return _pow_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_root(var& n) {
        return _root_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_real() {
        return _real_(_data);
    }

    template <typename T>
    var var::data_type<T>::_imag() {
        return _imag_(_data);
    }

    template <typename T>
    var var::data_type<T>::_abs() {
        return _abs_(_data);
    }

    template<typename T>
    var var::data_type<T>::_lead() {
        return _lead_(_data);
    }

    template <typename T>
    var var::data_type<T>::_push(var& n) {
        return _push_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_shift() {
        return _shift_(_data);
    }

    template <typename T>
    var var::data_type<T>::_drop() {
        return _drop_(_data);
    }

    template <typename T>
    var var::data_type<T>::_reverse() {
        return _reverse_(_data);
    }

    template <typename T>
    var var::data_type<T>::_get(var& n) {
        return _get_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_set(var& i, var& n) {
        return _set_(_data, i, n);
    }

    template <typename T>
    var var::data_type<T>::_del(var& n) {
        return _del_(_data, n);
    }

    template <typename T>
    var var::data_type<T>::_has(var& n) {
        return _has_(_data, n);
    }

    template<typename T>
    std::unique_ptr<var::interface_type> var::data_type<T>::clone() const {
        return std::make_unique<data_type<T>>(_data);
    }
}

/********************************************************************************************/
//
//                               fmt - Format Implementations
//
/********************************************************************************************/

template <>
struct std::formatter<Oliver::var> : std::formatter<std::string_view> {

    Oliver::Format_Args fmt_args;  // Used to capture the parsed format arguments.

    constexpr auto parse(std::format_parse_context& ctx) {
        return Oliver::parse_fmt_args(ctx, fmt_args);
    }

    auto format(const Oliver::var& a, std::format_context& ctx) const {

        std::string result = a.str(fmt_args);

        return std::format_to(ctx.out(), "{}", result);
    }
};


