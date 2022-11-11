#ifndef MAP_H
#define MAP_H

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

#include <concepts>
#include <iterator>

#include "../let.h"
#include "./error.h"
#include "./expression.h"
#include "./support_types/integer_value.h"

namespace Olly {

    /********************************************************************************************/
    //
    //                               'map' Class Definition
    //
    //          The map class is implimented using Lisp inspired data maps.  It
    //          is used to define the data maps as in Lisp.  
    //
    /********************************************************************************************/

    class map {

        let _node;

    public:

        map();
        map(let exp_pairs);
        map(let pair, let left, let right);
        virtual ~map();

        map(map&& n)                  = default;
        map(const map& n)             = default;
        map& operator= (map&& n)      = default;
        map& operator= (const map& n) = default;

        friend str_type           _type_(const map& self);
        friend bool_type            _is_(const map& self);
        friend real_type          _comp_(const map& self, const let& other);

        friend void                _str_(stream_type& out, const map& self);
        friend void               _repr_(stream_type& out, const map& self);

        friend size_type          _size_(const map& self);
        friend bool_type           _has_(const map& self, const let& key);
        friend let                 _get_(const map& self, const let& key);
        friend let                 _set_(const map& self, const let& key, const let& val);
        friend let                 _del_(const map& self, const let& key);
        friend let               _clear_(const map& self);

        friend let             _get_pair_(const map& self);
        friend let       _to_expression_(const map& self);

        friend let                 _add_(const map& self, const let& other);

    private:
        typedef std::vector<bool_type> direction_queue;
        typedef std::vector<let>       buffer_queue;

        int_type  get_height(let node) const;
        int_type get_balance(let node) const;
        let      set_balance(let node) const;

        size_type  get_size()         const;
        size_type  get_size(let node) const;

        let  get_list()         const;
        let  get_list(let node) const;

        let  get_key(let node) const;
        let  get_val(let node) const;

        let set_branch(let node, let key, let value) const;
        let  set_value(let node, let key, let value) const;

        let del_branch(let node, let key) const;
        let  del_value(let node, let key) const;

        let       left_rotation(let node) const;
        let      right_rotation(let node) const;

        let define_node(let pair, let left, let right) const;
    };

    /********************************************************************************************/
    //
    //                                 'map' Class Implimentation
    //
    /********************************************************************************************/

    inline map::map() : _node(expression()) {
    }

    inline map::map(let exp_pairs) : _node(expression()) {

        while (exp_pairs.is()) {

            let key = pop_lead(exp_pairs);
            let val = pop_lead(exp_pairs);
            let opr = pop_lead(exp_pairs);

            if (opr.op_code() == OP_CODE::EQ_op) {

                _node = set_value(_node, key, val);
            }
        }
    }

    inline map::map(let pair, let left, let right) : _node(expression()) {

        _node = define_node(pair, left, right);

        _node = set_balance(_node);
    }

    inline Olly::map::~map() {
    }

    str_type Olly::_type_(const map& self) {
        return "map";
    }

    bool_type _is_(const map& self) {
        return self._node.is();
    }

    real_type _comp_(const map& self, const let& other)  {
        
        const map* map_ptr = other.cast<map>();

        if (map_ptr) {

            return self.get_list().comp(map_ptr->get_list());
        }

        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const map& self) {

        if (!_is_(self)) {
            out << "{}";
            return;
        }

        out << "{";

        let l = self.get_list();

        while (l.is()) {

            let p = pop_lead(l);

            let k = pop_lead(p);
            let v = pop_lead(p);

            k.str(out);

            out << " = ";

            v.str(out);

            out << ", ";
        }

        out.seekp(-2, out.cur);
        out << "}";
    }

    void _repr_(stream_type& out, const map& self) {

        if (!_is_(self)) {
            out << "{}";
            return;
        }

        out << "{";

        let l = self.get_list();

        while (l.is()) {

            let p = pop_lead(l);

            let k = pop_lead(p);
            let v = pop_lead(p);

            k.repr(out);

            out << " = ";

            v.repr(out);

            out << ", ";
        }

        out.seekp(-2, out.cur);
        out << "}";
    }

