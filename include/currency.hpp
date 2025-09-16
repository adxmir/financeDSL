#ifndef CURRENCY_H
#define CURRENCY_H

#include <string_view>
#include <type_traits>
#include <array>
#include <vector>
#include <ostream>
#include <iomanip>
#include <concepts>


namespace Code{

    namespace Body{ 

        template< std::array<char, 3> currcode>
        struct currency{
            static constexpr auto code = currcode;
        };
        
        constexpr std::array<char, 3> GBP_currcode{'G', 'B', 'P'};
        constexpr std::array<char, 3> USD_currcode{'U', 'S', 'D'};
        constexpr std::array<char, 3> EUR_currcode{'E', 'U', 'R'};
        constexpr std::array<char, 3> JPY_currcode{'J', 'P', 'Y'};
        constexpr std::array<char,3> LKR_currcode{'L','K','R'};
        constexpr std::array<char,3> AUD_currcode{'A','U','D'};
        constexpr std::array<char,3> CAD_currcode{'C','A','D'};
        constexpr std::array<char,3> CHF_currcode{'C','H','F'};
        constexpr std::array<char,3> INR_currcode{'I','N','R'};


        using GBP = currency<GBP_currcode>;
        using USD = currency<USD_currcode>;
        using EUR = currency<EUR_currcode>;
        using JPY = currency<JPY_currcode>;
        using LKR = currency<LKR_currcode>;
        using AUD = currency<AUD_currcode>;
        using CAD = currency<CAD_currcode>;
        using CHF = currency<CHF_currcode>;
        using INR = currency<INR_currcode>;

        template<typename FROM, typename TO>
        struct fixed_conversion;

        //General GBP converisons with large currencies
        template<> struct fixed_conversion<GBP,USD>{ static constexpr double rate = 0.91;};
        template<> struct fixed_conversion<USD,GBP>{ static constexpr double rate = 1.1;};
        template<> struct fixed_conversion<GBP,EUR>{ static constexpr double rate = 1.17;};
        template<> struct fixed_conversion<EUR,GBP>{ static constexpr double rate = 0.855;};
        template<> struct fixed_conversion<GBP,JPY>{ static constexpr double rate = 190.0;};
        template<> struct fixed_conversion<JPY,GBP>{ static constexpr double rate = 0.00526;};
        template<> struct fixed_conversion<JPY, LKR> {static constexpr double rate = 2.05;};
        //USD conversions 
        template<> struct fixed_conversion<USD, AUD> {static constexpr double rate = 1.5;};
        template<> struct fixed_conversion<AUD, USD> {static constexpr double rate = 0.67;};
        template<> struct fixed_conversion<JPY, USD> {static constexpr double rate =  0.0068;};
        template<> struct fixed_conversion<USD, JPY> {static constexpr double rate = 147.02 ;};
        template<> struct fixed_conversion<USD, CAD> {static constexpr double rate = 1.38;};
        template<> struct fixed_conversion<CAD,USD> {static constexpr double rate = 0.73;};
        template<> struct fixed_conversion<USD , CHF> {static constexpr double rate = 0.79;};
        template<> struct fixed_conversion<CHF,USD> { static constexpr double rate = 1.26;};
        template<> struct fixed_conversion<USD, INR> {static constexpr double rate = 88.06;};
        template<> struct fixed_conversion<INR, USD> { static constexpr double rate = 0.011;};


        template<typename FROM, typename TO, typename = void>
        struct is_compatible : std::false_type{};

        template<typename FROM, typename TO>
        struct is_compatible<FROM, TO , std::void_t<decltype(fixed_conversion<FROM, TO>::rate)>>  : std::true_type{};

        template<typename FROM, typename TO> 
        inline constexpr bool is_compatible_v = is_compatible<FROM,TO>::value;


        template<typename FROM, typename TO>
        struct is_same : std::false_type{};

        template<typename FROM>
        struct is_same<FROM, FROM> : std::true_type{};
    
        template<typename FROM, typename TO>
        inline constexpr bool is_same_v = is_same<FROM,TO>::value;

        template<typename... Ts>
        struct type_list {};

        using Currencies = type_list<GBP,USD,JPY,EUR, LKR, AUD, CAD, CHF,INR>; 

        //Valid determined by whether direction conversion exists
        template<typename FROM, typename TO, typename = void>
        struct conversion{
            static constexpr bool valid = false;
            static constexpr double rate = 0.0;
        };

        template<typename FROM,typename TO>
        struct conversion<FROM, TO , std::void_t<decltype(fixed_conversion<FROM, TO>::rate)>>{
            static constexpr bool valid = true;
            static constexpr double rate = fixed_conversion<FROM,TO>::rate;
        };

        //Conversion loop to find possible indirect conversions
        template<typename FROM, typename TO, typename list>
        struct conversion_search;

        template<typename FROM, typename TO>
        struct conversion_search<FROM, TO, type_list<>>{
            static constexpr bool valid = false;
            static constexpr double rate(){return 0;}
        };

        template<typename FROM, typename TO, typename curr, typename... next>   
        struct conversion_search<FROM,TO,type_list<curr, next...>>{
            static constexpr bool valid = (conversion<FROM, curr>::valid && conversion<curr, TO>::valid) || conversion_search<FROM, TO, type_list<next...>>::valid;
            static constexpr double rate(){ 
                //Direct conversion vound
                if(conversion<FROM,curr>::valid && conversion<curr,TO>::valid){
                    return conversion<FROM,curr>::rate * conversion<curr,TO>::rate;
                } 
                //Indirect conversion found;
                else if constexpr(conversion<FROM,curr>::valid && conversion<curr,TO>::valid){
                    return conversion<FROM,curr>::rate * conversion<curr,TO>::rate;
                }
                //Check next code in currencies 
                else{
                    return conversion_search<FROM,TO, type_list<next...>>::rate();
                }
            }
        };

