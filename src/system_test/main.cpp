#include <algorithm>
#include <functional>
#include <map>
#include <vector>

#include "advanced_cpp/tests.h"
#include "basic_c/tests.h"
#include "basic_cpp/tests.h"
#include "debug_value_args.h"
#include "lumos/lumos.h"
#include "two_way_comm/two_way_comm.h"
#include "utils.h"

TestMap tests_;

void displayHelp()
{
    std::cout << "Usage: ./system-test [\033[32m<language>\033[0m \033[33m<category>\033[0m "
                 "\033[35m<test name>\033[0m] [-h, --help, help]"
              << std::endl;
    std::cout << "Available tests:" << std::endl;
    for (const auto& lang : tests_)
    {
        std::cout << " - \033[32m" << lang.first << "\033[0m" << std::endl;

        for (const auto& test_category : lang.second)
        {
            std::cout << "    * \033[33m" << test_category.first << "\033[0m" << std::endl;

            for (const auto& test_name : test_category.second)
            {
                std::cout << "       \033[35m" << test_name.first << "\033[0m" << std::endl;
            }
        }
    }
}

int main(int argc, char* argv[])
{
    debug_value_args::parseArgs(argc, argv);

    basic_cpp::addTests();
    advanced_cpp::addTests();
    basic_c::addTests();
    two_way_comm::addTests();

    if (argc == 1)
    {
        std::cout << "No arguments provided!" << std::endl;
        displayHelp();
    }
    else if (argc == 2)
    {
        const std::string arg(argv[1]);

        if ((arg == "-h") || (arg == "--help") || (arg == "help"))
        {
            displayHelp();
        }
        else if (arg == "all")
        {
            std::cout << "Running all tests..." << std::endl << std::endl;

            for (const auto& lang : tests_)
            {
                for (const auto& test_category : lang.second)
                {
                    for (const auto& test_name : test_category.second)
                    {
                        std::string test_name_full = "\033[32m" + lang.first + "\033[0m::\033[33m" +
                                                     test_category.first + "\033[0m::\033[35m" + test_name.first +
                                                     "\033[0m";
                        std::cout << "Running " << test_name_full << "..." << std::endl;
                        test_name.second();
                        std::cout << test_name_full << " ran successfully. Press enter to continue." << std::endl
                                  << std::endl;
                        std::cin.ignore();
                    }
                }
            }
        }
        else
        {
            std::cout << "\033[31mERROR: No matching single argument for provided argument: \033[0m" << arg << std::endl
                      << std::endl;
            displayHelp();
        }
    }
    else
    {
        const std::string lang(argv[1]);

        if (tests_.count(lang) > 0)
        {
            const std::string test_category(argv[2]);
            if (tests_[lang].count(test_category) > 0)
            {
                const std::string test_name(argv[3]);

                if (tests_[lang][test_category].count(test_name) > 0)
                {
                    tests_[lang][test_category][test_name]();
                }
                else if (test_name == "all")
                {
                    std::cout << "Running all tests for language \033[32m" << lang
                              << "\033[0m in test category \033[33m" << test_category << "\033[0m..." << std::endl
                              << std::endl;

                    for (const auto& test_category : tests_[lang])
                    {
                        for (const auto& test_name : test_category.second)
                        {
                            std::string test_name_full = "\033[32m" + lang + "\033[0m::\033[33m" + test_category.first +
                                                         "\033[0m::\033[35m" + test_name.first + "\033[0m";
                            std::cout << "Running " << test_name_full << "..." << std::endl;
                            test_name.second();
                            std::cout << test_name_full << " ran successfully. Press enter to continue." << std::endl
                                      << std::endl;
                            std::cin.ignore();
                        }
                    }
                }
                else
                {
                    std::cout << "\033[31mERROR: No matching test name for provided third argument: \033[0m"
                              << test_name << std::endl
                              << std::endl;
                    displayHelp();
                }
            }
            else if (test_category == "all")
            {
                std::cout << "Running all tests for language \033[32m" << lang << "\033[0m..." << std::endl
                          << std::endl;

                for (const auto& test_category : tests_[lang])
                {
                    for (const auto& test_name : test_category.second)
                    {
                        std::string test_name_full = "\033[32m" + lang + "\033[0m::\033[33m" + test_category.first +
                                                     "\033[0m::\033[35m" + test_name.first + "\033[0m";
                        std::cout << "Running " << test_name_full << "..." << std::endl;
                        test_name.second();
                        std::cout << test_name_full << " ran successfully. Press enter to continue." << std::endl
                                  << std::endl;
                        std::cin.ignore();
                    }
                }
            }
            else
            {
                std::cout << "\033[31mERROR: No matching test category for provided second argument: \033[0m"
                          << test_category << std::endl
                          << std::endl;
                displayHelp();
            }
        }
        else
        {
            std::cout << "\033[31mERROR: No matching language for provided first argument: \033[0m" << lang << std::endl
                      << std::endl;
            displayHelp();
        }
    }
}
