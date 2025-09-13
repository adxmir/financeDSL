#ifndef SQL_H
#define SQL_H

#include <string_view>
#include <type_traits>
#include <array>
#include <vector>
#include <ostream>
#include <iomanip>


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

    using AmountInt = unsigned long long;
    constexpr AmountInt fixedF_I = 1000000; //million

    template<typename T>
    struct Money{
        using code = T;
        AmountInt amount;
        constexpr Money(double amount_) : amount(static_cast<AmountInt>(amount_* fixedF_I)){};
        constexpr Money(int amount_) : amount(amount_ * fixedF_I){};

        operator double() const {
            return static_cast<double>(amount) / fixedF_I;
        }

        Money operator +(const Money<T>& other) const {return Money<T>(amount + other.amount, 0);}

        template<typename notT>
        Money operator +(const Money<notT>& other) const = delete;

        private:
        constexpr Money(AmountInt amountScaled, int) : amount(amountScaled){};
        
    };

    template<typename T>
    std::ostream& operator<<(std::ostream& msg, Money<T>& code){
        auto after_D = code.amount / fixedF_I;
        auto before_D = code.amount % fixedF_I;

        msg << after_D << '.' <<  std::setw(6) << std::setfill('0') << before_D;
        return msg;
    }

    template<typename FROM, typename TO>
    constexpr Money<TO> convert(const Money<FROM>& m){
        static_assert(is_compatible_v<FROM,TO>, "No rate available for this conversion");
        return Money<TO> (m.amount * Currency::fixed_conversion<FROM,TO>::rate / fixedF_I);
    };

}



#endif