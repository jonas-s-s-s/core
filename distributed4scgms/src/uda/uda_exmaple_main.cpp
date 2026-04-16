#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <algorithm>

#include <pagmo/algorithm.hpp>
#include <pagmo/population.hpp>
#include <pagmo/problem.hpp>

#include <pagmo/problems/rosenbrock.hpp>
#include <pagmo/problems/rastrigin.hpp>
#include <pagmo/problems/schwefel.hpp>
#include <pagmo/problems/ackley.hpp>

#include <scgms/rtl/SolverLib.h>

#include "uda_wrapper.h"
#include "scgms/rtl/UILib.h"
#include "scgms/utils/string_utils.h"

std::string to_string(const scgms::TSolver_Descriptor& desc)
{
    const std::wstring w(desc.description);
    return {w.begin(), w.end()};
}

bool is_excluded_solver(const scgms::TSolver_Descriptor& desc)
{
    static const std::vector<std::string> excludedSolvers = {
        "Generic distributed solver",
    };

    const auto name = to_string(desc);
    return std::any_of(excludedSolvers.begin(), excludedSolvers.end(),
                       [&](const std::string& ex) { return name == ex; });
}

void print_result(const std::string& solver_name,
                  const std::string& problem_name,
                  const pagmo::population& pop,
                  const double elapsed_ms)
{
    const auto& cf = pop.champion_f();
    const auto& cx = pop.champion_x();

    std::cout << std::fixed << std::setprecision(6);
    std::cout << "\n  Solver  : " << solver_name
        << "\n  Problem : " << problem_name
        << "\n  Best f  : " << cf[0]
        << "\n  Time    : " << elapsed_ms << " ms"
        << "\n  Best x  : [ ";
    for (size_t i = 0; i < std::min(cx.size(), static_cast<size_t>(5)); ++i)
        std::cout << cx[i] << " ";
    if (cx.size() > 5)
        std::cout << "... ";
    std::cout << "]\n";
}

bool run_test(const scgms::TSolver_Descriptor& desc,
              pagmo::problem& prob,
              const size_t pop_size,
              const size_t max_generations)
{
    const auto solver_name = to_string(desc);
    const std::string problem_name = prob.get_name();

    std::cout << "\n  Running " << solver_name << " on " << problem_name << " ...";

    try
    {
        const pagmo::algorithm algo{uda_wrapper{desc.id, max_generations, pop_size}};
        pagmo::population pop{prob, pop_size};

        const auto t0 = std::chrono::high_resolution_clock::now();
        pop = algo.evolve(pop);
        const auto t1 = std::chrono::high_resolution_clock::now();

        const double ms = std::chrono::duration<double, std::milli>(t1 - t0).count();

        print_result(solver_name, problem_name, pop, ms);
        return true;
    }
    catch (const std::exception& e)
    {
        std::cout << "\n  [FAILED] " << e.what() << "\n";
        return false;
    }
}

int main()
{
    // 1) Get all available SCGMS solvers
    std::cout << "Available SCGMS Solvers" << std::endl << std::endl;
    const auto solver_list = scgms::get_solver_descriptor_list();

    if (solver_list.empty())
    {
        std::cerr << "No solvers found. Is the SCGMS loaded?\n";
        return EXIT_FAILURE;
    }

    for (size_t i = 0; i < solver_list.size(); ++i)
        std::wcout << L"  [" << i << L"] " << solver_list[i].description << L"\n";

    // 2) Define problems on which we'll run our algorithms
    constexpr size_t dim = 10;
    std::vector<pagmo::problem> problems;
    problems.emplace_back(pagmo::rosenbrock{dim});
    problems.emplace_back(pagmo::rastrigin{dim});
    problems.emplace_back(pagmo::schwefel{dim});
    problems.emplace_back(pagmo::ackley{dim});

    // 3) Parameters
    constexpr size_t POP_SIZE = 100;
    constexpr size_t MAX_GENERATIONS = 1000;

    int passed = 0;
    int failed = 0;

    // 4) Run every solver on every problem
    for (auto& prob : problems)
    {
        std::cout << (prob.get_name() + "  (dim=" + std::to_string(dim) + ")") << std::endl << std::endl;

        for (const auto& solver : solver_list)
        {
            if (solver.specialized || is_excluded_solver(solver))
            {
                continue; // skip specialized or excluded solvers
            }

            if (run_test(solver, prob, POP_SIZE, MAX_GENERATIONS))
                ++passed;
            else
                ++failed;
        }
    }

    // 5) Print a summary
    std::cout << "Results" << std::endl;
    std::cout << "  Passed : " << passed << "\n" << "  Failed : " << failed << "\n" << "  Total  : " << passed + failed
        << "\n";

    return failed == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
