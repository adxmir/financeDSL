#ifndef SQL_H
#define SQL_H

#include <string_view>
#include <type_traits>
#include <array>
#include <vector>

namespace Code{

    namespace Currency{ 

        template< std::array<char, 3> currcode>
        struct currency{
            static constexpr auto code = currcode;
        };
        
        constexpr std::array<char, 3> GBP_currcode{'G', 'B', 'P'};
        constexpr std::array<char, 3> USD_currcode{'U', 'S', 'D'};
        constexpr std::array<char, 3> EUR_currcode{'E', 'U', 'R'};
        constexpr std::array<char, 3> JPY_currcode{'J', 'P', 'Y'};


        template<typename FROM, typename TO>
        struct fixed_conversion;

        using GBP = currency<GBP_currcode>;
        using USD = currency<USD_currcode>;
        using EUR = currency<EUR_currcode>;
        using JPY = currency<JPY_currcode>;

        template<> struct fixed_conversion<GBP,USD>{ static constexpr double rate = 0.91;};
        template<> struct fixed_conversion<USD,GBP>{ static constexpr double rate = 1.1;};
        template<> struct fixed_conversion<GBP,EUR>{ static constexpr double rate = 1.17;};
        template<> struct fixed_conversion<EUR,GBP>{ static constexpr double rate = 0.855;};
        template<> struct fixed_conversion<GBP,JPY>{ static constexpr double rate = 190.0;};
        template<> struct fixed_conversion<JPY,GBP>{ static constexpr double rate = 0.00526;};

        template<typename FROM, typename TO, typename = void>
        struct is_compatible : std::false_type{};

        template<typename FROM, typename TO>
        struct is_compatible<FROM, TO , std::void_t<decltype(fixed_conversion<FROM, TO>::rate)>>  : std::true_type{};

        template<typename FROM, typename TO> 
        inline constexpr bool is_compatible_v = is_compatible<FROM,TO>::value;

    }

    using namespace Currency;

    using AmountInt = int64_t;
    constexpr double fixedF_I = 1000000.0; //million

    template<typename T>
    struct Money{
        using code = T;
        AmountInt amount;
        constexpr Money(double amount_) : amount(static_cast<AmountInt>(amount_* fixedF_I)){};

        operator double() const {
            return static_cast<double>(amount) / fixedF_I;
        }
    };

    template<typename TO, typename FROM>
    constexpr Money<TO> convert(const Money<FROM>& m){
        static_assert(is_compatible_v<TO,FROM>);
        return Money<TO> (m.amount * Currency::fixed_conversion<FROM,TO>::rate / fixedF_I);;
    };

}



#endif