    size_type _size_(const map& self) {
        return self.get_size();
    }

    bool_type _has_(const map& self, const let& key) {

        let n = self;

        while (n.is_something()) {

            let pair = n.get_pair();

            if (first(pair) == key) {
                return true;
            }

            if (key < first(pair)) {

                n = second(n);
            }
            else {
                n = third(n);
            }
        }

        return false;
    }

    let _get_(const map& self, const let& key) {

        let n = self;

        while (n.is_something()) {

            let pair = n.get_pair();

            if (first(pair) == key) {
                return second(pair);
            }

            if (key < first(pair)) {

                n = second(n);
            }
            else {
                n = third(n);
            }
        }

        return nothing();
    }

    let _set_(const map& self, const let& key, const let& val) {
        
        if (!_is_(self)) {
            return map(make_pair(key, val), expression(), expression());
        }

        map n = self;

        n._node = n.set_value(n._node, key, val);

        return n;
    }

    let _del_(const map& self, const let& key) {
        
        if (!_is_(self)) {
            return self;
        }

        map n = self;

        n._node = n.del_value(n._node, key);

        return n;
    }

    let _clear_(const map& self) {
        return map();
    }

    let _get_pair_(const map& self) {
        return first(self._node);
    }

    let _to_expression_(const map& self) {
        return self.get_list();
    }

    let _add_(const map& self, const let& other) {

        const map* ptr = other.cast<map>();

        if (ptr) {

            let args = ptr->get_list();
            map node = self;

            while (args.is()) {

                let p = pop_lead(args);

                node._node = node.set_value(node._node, first(p), second(p));
            }

            return node;
        }

        return nothing();
    }

    inline int_type map::get_height(let node) const {
        return (node.is() ? fourth(node).get_integer() : -1);
    }

    inline int_type map::get_balance(let node) const {
        return get_height(second(node)) - get_height(third(node));
    }

    inline let map::set_balance(let node) const {
        
        int_type bf = get_balance(_node);

        if (bf > 1) {

            int_type left_bf = get_balance(second(node));

            if (left_bf >= 0) {

                node = right_rotation(node);
            }
            else {

                let left = left_rotation(second(node));

                node = define_node(first(node), left, third(node));

                node = right_rotation(node);
            }
        }
        else if (bf < -1) {

            int_type right_bf = get_balance(second(node));

            if (right_bf >= 0) {

                let right = right_rotation(third(node));

                node = define_node(first(node), second(node), right);

                node = left_rotation(node);
            }
            else {

                node = left_rotation(node);
            }
        }

        return node;
    }

    inline size_type map::get_size() const {
        return get_size(_node);
    }

    inline size_type map::get_size(let node) const {

        let buffer = expression();

        let n = node;

        int_type size = 0;

        while (n.is() || buffer.is()) {

            while (n.is()) {  // Loop through all left side branches.

                buffer = buffer.join(n);

                n = second(n);
            }

            n = pop_lead(buffer);

            // For other effects place manipulation code here.

            size += 1;

            // End code manipulation.

            n = third(n);  // Get any right side branches that may exist.
        }

        return size;
    }

    let map::get_list() const {
        return get_list(_node);
    }

    let map::get_list(let node) const {

        let buffer = expression();

        let result = expression();

        let n = node;

        while (n.is() || buffer.is()) {

            while (n.is()) {  // Loop through all left side branches.

                buffer = buffer.join(n);

                n = second(n);
            }

            n = pop_lead(buffer);

            // For other effects place manipulation code here.

            result = result.join(n.lead());

            // End code manipulation.

            n = third(n);  // Get any right side branches that may exist.
        }

        return result;
    }

    inline let Olly::map::get_key(let node) const {
        return first(first(node));
    }