        //Handle whether direct conversion is enough or if we can do indirect conversion
        template<typename FROM, typename TO>
        struct conversion_solver{
            private:
            static constexpr bool direct = conversion<FROM,TO>::valid;
            public:
            static constexpr bool valid = direct || conversion_search<FROM,TO,Currencies>::valid;
            static constexpr double rate(){
                if constexpr(direct){
                    return conversion<FROM,TO>::rate;
                }
                else if constexpr (conversion_search<FROM,TO,Currencies>::valid){
                    return conversion_search<FROM,TO,Currencies>::rate();
                }
                else{
                    return 0.0;
                    }
                }
            };
        };

    using AmountInt = unsigned long long;
    constexpr AmountInt fixedF_I = 1000000; //million
    
    template<typename T> concept number = std::integral<T> || std::floating_point<T>;

    using GBP = Body::GBP;
    using USD = Body::USD;
    using EUR = Body::EUR;
    using JPY = Body::JPY;
    using LKR = Body::LKR;
    using CAD = Body::CAD;
    using CHF = Body::CHF;
    using AUD = Body::AUD;
    using INR = Body::INR; 

    template<typename T>
    struct Money{
        using code = T;
        AmountInt amount;
        constexpr Money(double amount_) : amount(static_cast<AmountInt>(amount_* fixedF_I)){
           if(amount_ < 0){throw "Cannot have negative money";}
        };

        operator double() const {
            return static_cast<double>(amount) / fixedF_I;
        }

        template<number TYPE> constexpr Money operator/(const TYPE val){return Money<T>(amount / val);}
        template<number TYPE> constexpr Money operator *(const TYPE multiplier) const {return Money<T>(amount * multiplier,0);}
        constexpr Money operator +(const Money<T>& other) const {return Money<T>(amount + other.amount, 0);}
        constexpr Money operator-(const Money<T>& other) const {return Money<T>(amount - other.amount,0);}
        constexpr bool operator>(const Money<T>& other) const{return amount > other.amount;}
        constexpr bool operator<(const Money<T>& other) const{return amount < other.amount;}

        template<number TYPE> constexpr auto operator+(const TYPE) const = delete;
        template<number TYPE> constexpr auto operator-(const TYPE) const = delete;
        template<typename notT> constexpr Money operator-(const Money<notT>& other) const = delete;
        template<typename notT> constexpr Money operator+(const Money<notT>& other) const = delete;
        template<typename notT> constexpr bool operator>(const Money<notT>&other) = delete;
        template<typename notT> constexpr bool operator<(const Money<notT>&other) = delete;

        Money operator-() const = delete;

        private:
        constexpr Money(AmountInt amountScaled, int) : amount(amountScaled){};
        
    };

    template<typename FROM, typename TO>
    constexpr Money<TO> convert(const Money<FROM>& m){
        if constexpr (Body::is_same_v<FROM,TO>){
            return m;
        }
        else{
            static_assert(Body::conversion_solver<FROM,TO>::valid, "No conversion path available for conversion between");
            static_assert(Body::conversion_solver<FROM,TO>::rate() != 0, "Something went wrong with the conversion");
            return Money<TO> ((m.amount * Body::conversion_solver<FROM,TO>::rate()) / fixedF_I);
        }
    };

    //String format of fixed point amount without losing accuracy 
    template<typename T>
    std::ostream& operator<<(std::ostream& msg,const  Money<T>& code){
        auto after_D = code.amount / fixedF_I;
        auto before_D = code.amount % fixedF_I;

        msg << after_D << '.' <<  std::setw(6) << std::setfill('0') << before_D;
        return msg;
        }

   
    consteval Money<GBP> operator ""_GBP(unsigned long long amount){return Money<GBP>(amount);} 
    consteval Money<GBP> operator ""_GBP(long double amount){return Money<GBP>(amount);} 

    consteval Money<USD> operator ""_USD(unsigned long long amount){return Money<USD>(amount);} 
    consteval Money<USD> operator ""_USD(long double amount){return Money<USD>(amount);} 

    consteval Money<AUD> operator ""_AUD(unsigned long long amount){return Money<AUD>(amount);} 
    consteval Money<AUD> operator ""_AUD(long double amount){return Money<AUD>(amount);} 

    consteval Money<JPY> operator ""_JPY(unsigned long long amount){return Money<JPY>(amount);} 
    consteval Money<JPY> operator ""_JPY(long double amount){return Money<JPY>(amount);} 

    consteval Money<CAD> operator ""_CAD(unsigned long long amount){return Money<CAD>(amount);} 
    consteval Money<CAD> operator ""_CAD(long double amount){return Money<CAD>(amount);} 

    consteval Money<INR> operator ""_INR(unsigned long long amount){return Money<INR>(amount);} 
    consteval Money<INR> operator ""_INR(long double amount){return Money<INR>(amount);} 

    consteval Money<EUR> operator ""_EUR(unsigned long long amount){return Money<EUR>(amount);} 
    consteval Money<EUR> operator ""_EUR(long double amount){return Money<EUR>(amount);} 

    consteval Money<CHF> operator ""_CHF(unsigned long long amount){return Money<CHF>(amount);} 
    consteval Money<CHF> operator ""_CHF(long double amount){return Money<CHF>(amount);} 

    consteval Money<LKR> operator ""_LKR(unsigned long long amount){return Money<LKR>(amount);} 
    consteval Money<LKR> operator ""_LKR(long double amount){return Money<LKR>(amount);} 
}

#endif