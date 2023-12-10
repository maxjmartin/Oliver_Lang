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
//        along with this program.If not, see < https://www.gnu.org/licenses/>.
//    
//        The author can be reached at: maxjmartin@gmail.com
//
/*****************************************************************************************/

#include <compare>
#include <memory>
#include <string>
#include <utility>

#include "text_support.h"
#include "OpCodes.h"

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
    //          The fundamental structure of the 'var' data type was inspired and extended
    //          from a presentation entitled:
    //                 Title: Value Semantics and Concept-based Polymorphism
    //                 By - Sean Parent
    //                 (http://sean-parent.stlab.cc/papers-and-presentations)
    //
    /********************************************************************************************/

    using order = std::partial_ordering;

    const enum class op_code;

    class var {
        struct interface_type;
        template<typename T> struct data_type;

    public:

        var();
        template <typename T> var(T  x);

        var(const var& other);
        var(var&& other) noexcept;
        var& operator=(const var& other);
        var& operator=(var&& other) noexcept;

        template<typename T> const T*                 cast()  const;  // Get a const pointer reference.
        template<typename T> std::unique_ptr<T>       copy()  const;  // Get a unique pointer copy of the var data.
        template<typename T> std::unique_ptr<T>       move()       ;  // Transfer ownership of the pointer.

        constexpr std::string    str(const Format_Args& fmt)  const;  // String representation of the object, with FMT.
        std::string             type()                        const;  // The class generated type name.
        op_code              op_call()                        const;  // Get the operater code from the operator class.
        std::size_t        size_type()                        const;  // Convert the object to a size_type.
        std::int64_t    integer_type()                        const;  // Conver the object to a 64 bit integer.

        operator bool()                                       const;  // Get the boolean conversion of the object.

        bool        is_nothing()                              const;
        bool      is_something()                              const;

        bool       operator ==(const var& n)                  const;
        order      operator<=>(const var& n)                  const;

        var          operator&(var n)                              ;
        var          operator|(var n)                              ;
        var          operator^(var n)                              ;
        var          operator~()                                   ;

        var          operator+()                                   ;
        var          operator-()                                   ;

        var          operator+(var n)                              ;
        var          operator-(var n)                              ;
        var          operator*(var n)                              ;
        var          operator/(var n)                              ;
        var          operator%(var n)                              ;

        var                pow(var n)                              ;  // Raise to the power of.
        var               root(var n)                              ;  // Reduce to the root of.
        var               real()                                   ;  // The real value of a number
        var               imag()                                   ;  // The imaginary value of a number.
        var                abs()                                   ;  // Absolute value of an object.

        var               lead()                                   ;  // Lead element of an object.
        var               push(var n)                              ;  // Place an object as the lead element.
        var               next()                                   ;  // Remove the lead element from an object.
        var            reverse()                                   ;  // Reverse the order of an object's elements.

        //  TODO = Add a slice operator!

        //var&         operator[](var& n)                 ;
        //var&         operator[](var& n)            const;
        //// TODO: once at C++ 23
        //// var         operator[](var index, var value)  const;

        //var         operator>>(std::size_t shift)     const;
        //var         operator<<(std::size_t shift)     const;

        //var                get(var key)               const;  // Get an element from a collection.
        //var                set(var key, var val)      const;  // Set the value of an element in a collection.
        //var                del(var key)               const;  // Delete an element from a collection.

        //std::string              help()                      const;  // Define a string description of the object.

        //// var operator()(var a, var b, var c, var d, var e, var f) const;
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

            virtual ~interface_type() {};

            virtual std::unique_ptr<interface_type> clone()         const = 0;

            virtual operator bool()                                 const = 0;

            virtual std::string     _type()                         const = 0;
            virtual bool            _is()                           const = 0;
            virtual std::string     _str(const Format_Args& fmt)    const = 0;
            virtual std::size_t     _size_type()                    const = 0;
            virtual std::int64_t    _integer_type()                 const = 0;

            virtual order         _comp(const var& n)               const = 0;

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
            virtual var             _next()                               = 0;
            virtual var             _reverse()                            = 0;

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
            virtual ~data_type();

            operator bool()                                 const;
            std::string     _type()                         const;
            std::size_t     _size_type()                    const;
            std::int64_t    _integer_type()                 const;

            bool            _is()                           const;
            std::string     _str(const Format_Args& fmt)    const;

            order         _comp(const var& n)               const;

            var             _and(var& n)                         ;
            var             _or(var& n)                          ;
            var             _xor(var& n)                         ;
            var             _neg()                               ;

            var             _u_add()                             ;

            var             _add(var& n)                         ;
            var             _sub(var& n)                         ;
            var             _mul(var& n)                         ;
            var             _div(var& n)                         ;
            var             _mod(var& n)                         ;

            var             _pow(var& n)                         ;
            var             _root(var& n)                        ;
            var             _real()                              ;
            var             _imag()                              ;
            var             _abs()                               ;

            var             _lead()                              ;
            var             _push(var& n)                        ;
            var             _next()                              ;
            var             _reverse()                           ;

            op_code         _op_call()                      const;

            T _data;

            std::unique_ptr<interface_type> clone() const;
        };

        std::unique_ptr<interface_type> _self;

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
        friend order                _comp_(const nothing& self, var& n);
        friend std::string           _str_(const nothing& self, const Format_Args& fmt);
    };

    /********************************************************************************************/
    //
    //                      'var' Class Function Default Template API.
    //
    //           The following function templates define the over-ridable functions which
    //           may be used to tailor the behavior of any object held within a 'var'.
    //
    //           Each function defined here defines the default behavior of each function
    //           which is invoked if a function is not overwritten for a defined class.
    //
    /********************************************************************************************/


    template<typename T>            /****  Self Refrence Return  ****/
    var value_of(T& self);

    template<typename T>
    inline var value_of(T& self) {
        return var(std::move(self));
    }


    template<typename T>            /****  Type Name  ****/
    std::string _type_(const T& self);

    template<typename T>
    inline std::string _type_(const T& self) {
        return fmt::format(fmt::runtime("Invalid type = \{ {} \}"), typeid(self).name());
    }


    template<typename T>            /****  Return An Operation Code ****/
    op_code _op_call_(const T& self);

    template<typename T>
    inline op_code _op_call_(const T& self) {
        return op_code::nothing_op;
    }


    template<typename T>            /****  Convert To A std::size_t  ****/
    std::size_t _size_type_(const T& self);

    template<typename T>
    inline std::size_t _size_type_(const T& self) {
        return 0ull;
    }


    template<typename T>            /****  Convert To A Signed 64 Bit Integer  ****/
    std::int64_t _integer_type_(const T& self);

    template<typename T>
    inline std::int64_t _integer_type_(const T& self) {
        return 0ll;
    }


    template<typename T>            /****  Boolean Conversion  ****/
    bool _is_(const T& self);

    template<typename T>
    inline bool _is_(const T& self) {
        return false;
    }


    template<typename T>            /****  String Conversion  ****/
    std::string _str_(const T& self, const Format_Args& fmt);

    template<typename T>
    inline std::string _str_(const T& self, const Format_Args& fmt) {
        return ""s;
    }


    template<typename T>            /****  Comparison Between Variables  ****/
    order _comp_(const T& self, const var& n);

    template<typename T>
    inline order _comp_(const T& self, const var& n) {
        return order::unordered;
    }


    template<typename T>            /****  Logical Conjunction  ****/
    var _and_(T& self, var& n);

    template<typename T>
    inline var _and_(T& self, var& n) {
        return var();
    }


    template<typename T>            /****  Logical Inclusive Disjunction  ****/
    var _or_(T& self, var& n);

    template<typename T>
    inline var _or_(T& self, var& n) {
        return var();
    }


    template<typename T>            /****  Logical Exclusive Disjunction  ****/
    var _xor_(T& self, var& n);

    template<typename T>
    inline var _xor_(T& self, var& n) {
        return var();
    }


    template<typename T>            /****  Unary Addition  ****/
    var _u_add_(T& self);

    template<typename T>
    inline var _u_add_(T& self) {
        return value_of(self);
    }


    template<typename T>            /****  Unary Negation  ****/
    var _neg_(T& self);

    template<typename T>
    inline var _neg_(T& self) {
        return value_of(self);
    }


    template<typename T>            /****  Addition or Concatenation  ****/
    var _add_(T& self, var& n);

    template<typename T>
    inline var _add_(T& self, var& n) {
        return var();
    }


    template<typename T>            /****  Subtraction ****/
    var _sub_(T& self, var& n);

    template<typename T>
    inline var _sub_(T& self, var& n) {
        return var();
    }


    template<typename T>            /****  Multiplication ****/
    var _mul_(T& self, var& n);

    template<typename T>
    inline var _mul_(T& self, var& n) {
        return var();
    }


    template<typename T>            /****  Division  ****/
    var _div_(T& self, var& n);

    template<typename T>
    inline var _div_(T& self, var& n) {
        return var();
    }


    template<typename T>            /****  Modulation  ****/
    var _mod_(T& self, var& n);

    template<typename T>
    inline var _mod_(T& self, var& n) {
        return var();
    }


    template<typename T>            /****  To Power Of  ****/
    var _pow_(T& self, var& n);

    template<typename T>
    inline var _pow_(T& self, var& n) {
        return var();
    }


    template<typename T>            /****  To Root Of  ****/
    var _root_(T& self, var& n);

    template<typename T>
    inline var _root_(T& self, var& n) {
        return var();
    }


    template<typename T>            /****  Real Value  ****/
    var _real_(T& self);

    template<typename T>
    inline var _real_(T& self) {
        return var();
    }


    template<typename T>            /****  Imaginary Value  ****/
    var _imag_(T& self);

    template<typename T>
    inline var _imag_(T& self) {
        return var();
    }


    template<typename T>            /****  Absolute Value  ****/
    var _abs_(T& self);

    template<typename T>
    inline var _abs_(T& self) {
        return var();
    }


    template<typename T>            /****  Lead Element Of  ****/
    var _lead_(T& self);

    template<typename T>
    inline var _lead_(T& self) {
        return var();
    }


    template<typename T>            /**** Prepend Lead Element Of  ****/
    var _push_(T& self, var& n);

    template<typename T>
    inline var _push_(T& self, var& n) {
        return var();
    }


    template<typename T>            /****  Drop The Leading Element  ****/
    var _next_(T& self);

    template<typename T>
    inline var _next_(T& self) {
        return var();
    }


    template<typename T>            /****  Reverse The Elements Of  ****/
    var _reverse_(T& self);

    template<typename T>
    inline var _reverse_(T& self) {
        return var();
    }

    /********************************************************************************************/
    //
    //                               'nothing' Class Implementation
    //
    /********************************************************************************************/

    inline std::string _type_(const nothing& self) {
        return "nothing"s;
    }

    inline bool _is_(const nothing& self) {
        return false;
    }

    inline order _comp_(const nothing& self, var& n) {
        return order::unordered;
    }

    inline std::string _str_(const nothing& self, const Format_Args& fmt) {
        return "nothing"s;
    }

    /********************************************************************************************/
    //
    //                                'var' Class Implementation
    //
    /********************************************************************************************/

    inline var::var() : _self(std::make_unique<data_type<nothing>>(nothing())) {
    }

    template <typename T>
    inline var::var(T x) : _self(std::make_unique<data_type<T>>(x)) {
    }

    inline var::var(const var& other) : _self(other._self->clone()) {
    }

    inline var::var(var&& other) noexcept : _self(std::move(other._self)) {
    }

    inline var& var::operator=(const var& other) {
        if (this != &other) {
            *this = var(other);
        }
        return *this;
    }

    inline var& var::operator=(var&& other) noexcept {
        if (this != &other) {
            _self = std::move(other._self);
        }
        return *this;
    }

    template<typename T>
    inline std::unique_ptr<T> var::move() {

        data_type<T>* p = dynamic_cast<typename std::unique_ptr<data_type<T>>::element_type*>(_self.release());

        if (p) {
            return std::make_unique<T>(p->_data);
        }
        return nullptr;
    }

    template <typename T>
    inline const T* var::cast() const {
        const auto p = dynamic_cast<data_type<T>*>(_self.get());

        if (p) {
            return std::addressof(p->_data);
        }
        return nullptr;
    }

    template <typename T>
    inline std::unique_ptr<T> var::copy() const {

        const data_type<T>* p = dynamic_cast<typename std::unique_ptr<data_type<T>>::element_type*>(_self.get());

        if (p) {
            return std::make_unique<T>(p->_data);
        }
        return nullptr;
    }

    inline std::string var::type() const {
        return _self->_type();
    }

    inline op_code var::op_call() const {
        return _self->_op_call();
    }

    inline std::size_t var::size_type() const {
        return _self->_size_type();
    }

    inline std::int64_t var::integer_type() const {
        return _self->_integer_type();
    }

    inline constexpr std::string var::str(const Format_Args& fmt) const {
        return _self->_str(fmt);
    }

    inline var::operator bool() const {
        return _self->_is();
    }

    inline bool var::is_nothing() const {
        return _self->_type() == "nothing"s;
    }

    inline bool var::is_something() const {
        return _self->_type() != "nothing"s;
    }

    inline bool var::operator==(const var& n) const {
        return _self->_comp(n) == order::equivalent;
    }

    inline order var::operator<=>(const var& n) const {
        return _self->_comp(n);
    }

    inline var var::operator&(var n) {
        return _self->_and(n);
    }

    inline var var::operator|(var n) {
        return _self->_or(n);
    }

    inline var var::operator^(var n) {
        return _self->_xor(n);
    }

    inline var var::operator+() {
        return _self->_u_add();
    }

    inline var var::operator-() {
        return _self->_neg();
    }

    inline var var::operator+(var n) {
        return _self->_add(n);
    }

    inline var var::operator-(var n) {
        return _self->_sub(n);
    }

    inline var var::operator*(var n) {
        return _self->_mul(n);
    }

    inline var var::operator/(var n) {
        return _self->_div(n);
    }

    inline var var::operator%(var n) {
        return _self->_mod(n);
    }

    inline var var::pow(var n) {
        return _self->_pow(n);
    }

    inline var var::root(var n) {
        return _self->_root(n);
    }

    inline var var::real() {
        return _self->_real();
    }

    inline var var::imag() {
        return _self->_imag();
    }

    inline var var::abs() {
        return _self->_abs();
    }

    inline var var::lead() {
        return _self->_lead();
    }

    inline var var::push(var n) {
        return _self->_push(n);
    }

    inline var var::next() {
        return _self->_next();
    }

    inline var var::reverse() {
        return _self->_reverse();
    }

    /********************************************************************************************/
    //
    //                                'data_type' Class Implementation
    //
    /********************************************************************************************/

    template <typename T>
    inline var::data_type<T>::data_type(T val) : _data(std::move(val)) {
    }

    template<typename T>
    inline var::data_type<T>::~data_type() {
    }

    template <typename T>
    inline op_code var::data_type<T>::_op_call() const {
        return _op_call_(_data);
    }

    template <typename T>
    inline std::size_t var::data_type<T>::_size_type() const {
        return _size_type_(_data);
    }

    template <typename T>
    inline std::int64_t var::data_type<T>::_integer_type() const {
        return _integer_type_(_data);
    }

    template <typename T>
    inline var::data_type<T>::operator bool() const {
        return _is();
    }

    template <typename T>
    inline std::string var::data_type<T>::_type() const {
        return _type_(_data);
    }

    template <typename T>
    inline bool var::data_type<T>::_is() const {
        return _is_(_data);
    }

    template<typename T>
    inline std::string var::data_type<T>::_str(const Format_Args& fmt) const {
        return _str_(_data, fmt);
    }

    template <typename T>
    inline order var::data_type<T>::_comp(const var& n) const {
        return _comp_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_and(var& n) {
        return _and_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_or(var& n) {
        return _or_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_xor(var& n) {
        return _xor_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_neg() {
        return _neg_(_data);
    }

    template <typename T>
    inline var var::data_type<T>::_u_add() {
        return _u_add_(_data);
    }

    template <typename T>
    inline var var::data_type<T>::_add(var& n) {
        return _add_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_sub(var& n) {
        return _sub_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_mul(var& n) {
        return _mul_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_div(var& n) {
        return _div_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_mod(var& n) {
        return _mod_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_pow(var& n) {
        return _pow_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_root(var& n) {
        return _root_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_real() {
        return _real_(_data);
    }

    template <typename T>
    inline var var::data_type<T>::_imag() {
        return _imag_(_data);
    }

    template <typename T>
    inline var var::data_type<T>::_abs() {
        return _abs_(_data);
    }

    template<typename T>
    inline var var::data_type<T>::_lead() {
        return _lead_(_data);
    }

    template <typename T>
    inline var var::data_type<T>::_push(var& n) {
        return _push_(_data, n);
    }

    template <typename T>
    inline var var::data_type<T>::_next() {
        return _next_(_data);
    }

    template <typename T>
    inline var var::data_type<T>::_reverse() {
        return _reverse_(_data);
    }

    template<typename T>
    inline std::unique_ptr<var::interface_type> var::data_type<T>::clone() const {
        return std::make_unique<data_type<T>>(_data);
    }
}

/********************************************************************************************/
//
//                               fmt - Format Implementations
//
/********************************************************************************************/

template <>
struct fmt::formatter<Oliver::var> : fmt::formatter<std::string> {

    Oliver::Format_Args fmt_args;  // Used to capture the parsed format arguments.

    constexpr auto parse(fmt::format_parse_context& ctx) {
        return parse_fmt_args(ctx, fmt_args);
    }

    auto format(const Oliver::var& a, fmt::format_context& ctx) const {

        std::string result = a.str(fmt_args);

        return fmt::format_to(ctx.out(), "{}", result);
    }
};