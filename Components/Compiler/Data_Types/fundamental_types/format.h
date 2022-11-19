#ifndef FORMAT_H
#define FORMAT_H

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

#include "./number.h"

namespace Olly {


    /********************************************************************************************/
    //
    //                               'format' Class Definition
    //
    //        The format class provides a wrapper around normal C++ formats.  Why
    //        re-invent the wheel?
    //
    /********************************************************************************************/

    static const enum class FORMAT_CODE {
        nothing_op = 0,
        bool_alpha_op, bool_numeric_op, left_op, right_op, internal_op,
        dec_op, oct_op, hex_op, scientific_op, fixed_op, def_float_op, hex_float_op,
        show_base_op, no_base_op, show_positive_op, no_positive_op, show_point_op, no_point_op,
        skip_ws_op, keep_ws_op, flush_buf_op, no_flush_buf_op, base_upper_op, base_lower_op,
        get_money_op, get_time_op, put_money_op, put_time_op, 
        set_fill_op, set_prec_op, set_width_op, 
        reset_op
    };

    static const std::map<str_type, FORMAT_CODE> FORMAT_OPERATORS = {
        { "bool_alpha",       FORMAT_CODE::bool_alpha_op       },    
        { "bool_numeric",     FORMAT_CODE::bool_numeric_op     },
        { "left",             FORMAT_CODE::left_op             },
        { "right",            FORMAT_CODE::right_op            },
        { "internal",         FORMAT_CODE::internal_op         },
        { "dec",              FORMAT_CODE::dec_op              },
        { "oct",              FORMAT_CODE::oct_op              },
        { "hex",              FORMAT_CODE::hex_op              },
        { "scientific",       FORMAT_CODE::scientific_op       },
        { "fixed",            FORMAT_CODE::fixed_op            },
        { "show_base",        FORMAT_CODE::show_base_op        },
        { "no_base",          FORMAT_CODE::no_base_op          },
        { "def_float",        FORMAT_CODE::def_float_op        },
        { "hex_float",        FORMAT_CODE::hex_float_op        },
        { "show_base",        FORMAT_CODE::show_base_op        },
        { "no_base",          FORMAT_CODE::no_base_op          },
        { "show_positive",    FORMAT_CODE::show_positive_op    },
        { "no_positive",      FORMAT_CODE::no_positive_op      },
        { "show_point",       FORMAT_CODE::show_point_op       },
        { "no_point",         FORMAT_CODE::no_point_op         },
        { "skip_ws",          FORMAT_CODE::skip_ws_op          },
        { "keep_ws",          FORMAT_CODE::keep_ws_op          },
        { "flush_buf",        FORMAT_CODE::flush_buf_op        },
        { "no_flush_buf",     FORMAT_CODE::no_flush_buf_op     },
        { "upper_case",       FORMAT_CODE::base_upper_op       },
        { "lower_case",       FORMAT_CODE::base_lower_op       },
        { "get_money",        FORMAT_CODE::get_money_op        },
        { "get_time",         FORMAT_CODE::get_time_op         },
        { "put_money",        FORMAT_CODE::put_money_op        },
        { "put_time",         FORMAT_CODE::put_time_op         },
        { "set_fill",         FORMAT_CODE::set_fill_op         },
        { "set_prec",         FORMAT_CODE::set_prec_op         },
        { "set_width",        FORMAT_CODE::set_width_op        },
        { "reset",            FORMAT_CODE::reset_op            },

        // get_money_op, get_time_op, put_money_op, put_time_op,
        // set_base_op, set_fill_op, set_prec_op, set_width_op,
        // reset_op
    };

    class format {

        tokens_type _format_codes;

    public:

        format();
        format(str_type str);
        virtual ~format();

        format(format&& n)                  = default;
        format(const format& n)             = default;
        format& operator= (format&& n)      = default;
        format& operator= (const format& n) = default;

        friend str_type    _type_(const format& self);
        friend bool          _is_(const format& self);
        friend real_type   _comp_(const format& self, const let& other);
        friend void         _str_(stream_type& out, const format& self);
        friend void        _repr_(stream_type& out, const format& self);

        friend int_type    _size_(const format& self);

    private:

        FORMAT_CODE get_format_code(str_type str) const;
    };

    format::format() : _format_codes() {
    }

    format::format(str_type str) : _format_codes(split(to_lower(str), "")) {
    }

