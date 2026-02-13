 ```C++   
    template<typename VALUE = intmax_t>
    class SeqVector : public std::ranges::view_interface<SeqVector<VALUE>> {

    public:
        using impl_type              = std::vector<VALUE>;
        using value_type             = typename std::vector<VALUE>::value_type;
        using size_type              = typename std::vector<VALUE>::size_type;
        using iterator               = typename std::vector<VALUE>::iterator;
        using const_iterator         = typename std::vector<VALUE>::const_iterator;
        using reverse_iterator       = typename std::vector<VALUE>::reverse_iterator;
        using const_reverse_iterator = typename std::vector<VALUE>::const_reverse_iterator;

        constexpr SeqVector() noexcept;
        constexpr SeqVector(value_type value);
        constexpr SeqVector(const std::valarray<value_type>& val);
        constexpr SeqVector(std::initializer_list<value_type> list);

        template<typename LE, typename Op, typename RE>
        constexpr SeqVector(ExprTemplate<LE, Op, RE>&& expr);

                 ~SeqVector()                                noexcept;
        constexpr SeqVector(SeqVector&& arr)                 noexcept;
        constexpr SeqVector(const SeqVector& arr)            noexcept;
        constexpr SeqVector& operator=(SeqVector&& arr)      noexcept;
        constexpr SeqVector& operator=(const SeqVector& arr) noexcept;

        friend void swap(SeqVector<VALUE>& first, SeqVector<VALUE>& second);

        operator  bool()                                                const;
        constexpr bool operator==(const SeqVector& b)                   const;
        constexpr std::partial_ordering operator<=>(const SeqVector& b) const;

        constexpr const value_type  operator[](std::size_t index) const;
        constexpr       value_type& operator[](std::size_t index);

        auto begin()         noexcept;
        auto begin()   const noexcept;
        auto cbegin()  const noexcept;
        auto end()           noexcept;
        auto end()     const noexcept;
        auto cend()    const noexcept;
        auto rbegin()        noexcept;
        auto rbegin()  const noexcept;
        auto crbegin() const noexcept;
        auto rend()          noexcept;
        auto rend()    const noexcept;
        auto crend()   const noexcept;

        constexpr auto view() noexcept;

        constexpr std::size_t size()     const;
        constexpr std::size_t max_size() const;
        constexpr std::size_t capacity() const;

        constexpr SeqVector<VALUE>& clear  ();
        constexpr SeqVector<VALUE>& resize (std::size_t size);
        constexpr SeqVector<VALUE>& resize (std::size_t size, value_type value);
        constexpr SeqVector<VALUE>& reserve(std::size_t size);

        constexpr SeqVector<VALUE>& assign (      SeqVector<VALUE>&&   range);
        constexpr SeqVector<VALUE>& assign (const SeqVector<VALUE>&    range);
        constexpr SeqVector<VALUE>& assign (      std::vector<VALUE>&& range, std::size_t width);
        constexpr SeqVector<VALUE>& assign (const std::vector<VALUE>&  range, std::size_t width);
        constexpr SeqVector<VALUE>& assign (      SeqVector<VALUE>&&   range);
        constexpr SeqVector<VALUE>& assign (const SeqVector<VALUE>&    range);
        constexpr SeqVector<VALUE>& assign (      std::vector<VALUE>&& range, std::size_t width);
        constexpr SeqVector<VALUE>& assign (const std::vector<VALUE>&  range, std::size_t width);
        constexpr SeqVector<VALUE>& assign (std::size_t width, VALUE value);

        constexpr SeqVector<VALUE>& pop_back ();
        constexpr SeqVector<VALUE>& push_back(value_type value);

        constexpr SeqVector<VALUE>& insert(std::size_t at, const value_type& range);
        constexpr SeqVector<VALUE>& insert(std::size_t at, SeqVector<VALUE>&& range);

        constexpr SeqVector<VALUE>& shift (int index);
        constexpr SeqVector<VALUE>& cshift(int index);

        constexpr SeqVector<VALUE>& apply(value_type func(value_type));
        constexpr SeqVector<VALUE>& apply(value_type func(const value_type&));
        constexpr SeqVector<VALUE>& apply(const SeqVector<VALUE>& b, value_type func(value_type, value_type));
        constexpr SeqVector<VALUE>& apply(const SeqVector<VALUE>& b, value_type func(const value_type&, value_type));
        constexpr SeqVector<VALUE>& apply(const SeqVector<VALUE>& b, value_type func(value_type, const value_type&));
        constexpr SeqVector<VALUE>& apply(const SeqVector<VALUE>& b, value_type func(const value_type&, const value_type&));

        constexpr SeqVector<VALUE>& operator++();
        constexpr SeqVector<VALUE>  operator++(int);
        constexpr SeqVector<VALUE>& operator--();
        constexpr SeqVector<VALUE>  operator--(int);

        constexpr SeqVector<VALUE> operator+();
        constexpr SeqVector<VALUE> operator-();
        constexpr SeqVector<VALUE> operator~();

        constexpr SeqVector<VALUE>& operator+=(const SeqVector& b);
        constexpr SeqVector<VALUE>& operator-=(const SeqVector& b);
        constexpr SeqVector<VALUE>& operator*=(const SeqVector& b);
        constexpr SeqVector<VALUE>& operator/=(const SeqVector& b);
        constexpr SeqVector<VALUE>& operator%=(const SeqVector& b);
        constexpr SeqVector<VALUE>& operator&=(const SeqVector& b);
        constexpr SeqVector<VALUE>& operator|=(const SeqVector& b);
        constexpr SeqVector<VALUE>& operator^=(const SeqVector& b);
        constexpr SeqVector<VALUE>& operator<<=(const SeqVector& b);
        constexpr SeqVector<VALUE>& operator>>=(const SeqVector& b);

        template<typename RightExpr>
        SeqVector<VALUE>& operator=(RightExpr&& re);
        template<typename RightExpr>
        SeqVector<VALUE>& operator+=(RightExpr&& re);
        template<typename RightExpr>
        SeqVector<VALUE>& operator-=(RightExpr&& re);
        template<typename RightExpr>
        SeqVector<VALUE>& operator*=(RightExpr&& re);
        template<typename RightExpr>
        SeqVector<VALUE>& operator/=(RightExpr&& re);
        template<typename RightExpr>
        SeqVector<VALUE>& operator%=(RightExpr&& re);
        template<typename RightExpr>
        SeqVector<VALUE>& operator&=(RightExpr&& re);
        template<typename RightExpr>
        SeqVector<VALUE>& operator|=(RightExpr&& re);
        template<typename RightExpr>
        SeqVector<VALUE>& operator^=(RightExpr&& re);
        template<typename RightExpr>
        SeqVector<VALUE>& operator<<=(RightExpr&& re);
        template<typename RightExpr>
        SeqVector<VALUE>& operator>>=(RightExpr&& re);

        template<typename RightExpr>
        auto operator+(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Add_Op<value_type>, decltype(std::forward<RightExpr>(re))>;
        template<typename RightExpr>
        auto operator-(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Sub_Op<value_type>, decltype(std::forward<RightExpr>(re))>;
        template<typename RightExpr>
        auto operator*(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Mul_Op<value_type>, decltype(std::forward<RightExpr>(re))>;
        template<typename RightExpr>
        auto operator/(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Div_Op<value_type>, decltype(std::forward<RightExpr>(re))>;
        template<typename RightExpr>
        auto operator%(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Mod_Op<value_type>, decltype(std::forward<RightExpr>(re))>;
        template<typename RightExpr>
        auto operator&(RightExpr&& re) const -> ExprTemplate<const SeqVector&, And_Op<value_type>, decltype(std::forward<RightExpr>(re))>;
        template<typename RightExpr>
        auto operator|(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Or_Op<value_type>, decltype(std::forward<RightExpr>(re))>;
        template<typename RightExpr>
        auto operator^(RightExpr&& re) const -> ExprTemplate<const SeqVector&, Xor_Op<value_type>, decltype(std::forward<RightExpr>(re))>;
        template<typename RightExpr>
        auto operator<<(RightExpr&& re) const -> ExprTemplate<const SeqVector&, LeftShift_Op<value_type>, decltype(std::forward<RightExpr>(re))>;
        template<typename RightExpr>
        auto operator>>(RightExpr&& re) const -> ExprTemplate<const SeqVector&, RightShift_Op<value_type>, decltype(std::forward<RightExpr>(re))>;

        constexpr SeqVector<VALUE>& abs();
        constexpr SeqVector<VALUE>  abs(SeqVector<VALUE> a);

        constexpr value_type sum() const;
        constexpr value_type sum(const SeqVector<VALUE>& a);

        constexpr value_type max_val() const;
        constexpr value_type max_val(const SeqVector<VALUE>& a) noexcept;

        constexpr value_type min_val() const;
        constexpr value_type min_val(const SeqVector<VALUE>& a) noexcept;

        constexpr SeqVector<VALUE>& exp();
        constexpr SeqVector<VALUE>  exp(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& log();
        constexpr SeqVector<VALUE>  log(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& log10();
        constexpr SeqVector<VALUE>  log10(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& pow(const value_type& exp);
        constexpr SeqVector<VALUE>  pow(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& sqrt();
        constexpr SeqVector<VALUE>  sqrt(SeqVector<VALUE> a);

        constexpr SeqVector<VALUE>& sin();
        constexpr SeqVector<VALUE>  sin(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& cos();
        constexpr SeqVector<VALUE>  cos(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& tan();
        constexpr SeqVector<VALUE>  tan(SeqVector<VALUE> a);

        constexpr SeqVector<VALUE>& asin();
        constexpr SeqVector<VALUE>  asin(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& acos();
        constexpr SeqVector<VALUE>  acos(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& atan();
        constexpr SeqVector<VALUE>  atan(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& atan2();
        constexpr SeqVector<VALUE>  atan2(SeqVector<VALUE> a);

        constexpr SeqVector<VALUE>& sinh();
        constexpr SeqVector<VALUE>  sinh(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& cosh();
        constexpr SeqVector<VALUE>  cosh(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& tanh();
        constexpr SeqVector<VALUE>  tanh(SeqVector<VALUE> a);

        constexpr SeqVector<VALUE>& asinh();
        constexpr SeqVector<VALUE>  asinh(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& acosh();
        constexpr SeqVector<VALUE>  acosh(SeqVector<VALUE> a);
        constexpr SeqVector<VALUE>& atanh();
        constexpr SeqVector<VALUE>  atanh(SeqVector<VALUE> a);
    };
```
