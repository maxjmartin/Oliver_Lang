
#include <iostream>

#include <vector>
#include <string>
#include <iostream>
#include <algorithm>
#include <stdexcept>
#include "app/app.h"

#include <boost/multiprecision/cpp_dec_float.hpp>
#include <complex>


    bool almost_equal(const Oliver::MPA::number& n,
                      const std::complex<boost::multiprecision::cpp_dec_float_50>& bc) {
        using fp = boost::multiprecision::cpp_dec_float_50;
        fp n_real(n.real().to_string());
        fp n_imag(n.imag().to_string());
        fp dr = abs(n_real - bc.real());
        fp di = abs(n_imag - bc.imag());
        // relative tolerance with an absolute floor
        const fp rel = fp("1e-45");   // ~5e-45 relative (tune as needed)
        const fp absf = fp("1e-60");  // very small floor to handle near-zero
        const fp thr_r = max(absf, rel * max(abs(n_real), abs(bc.real())));
        const fp thr_i = max(absf, rel * max(abs(n_imag), abs(bc.imag())));
        return (dr <= thr_r) && (di <= thr_i);
    }

    void print_error(const std::string& operation, 
        const Oliver::MPA::number& n, 
        const std::complex<boost::multiprecision::cpp_dec_float_50>& bc,
        const std::size_t bits
    ) {
        fmt::println("{} failed:\n MPA_real {}\n BST_real {}\n MPA_imag {}\n BST_imag {}\n at {} bits", 
            operation, 
            n.real().to_string(), 
            bc.real().str(), 
            n.imag().to_string(),
            bc.imag().str(),
            bits);
    }
    
    bool complex_tests() {
        fmt::println("Running rigorous complex number tests...");
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Test parameters
        const std::size_t num_iterations = 1000;
        bool all_tests_passed = true;
        std::size_t total_operations = 0;
        
        // Rigorous test cases
        std::vector<std::pair<std::string, std::string>> test_cases = {
            {"1.414213562373095048801688724209698078569671875376948073176679737990732478462107038850387534327641573", 
             "3.141592653589793238462643383279502884197169399375105820974944592307816406286208998628034825342117068"},
            {"-2.718281828459045235360287471352662497757247093699959574966967627724076630353547594571382178525166427", 
             "1.732050807568877293527446341505872366942805253810380628055806979451820864027756639754190885162593329"},
            {"123456789.987654321123456789987654321123456789987654321123456789987654321", 
             "-987654321.123456789987654321123456789987654321123456789987654321123456789"},
            {"1e-50", "1e-50"},
            {"1e50", "-1e50"}
        };
        
        Oliver::MPA::math_env env_local;
        env_local.rounding_mode("half_even");
        env_local.decimal_scale(100); // High precision
        
        for (std::size_t iter = 0; iter < num_iterations; ++iter) {
            for (std::size_t i = 0; i < test_cases.size(); ++i) {
                for (std::size_t j = i + 1; j < test_cases.size(); ++j) {
                    // Create test numbers
                    Oliver::MPA::number a(test_cases[i].first, test_cases[i].second);
                    Oliver::MPA::number b(test_cases[j].first, test_cases[j].second);
                    
                    // Create boost reference numbers for timing comparison
                    std::complex<boost::multiprecision::cpp_dec_float_50> ba{
                        boost::multiprecision::cpp_dec_float_50(test_cases[i].first),
                        boost::multiprecision::cpp_dec_float_50(test_cases[i].second)
                    };
                    std::complex<boost::multiprecision::cpp_dec_float_50> bb{
                        boost::multiprecision::cpp_dec_float_50(test_cases[j].first),
                        boost::multiprecision::cpp_dec_float_50(test_cases[j].second)
                    };
                    
                    // Test addition with timing
                    auto mpa_start = std::chrono::high_resolution_clock::now();
                    Oliver::MPA::number sum = a + b;
                    auto mpa_end = std::chrono::high_resolution_clock::now();
                    auto mpa_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(mpa_end - mpa_start);
                    
                    auto boost_start = std::chrono::high_resolution_clock::now();
                    std::complex<boost::multiprecision::cpp_dec_float_50> bsum = ba + bb;
                    auto boost_end = std::chrono::high_resolution_clock::now();
                    auto boost_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(boost_end - boost_start);
                    
                    fmt::println("Addition: MPA={}ns, Boost={}ns, Result: {}", 
                               mpa_duration.count(), boost_duration.count(), sum.to_string());
                    total_operations++;
                    
                    // Test subtraction with timing
                    mpa_start = std::chrono::high_resolution_clock::now();
                    Oliver::MPA::number diff = a - b;
                    mpa_end = std::chrono::high_resolution_clock::now();
                    mpa_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(mpa_end - mpa_start);
                    
                    boost_start = std::chrono::high_resolution_clock::now();
                    std::complex<boost::multiprecision::cpp_dec_float_50> bdiff = ba - bb;
                    boost_end = std::chrono::high_resolution_clock::now();
                    boost_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(boost_end - boost_start);
                    
                    fmt::println("Subtraction: MPA={}ns, Boost={}ns, Result: {}", 
                               mpa_duration.count(), boost_duration.count(), diff.to_string());
                    total_operations++;
                    
                    // Test multiplication with timing
                    mpa_start = std::chrono::high_resolution_clock::now();
                    Oliver::MPA::number prod = a * b;
                    mpa_end = std::chrono::high_resolution_clock::now();
                    mpa_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(mpa_end - mpa_start);
                    
                    boost_start = std::chrono::high_resolution_clock::now();
                    std::complex<boost::multiprecision::cpp_dec_float_50> bprod = ba * bb;
                    boost_end = std::chrono::high_resolution_clock::now();
                    boost_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(boost_end - boost_start);
                    
                    fmt::println("Multiplication: MPA={}ns, Boost={}ns, Result: {}", 
                               mpa_duration.count(), boost_duration.count(), prod.to_string());
                    total_operations++;
                    
                    // Test division with timing (avoid division by zero)
                    if (bb != std::complex<boost::multiprecision::cpp_dec_float_50>(0, 0)) {
                        mpa_start = std::chrono::high_resolution_clock::now();
                        Oliver::MPA::number quot = a / b;
                        mpa_end = std::chrono::high_resolution_clock::now();
                        mpa_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(mpa_end - mpa_start);
                        
                        boost_start = std::chrono::high_resolution_clock::now();
                        std::complex<boost::multiprecision::cpp_dec_float_50> bquot = ba / bb;
                        boost_end = std::chrono::high_resolution_clock::now();
                        boost_duration = std::chrono::duration_cast<std::chrono::nanoseconds>(boost_end - boost_start);
                        
                        fmt::println("Division: MPA={}ns, Boost={}ns, Result: {}", 
                                   mpa_duration.count(), boost_duration.count(), quot.to_string());
                        total_operations++;
                    }
                }
            }
        }
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        fmt::println("Complex number tests completed!");
        fmt::println("Completed {} operations in {} ms", total_operations, duration.count());
        fmt::println("Average time per operation: {:.3f} microseconds", 
                    (double)duration.count() * 1000.0 / total_operations);
        
        return all_tests_passed;
    }

int main(int argc, char **argv) {

    (void)argc;
    (void)argv;

    using namespace Oliver;
    using namespace std;

    set_current_locale();

    Oliver::MPA::math_env env;
    env.rounding_mode("none");
    env.decimal_scale(50); // Set precision to 50 decimal places

    auto start = std::chrono::high_resolution_clock::now();

    //complex_tests();
    Oliver::MPA::integer_tests();
    // Oliver::MPA::decimal_tests();

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> duration = end - start;

    std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

    // Oliver::MPA::whole_number_tests();
    // numbers_tests::test_whole_number_all();
    // Oliver::MPA::integer_tests();

    return 0;
}
