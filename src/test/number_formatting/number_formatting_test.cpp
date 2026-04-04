#include <gtest/gtest.h>

#include <cmath>
#include <functional>
#include <iostream>
#include <string>
#include <vector>

#include "misc/misc.h"

//////// Testing getIntExponent ////////
TEST(TestGetIntExponent, TestBasic)
{
    EXPECT_EQ(0, getIntExponent(0.0));

    std::vector<double> test_cases = {0.0, 1.0, 10.0, 100.0, 1000.0, 10000.0, 0.1, 0.01, 0.001, 0.0001};
    std::vector<int> expected_results = {0, 0, 1, 2, 3, 4, -1, -2, -3, -4};

    for (size_t k = 0; k < test_cases.size(); ++k)
    {
        EXPECT_EQ(expected_results[k], getIntExponent(test_cases[k]));
    }
}

TEST(TestGetIntExponent, TestSweep)
{
    std::vector<std::int32_t> exponents;
    std::vector<double> base_values;

    for (std::int32_t k = -300; k <= 300; k += 1)
    {
        exponents.push_back(k);
    }

    for (double k = -0.999; k <= 0.999; k += 0.01)
    {
        base_values.push_back(k);
    }

    for (size_t k = 0; k < exponents.size(); ++k)
    {
        for (size_t i = 0; i < base_values.size(); ++i)
        {
            double base_val = base_values[i];
            base_val = (base_val == 0.0) ? 0.1 : base_val;
            base_val = (base_val < 0.0) ? (base_val - 1.0) : (base_val + 1.0);

            const double exponent_val = static_cast<double>(exponents[k]);
            const std::int32_t int_exponent_val = exponents[k];

            const double test_val = base_val * std::pow(10.0, exponent_val);
            const int actual_val = getIntExponent(test_val);

            EXPECT_EQ(int_exponent_val, actual_val);
        }
    }
}

