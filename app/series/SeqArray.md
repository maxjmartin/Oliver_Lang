```C++

    template<std::size_t SIZE, typename VALUE = intmax_t>
    class SeqArray : public std::ranges::view_interface<SeqArray<SIZE, VALUE>> {
    public:
        // Type aliases
        using impl_type              = std::array<VALUE, (SIZE > 0 ? SIZE : 1)>;
        using value_type             = impl_type::value_type;
        using size_type              = impl_type::size_type;
        using iterator               = impl_type::iterator;
        using const_iterator         = impl_type::const_iterator;
        using reverse_iterator       = impl_type::reverse_iterator;
        using const_reverse_iterator = impl_type::const_reverse_iterator;

        constexpr SeqArray();
        constexpr SeqArray(value_type value);
        constexpr SeqArray(const std::valarray<value_type>& val);
        constexpr SeqArray(std::initializer_list<value_type> list);

                 ~SeqArray()                                noexcept = default;
        constexpr SeqArray(SeqArray&& arr)                  noexcept = default;
        constexpr SeqArray(const SeqArray& arr)             noexcept = default;
        constexpr SeqArray& operator =(SeqArray&& arr)      noexcept = default;
        constexpr SeqArray& operator =(const SeqArray& arr) noexcept = default;

        constexpr void swap(SeqArray<SIZE, VALUE>& first, SeqArray<SIZE, VALUE>& second);

        operator bool()                                                const;
        constexpr bool                  operator== (const SeqArray& b) const;
        constexpr std::partial_ordering operator<=>(const SeqArray& b) const;

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

        constexpr bool        constant() const;
        constexpr bool        sequence() const;
        constexpr std::size_t size()     const;
        constexpr std::size_t max_size() const;

        constexpr SeqArray<SIZE, VALUE>&  shift(int index);
        constexpr SeqArray<SIZE, VALUE>& cshift(int index);

        constexpr SeqArray<SIZE, VALUE>& apply(value_type func(value_type));
        constexpr SeqArray<SIZE, VALUE>& apply(value_type func(const value_type&));
        constexpr SeqArray<SIZE, VALUE>& apply(const SeqArray<SIZE, VALUE>& b, value_type func(value_type, value_type));
        constexpr SeqArray<SIZE, VALUE>& apply(const SeqArray<SIZE, VALUE>& b, value_type func(const value_type&, value_type));
        constexpr SeqArray<SIZE, VALUE>& apply(const SeqArray<SIZE, VALUE>& b, value_type func(value_type, const value_type&));
        constexpr SeqArray<SIZE, VALUE>& apply(const SeqArray<SIZE, VALUE>& b, value_type func(const value_type& , const value_type&));

        constexpr SeqArray<SIZE, VALUE>& operator++();
        constexpr SeqArray<SIZE, VALUE>  operator++(int);
        constexpr SeqArray<SIZE, VALUE>& operator--();
        constexpr SeqArray<SIZE, VALUE>  operator--(int);

        constexpr SeqArray<SIZE, VALUE> operator+();
        constexpr SeqArray<SIZE, VALUE> operator-();
        constexpr SeqArray<SIZE, VALUE> operator~();

        constexpr SeqArray<SIZE, VALUE>& operator+= (const SeqArray& b);
        constexpr SeqArray<SIZE, VALUE>& operator-= (const SeqArray& b);
        constexpr SeqArray<SIZE, VALUE>& operator*= (const SeqArray& b);
        constexpr SeqArray<SIZE, VALUE>& operator/= (const SeqArray& b);
        constexpr SeqArray<SIZE, VALUE>& operator%= (const SeqArray& b);
        constexpr SeqArray<SIZE, VALUE>& operator&= (const SeqArray& b);
        constexpr SeqArray<SIZE, VALUE>& operator|= (const SeqArray& b);
        constexpr SeqArray<SIZE, VALUE>& operator^= (const SeqArray& b);
        constexpr SeqArray<SIZE, VALUE>& operator<<=(const SeqArray& b);
        constexpr SeqArray<SIZE, VALUE>& operator>>=(const SeqArray& b);

        constexpr SeqArray<SIZE, VALUE>& abs();
        constexpr SeqArray<SIZE, VALUE>& exp();
        constexpr SeqArray<SIZE, VALUE>& log();
        constexpr SeqArray<SIZE, VALUE>& log10();
        constexpr SeqArray<SIZE, VALUE>& pow(const value_type& exp);
        constexpr SeqArray<SIZE, VALUE>& sqrt();
        constexpr SeqArray<SIZE, VALUE>& sin();
        constexpr SeqArray<SIZE, VALUE>& cos();
        constexpr SeqArray<SIZE, VALUE>& tan();
        constexpr SeqArray<SIZE, VALUE>& asin();
        constexpr SeqArray<SIZE, VALUE>& acos();
        constexpr SeqArray<SIZE, VALUE>& atan();
        constexpr SeqArray<SIZE, VALUE>& atan2();
        constexpr SeqArray<SIZE, VALUE>& sinh();
        constexpr SeqArray<SIZE, VALUE>& cosh();
        constexpr SeqArray<SIZE, VALUE>& tanh();
        constexpr SeqArray<SIZE, VALUE>& asinh();
        constexpr SeqArray<SIZE, VALUE>& acosh();
        constexpr SeqArray<SIZE, VALUE>& atanh();
    };

    template <typename VALUE = intmax_t>
    using scalar = SeqArray<0, VALUE>;
```