    format::~format() {
    }

    inline FORMAT_CODE format::get_format_code(str_type code_str) const {
        
        auto it = FORMAT_OPERATORS.find(code_str);

        if (it != FORMAT_OPERATORS.end()) {

            return it->second;
        }

        return FORMAT_CODE::nothing_op;
    }

    str_type _type_(const format& self) {
        return "format";
    }

    bool _is_(const format& self) {
        return !self._format_codes.empty();
    }

    real_type _comp_(const format& self, const let& other) {

        const format* s = other.cast<format>();

        if (s) {

            if (self._format_codes == s->_format_codes) {
                return 0.0;
            }
        }
        return NOT_A_NUMBER;
    }

    void _str_(stream_type& out, const format& self) {
        
        for (auto code = self._format_codes.cbegin(); code != self._format_codes.cend(); ++code) {
            
            switch (self.get_format_code(*code)) {

                case FORMAT_CODE::bool_alpha_op: {

                    out << std::boolalpha;
                }   break;


                case FORMAT_CODE::bool_numeric_op: {

                    out << std::noboolalpha;
                }   break;

                case FORMAT_CODE::left_op: {

                    out << std::left;
                }   break;

                case FORMAT_CODE::right_op: {

                    out << std::right;
                }   break;

                case FORMAT_CODE::internal_op: {

                    out << std::internal;
                }   break;

                case FORMAT_CODE::dec_op: {

                    out << std::dec;
                }   break;

                case FORMAT_CODE::oct_op: {

                    out << std::oct;
                }   break;

                case FORMAT_CODE::hex_op: {

                    out << std::hex;
                }   break;

                case FORMAT_CODE::scientific_op: {

                    out << std::scientific;
                }   break;

                case FORMAT_CODE::fixed_op: {

                    out << std::fixed;
                }   break;

                case FORMAT_CODE::def_float_op: {

                    out << std::defaultfloat;
                }   break;

                case FORMAT_CODE::hex_float_op: {

                    out << std::hexfloat;
                }   break;

                case FORMAT_CODE::show_base_op: {

                    out << std::showbase;
                }   break;

                case FORMAT_CODE::no_base_op: {

                    out << std::noshowbase;
                }   break;

                case FORMAT_CODE::show_positive_op: {

                    out << std::showpos;
                }   break;

                case FORMAT_CODE::no_positive_op: {

                    out << std::noshowpos;
                }   break;

                case FORMAT_CODE::show_point_op: {

                    out << std::showpoint;
                }   break;

                case FORMAT_CODE::no_point_op: {

                    out << std::noshowpoint;
                }   break;

                case FORMAT_CODE::skip_ws_op: {

                    out << std::skipws;
                }   break;

                case FORMAT_CODE::keep_ws_op: {

                    out << std::noskipws;
                }   break;

                case FORMAT_CODE::flush_buf_op: {

                    out << std::unitbuf;
                }   break;

                case FORMAT_CODE::no_flush_buf_op: {

                    out << std::nounitbuf;
                }   break;

                case FORMAT_CODE::base_upper_op: {

                    out << std::uppercase;
                }   break;

                case FORMAT_CODE::base_lower_op: {

                    out << std::nouppercase;
                }   break;

                case FORMAT_CODE::set_fill_op: {

                    if (code != self._format_codes.cend()) {
                        ++code;

                        if (code->size() == 1) {

                            out.fill(*code->c_str());
                        }
                    }
                }   break;

                case FORMAT_CODE::set_prec_op: {

                    if (code != self._format_codes.cend()) {
                        ++code;

                        int_type n = number(*code).integer();

                        out.precision(n);
                    }
                }   break;

                case FORMAT_CODE::set_width_op: {

                    if (code != self._format_codes.cend()) {
                        ++code;

                        int_type n = number(*code).integer();

                        out.width(n);
                    }
                }   break;

                case FORMAT_CODE::reset_op: {

                    out = stream_type();
                    out << std::boolalpha;
                }   break;
            }
        }
    }

    void _repr_(stream_type& out, const format& self) {

        out << "`";

        for (str_type code : self._format_codes) {

            out << code << " ";
        }
        out.seekp(-1, out.cur);

        out << "`";
    }

    int_type _size_(const format& self) {
        return (int_type)self._format_codes.size();
    }

}
#endif // FORMAT_H