    inline let Olly::map::get_val(let node) const {
        return second(first(node));
    }

    let map::set_branch(let node, let key, let value) const {

        if (!node.is()) {
            return map(make_pair(key, value), expression(), expression())._node;
        }

        if (key.type() != get_key(node).type()) {

            return node;
        }

        let pair = first(node);

        if (first(pair) == key) {

            return map(make_pair(key, value), second(node), third(node))._node;
        }

        return nothing();
    }

    let map::set_value(let node, let key, let value) const {

        let new_node = set_branch(node, key, value);

        if (new_node.is_nothing()) {

            auto left_right = std::vector<bool_type>();
            auto queue = std::vector<let>();

            while (new_node.is_nothing()) {

                queue.emplace_back(node);

                let pair = first(node);

                if (first(pair) < key) {  // left

                    left_right.emplace_back(true);

                    node = second(node);

                    new_node = set_branch(node, key, value);
                }
                else if (first(pair) > key) {  // right

                    left_right.emplace_back(false);

                    node = third(node);

                    new_node = set_branch(node, key, value);
                }
            }

            auto j = left_right.crbegin();

            for (auto i = queue.crbegin(); i != queue.crend(); ++i, ++j) {

                if (*j) {
                    new_node = map(first(*i), new_node, third(*i))._node;
                }
                else {
                    new_node = map(first(*i), second(*i), new_node)._node;
                }
            }
        }

        return new_node;
    }

    let map::del_branch(let node, let key) const {

        if (!node.is()) {
            return node;
        }

        let pair = first(node);

        if (first(pair) == key) {

            let left = second(node);
            let right = third(node);

            map l_node;
                l_node._node = define_node(first(left), second(left), third(left));

            map r_node;
                r_node._node = define_node(first(right), second(right), third(right));

            let args = l_node.get_list().add(r_node.get_list());

            node = expression();

            while (args.is()) {

                let p = pop_lead(args);

                node = set_value(node, first(p), second(p));
            }

            return node;
        }

        return nothing();
    }

    let map::del_value(let node, let key) const {

        let new_node = del_branch(node, key);

        if (new_node.is_nothing()) {

            auto left_right = std::vector<bool_type>();
            auto queue      = std::vector<let>();

            while (new_node.is_nothing()) {

                queue.emplace_back(node);

                let pair = first(node);

                if (first(pair) < key) {  // left

                    left_right.emplace_back(true);

                    node = second(node);

                    new_node = del_branch(node, key);
                }
                else if (first(pair) > key) {  // right

                    left_right.emplace_back(false);

                    node = third(node);

                    new_node = del_branch(node, key);
                }
            }

            auto j = left_right.crbegin();

            for (auto i = queue.crbegin(); i != queue.crend(); ++i, ++j) {

                if (*j) {
                    new_node = map(first(*i), new_node, third(*i))._node;
                }
                else {
                    new_node = map(first(*i), second(*i), new_node)._node;
                }
            }
        }

        return new_node;
    }

    inline let map::right_rotation(let node) const {
    
        let a = second(node);
        a = define_node(first(a), second(a), third(a));

        let b = third(a);

        let c = define_node(first(node), b, third(node));

        a = define_node(first(a), second(a), c);

        return a;
    }

    inline let map::left_rotation(let node) const {

        let a = third(node);
        a = define_node(first(a), second(a), third(a));

        let b = second(a);

        let c = define_node(first(node), second(node), b);

        a = define_node(first(a), c, third(a));

        return a;
    }

    inline let map::define_node(let pair, let left, let right) const {
        
        int_type lh = get_height(left);
        int_type rh = get_height(right);

        integer_type height = (lh > rh ? lh : rh) + 1;

        let node = expression();

        if (third(pair).is_nothing() && pair.is_type(_node)) {

            node = node.join(height);
            node = node.join(right);
            node = node.join(left);
            node = node.join(pair);
        }

        return node;
    }
}
#endif // MAP_H