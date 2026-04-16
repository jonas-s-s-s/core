#pragma once

#include <pagmo/population.hpp>
#include <scgms/rtl/SolverLib.h>

#include <string>
#include <stdexcept>
#include <limits>
#include <vector>

#include "dll_visibility.h"

class DLL_PUBLIC uda_wrapper
{
public:
    explicit uda_wrapper(const GUID& solver_id,
                         const size_t max_generations = 100000,
                         const size_t population_size = 100);

    // Must exist or else this class won't be recognized as Pagmo UDA
    uda_wrapper() = default;

    pagmo::population evolve(pagmo::population pop) const;

    std::string get_name() const;

    std::string get_extra_info() const;

private:
    GUID m_solver_id;
    size_t m_max_generations;
    size_t m_population_size;
};