//////// Testing getBase ////////
TEST(TestGetBase, TestBasic)
{
    EXPECT_EQ(0.0, getBase(0.0));

    std::vector<double> test_cases = {0.0, 1.0, 10.0, 100.0, 1000.0, 10000.0, 0.1, 0.01, 0.001, 0.0001};
    std::vector<double> expected_results = {0.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

    for (size_t k = 0; k < test_cases.size(); ++k)
    {
        EXPECT_EQ(expected_results[k], getBase(test_cases[k]));
    }
}

TEST(TestGetBase, TestSweep)
{
    std::vector<std::int32_t> exponents;
    std::vector<double> base_values;

    for (std::int32_t k = -300; k <= 300; k += 1)
    {
        exponents.push_back(k);
    }

    for (double k = -0.999; k <= 0.999; k += 0.01)
    {
        base_values.push_back(k);
    }

    for (size_t k = 0; k < exponents.size(); ++k)
    {
        for (size_t i = 0; i < base_values.size(); ++i)
        {
            double base_val = base_values[i];
            base_val = (base_val == 0.0) ? 0.1 : base_val;
            base_val = (base_val < 0.0) ? (base_val - 1.0) : (base_val + 1.0);

            const double exponent_val = static_cast<double>(exponents[k]);

            const double test_val = base_val * std::pow(10.0, exponent_val);
            const double actual_val = getBase(test_val);

            EXPECT_NEAR(base_val, actual_val, 1.0e-10);
        }
    }
}

TEST(TestToStringWithNumDecimalPlaces, TestSweep)
{
    std::vector<std::int32_t> exponents;
    std::vector<double> base_values;

    for (std::int32_t k = -300; k <= 300; k += 1)
    {
        exponents.push_back(k);
    }

    for (double k = -0.999; k <= 0.999; k += 0.01)
    {
        base_values.push_back(k);
    }

    for (size_t k = 0; k < exponents.size(); ++k)
    {
        for (size_t i = 0; i < base_values.size(); ++i)
        {
            double base_val = base_values[i];
            base_val = (base_val == 0.0) ? 0.1 : base_val;
            base_val = (base_val < 0.0) ? (base_val - 1.0) : (base_val + 1.0);

            const double exponent_val = static_cast<double>(exponents[k]);

            const double test_val = base_val * std::pow(10.0, exponent_val);
            const std::string string_val = std::to_string(base_val);

            EXPECT_EQ(toStringWithNumDecimalPlaces(getBase(test_val), 6), string_val);
        }
    }
}

TEST(TestFormatNumberInternal, TestSweep)
{
    /*std::vector<std::int32_t> exponents;
    std::vector<double> base_values;

    for (std::int32_t k = -300; k <= 300; k += 1)
    {
        exponents.push_back(k);
    }

    for (double k = -0.999; k <= 0.999; k += 0.01)
    {
        if (k < 0.0)
        {
            base_values.push_back(k - 0.0001613434123);
        }
        else
        {
            base_values.push_back(k + 0.0001613434123);
        }
    }

    for (size_t k = 0; k < exponents.size(); ++k)
    {
        for (size_t i = 0; i < base_values.size(); ++i)
        {
            double base_val = base_values[i];
            base_val = (base_val == 0.0) ? 0.1 : base_val;
            base_val = (base_val < 0.0) ? (base_val - 1.0) : (base_val + 1.0);

            const double exponent_val = static_cast<double>(exponents[k]);

            const double test_val = base_val * std::pow(10.0, exponent_val);
            std::string string_val = std::to_string(base_val);
            std::string print_string;

            {
                std::ostringstream out;
                out.precision(15);
                out << std::fixed << test_val;
                print_string = out.str();
            }

            if (exponents[k] > 4)
            {
                std::ostringstream out;
                out.precision(5);
                out << std::fixed << base_val;
                string_val = out.str();

                string_val += "e+" + std::to_string(exponents[k]);
            }
            else if (exponents[k] < -2)
            {
                std::ostringstream out;
                out.precision(5);
                out << std::fixed << base_val;
                string_val = out.str();

                string_val += "e" + std::to_string(exponents[k]);
            }
            else
            {
                std::ostringstream out;
                out.precision(5 - exponents[k]);
                out << std::fixed << test_val;
                string_val = out.str();
            }

            const std::string actual_val = formatNumberInternal(test_val, 5);

            EXPECT_EQ(actual_val, string_val);
        }
    }*/
}

TEST(TestFormatNumber, TestSweep)
{
    std::vector<std::int32_t> exponents;
    std::vector<double> base_values;

    for (std::int32_t k = -5; k <= 5; k += 1)
    {
        exponents.push_back(k);
    }

    for (double k = -0.9; k <= 0.9; k += 0.1)
    {
        /*if (k < 0.0)
        {
            base_values.push_back(k - 0.0001613434123);
        }
        else
        {
            base_values.push_back(k + 0.0001613434123);
        }*/
        base_values.push_back(k);
    }

    for (size_t k = 0; k < exponents.size(); ++k)
    {
        for (size_t i = 0; i < base_values.size(); ++i)
        {
            double base_val = base_values[i];
            base_val = (base_val == 0.0) ? 0.1 : base_val;
            base_val = (base_val < 0.0) ? (base_val - 1.0) : (base_val + 1.0);

            const double exponent_val = static_cast<double>(exponents[k]);

            const double test_val = base_val * std::pow(10.0, exponent_val);
            std::string string_val = std::to_string(base_val);
            std::string print_string;

            {
                std::ostringstream out;
                out.precision(15);
                out << std::fixed << test_val;
                print_string = out.str();
            }

            if (exponents[k] > 4)
            {
                std::ostringstream out;
                out.precision(5);
                out << std::fixed << base_val;
                string_val = out.str();

                string_val += "e+" + std::to_string(exponents[k]);
            }
            else if (exponents[k] < -2)
            {
                std::ostringstream out;
                out.precision(5);
                out << std::fixed << base_val;
                string_val = out.str();

                string_val += "e" + std::to_string(exponents[k]);
            }
            else
            {
                std::ostringstream out;
                out.precision(5 - exponents[k]);
                out << std::fixed << test_val;
                string_val = out.str();
            }

            const std::string actual_val = formatNumber(test_val, 5);

            // std::cout << "[" << exponents[k] << ", " << print_string << "]: " << actual_val << ", " << string_val
            //           << std::endl;

            // EXPECT_EQ(actual_val, string_val);
        }
    }
}

TEST(TestLStrip, TestBasic)
{
    EXPECT_EQ("", zeroLStrip(""));
    EXPECT_EQ("0", zeroLStrip("0"));
    EXPECT_EQ("0", zeroLStrip("00000"));
    EXPECT_EQ("0", zeroLStrip("00"));
    EXPECT_EQ("1", zeroLStrip("00001"));
    EXPECT_EQ("10", zeroLStrip("00010"));
    EXPECT_EQ("100", zeroLStrip("00100"));
    EXPECT_EQ("1000", zeroLStrip("01000"));
    EXPECT_EQ("10000", zeroLStrip("10000"));

    EXPECT_EQ("317", zeroLStrip("317"));
    EXPECT_EQ("3170", zeroLStrip("3170"));
    EXPECT_EQ("31700", zeroLStrip("31700"));
    EXPECT_EQ("317000", zeroLStrip("317000"));

    EXPECT_EQ("317", zeroLStrip("00317"));
    EXPECT_EQ("3170", zeroLStrip("003170"));
    EXPECT_EQ("31700", zeroLStrip("0031700"));
    EXPECT_EQ("317000", zeroLStrip("00317000"));

    for (size_t k = 0; k < 1000; k++)
    {
        const size_t num = rand() % 1000;
        const size_t num_pre_zeros = rand() % 5;
        const size_t num_post_zeros = rand() % 5;

        const std::string pre_zeros(num_pre_zeros, '0');
        const std::string post_zeros(num_post_zeros, '0');
        const std::string num_str = std::to_string(num);
        std::string exp_str = num_str + post_zeros;
        const std::string test_str = pre_zeros + exp_str;

        if (std::all_of(test_str.begin(), test_str.end(), [](const char c) { return c == '0'; }))
        {
            exp_str = "0";
        }

        EXPECT_EQ(exp_str, zeroLStrip(test_str))
            << "[" << pre_zeros << ", " << num_str << ", " << post_zeros << "], res: " << test_str << std::endl;
    }
}

TEST(TestRStrip, TestBasic)
{
    EXPECT_EQ("", zeroRStrip(""));
    EXPECT_EQ("0", zeroRStrip("0"));
    EXPECT_EQ("0", zeroRStrip("00000"));
    EXPECT_EQ("0", zeroRStrip("00"));
    EXPECT_EQ("00001", zeroRStrip("00001"));
    EXPECT_EQ("0001", zeroRStrip("00010"));
    EXPECT_EQ("001", zeroRStrip("00100"));
    EXPECT_EQ("01", zeroRStrip("01000"));
    EXPECT_EQ("1", zeroRStrip("10000"));

    EXPECT_EQ("317", zeroRStrip("317"));
    EXPECT_EQ("317", zeroRStrip("3170"));
    EXPECT_EQ("317", zeroRStrip("31700"));
    EXPECT_EQ("317", zeroRStrip("317000"));

    EXPECT_EQ("00317", zeroRStrip("00317"));
    EXPECT_EQ("00317", zeroRStrip("003170"));
    EXPECT_EQ("00317", zeroRStrip("0031700"));
    EXPECT_EQ("00317", zeroRStrip("00317000"));

    for (size_t k = 0; k < 1000; k++)
    {
        size_t num = rand() % 998;
        if (num % 10 == 0)
        {
            num += 1;
        }

        const size_t num_pre_zeros = rand() % 5;
        const size_t num_post_zeros = rand() % 5;

        const std::string pre_zeros(num_pre_zeros, '0');
        const std::string post_zeros(num_post_zeros, '0');

        const std::string num_str = std::to_string(num);
        std::string exp_str = pre_zeros + num_str;
        const std::string test_str = pre_zeros + num_str + post_zeros;

        if (std::all_of(test_str.begin(), test_str.end(), [](const char c) { return c == '0'; }))
        {
            exp_str = "0";
        }

        EXPECT_EQ(exp_str, zeroRStrip(test_str))
            << "[" << pre_zeros << ", " << num_str << ", " << post_zeros << "], res: " << test_str << std::endl;
    }
}

TEST(TestStripNumberFromLastZeros, TestBasic)
{
    // clang-format off
    
    const std::vector<std::pair<std::string, std::string>> test_cases =
        {
            {"0.0", "0.0"},
            {"0.000", "0.0"},
            {"0.0000", "0.0"},
            {"0.00000", "0.0"},
            {"0001000.0002000e+0020", "1000.0002e+20"},
            {"1000.0002e+2", "1000.0002e+2"},
            {"0001000.0002000e20", "1000.0002e+20"},
            {"1000.0002e2", "1000.0002e+2"},
            {"0001000.0002000e-20", "1000.0002e-20"},
            {"0001000.0002000e-0020", "1000.0002e-20"},
            {"0001000.0002000e-020", "1000.0002e-20"},
            {"1000.0002e-2", "1000.0002e-2"},
            {"0001000.0002000e+0020", "1000.0002e+20"},
            {"1000.0002e+002", "1000.0002e+2"},
            {"0001000.0002000e0020", "1000.0002e+20"},
            {"1000.0002e002", "1000.0002e+2"},
            {"0001000.0002000e-0020", "1000.0002e-20"},
            {"1000.0002e-002", "1000.0002e-2"},
            {"1000.", "1000.0"},
            {"1000", "1000.0"},
            {"0.0002000", "0.0002"},
            {"000.0002000", "0.0002"},
            {".0002000", "0.0002"},
            {"0", "0.0"},
            {"00", "0.0"},
            {"00", "0.0"},
            {"001", "1.0"},
            {"0010", "10.0"},
            {"0.", "0.0"},
            {".0", "0.0"},
            {"0.43523400", "0.435234"},
            {"0.123", "0.123"},
            {"0.00000", "0.0"},
            {"000123.4000", "123.4"},
            {"1000.00", "1000.0"},
            {"-123.000", "-123.0"},
            {"1.230000e+2", "1.23e+2"},
            {"1.23e+20", "1.23e+20"},
            {"1.23e+10", "1.23e+10"},
            {"02e+10", "2.0e+10"},
            {"0", "0.0"},
            {".0000", "0.0"},
            {"0000.", "0.0"},
            {"0000", "0.0"},
            {"0.00100", "0.001"},
            {"0.0000001", "0.0000001"},
            {"0.1000", "0.1"},
            {"0.000", "0.0"},
            {"123.45000", "123.45"},
            {"0000.0000", "0.0"},
            {"0000.0001", "0.0001"},
            {"0.00001234000", "0.00001234"},
            {"1234567890.000", "1234567890.0"},
            {"0.10000000", "0.1"},
            {"123.000000", "123.0"},
            {"0.000000000", "0.0"},
            {"0.01010100", "0.010101"},
            {"0.9999900000", "0.99999"},
            {"0.1230000000000", "0.123"},
            {"0.000000000123", "0.000000000123"},
            {"1000000000.00000", "1000000000.0"},
            {"0.0000000000000000001", "0.0000000000000000001"},
            {"1000000000000000000.000000000000000000", "1000000000000000000.0"},
            {"0.1000", "0.1"},
            {"0.000", "0.0"},
            {"123.45000", "123.45"},
            {"0000.0000", "0.0"},
            {"0000.0001", "0.0001"},
            {"0.00001234000", "0.00001234"},
            {"1234567890.000", "1234567890.0"},
            {"0.e10", "0.0e+10"},
            {"0.0e10", "0.0e+10"}
        };

    for (const std::pair<std::string, std::string>& s : test_cases)
    {
        const std::string res{stripNumberFromLastZeros(s.first)};
        const std::string res_neg{stripNumberFromLastZeros("-" + s.first)};
        EXPECT_EQ(s.second, res) << "Input: " << s.first << ", Expected: " << s.second << ", Actual: " << res;
        EXPECT_EQ("-" + s.second, res_neg) << "Input: " << ("-" + s.first) << ", Expected: " << ("-" + s.second) << ", Actual: " << res;
    }


    // Signs can be
    const std::vector<std::string> signs = {"", "-"};

    // Before comma can be
    const std::vector<std::pair<std::string, std::string>> before_comma_vec =
        {
        {"", "0"},
        {"00", "0"},
        {"000", "0"},
        {"1", "1"},
        {"10", "10"},
        {"100", "100"},
        {"01", "1"},
        {"0010", "10"},
        {"000100", "100"}
    };

    // After comma can be
    const std::vector<std::pair<std::string, std::string>> after_comma_vec = {
    {"", "0"},
    {"0", "0"},
    {"00", "0"},
    {"000", "0"},
    {"1", "1"},
    {"10", "1"},
    {"100", "1"},
    {"01", "01"},
    {"0010", "001"},
    {"000100", "0001"}
    };

    // Exponent can be
    const std::vector<std::pair<std::string, std::string>> exponent_vec = {
        {"", ""},
        {"e+10", "e+10"},
        {"e10", "e+10"},
        {"e-10", "e-10"},
        {"e+010", "e+10"},
        {"e010", "e+10"},
        {"e-010", "e-10"},
        {"e+1", "e+1"},
        {"e1", "e+1"},
        {"e0", ""},
        {"e-0", ""},
        {"e+0", ""},
        {"e00", ""},
        {"e-00", ""},
        {"e+00", ""},
        {"e+01", "e+1"},
        {"e01", "e+1"},
        {"e+010", "e+10"},
        {"e010", "e+10"},
        {"e-01", "e-1"}
    };

    for (const std::string& sign : signs)
    {
        for (const std::pair<std::string, std::string>& before_comma : before_comma_vec)
        {
            for (const std::pair<std::string, std::string>& after_comma : after_comma_vec)
            {
                if(before_comma.first == "" && after_comma.first == "")
                {
                    continue;
                }
                for (const std::pair<std::string, std::string>& exponent : exponent_vec)
                {
                    const std::string test_str = sign + before_comma.first + "." + after_comma.first + exponent.first;
                    const std::string expected_str = sign + before_comma.second + "." + after_comma.second + exponent.second;

                    EXPECT_EQ(expected_str, stripNumberFromLastZeros(test_str));
                }
            }
        }
    }
    
    // clang-format off
}

TEST(TestStripNumberFromLastZeros, TestRandom)
{
    for (size_t k = 0; k < 100000; k++)
    {
        const size_t before_comma = rand() % 1000;
        size_t after_comma = rand() % 998;
        const size_t exponent = rand() % 1000;

        after_comma = (after_comma % 10U) == 0U ? after_comma + 1U : after_comma;

        const size_t num_pre_zeros = rand() % 5;
        const size_t num_post_zeros = rand() % 5;
        const size_t num_exp_zeros = rand() % 5;

        const bool negative_num = (rand() % 1000) > 500;
        const bool negative_exp = (rand() % 1000) > 500;

        const std::string sgn = (negative_num) ? "-" : "";
        const std::string exp_sgn = (negative_exp) ? "-" : "+";

        const std::string pre_zeros(num_pre_zeros, '0');
        const std::string post_zeros(num_post_zeros, '0');
        const std::string exp_zeros(num_exp_zeros, '0');

        const std::string before_comma_str = std::to_string(before_comma);
        const std::string after_comma_str = std::to_string(after_comma);
        const std::string exponent_str = std::to_string(exponent);

        std::string expected_exponent_str = "e" + exp_sgn + exponent_str;

        if (expected_exponent_str == "e+0" || expected_exponent_str == "e-0" || expected_exponent_str == "e0")
        {
            expected_exponent_str = "";
        }

        std::string expected_str = sgn + before_comma_str + "." + after_comma_str + expected_exponent_str;

        std::string test_str = sgn + pre_zeros + before_comma_str + "." + after_comma_str + "e" + exp_sgn + exp_zeros + exponent_str;

        EXPECT_EQ(expected_str, stripNumberFromLastZeros(test_str));
    }
}
