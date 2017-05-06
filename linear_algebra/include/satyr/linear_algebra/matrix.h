#pragma once

#include <satyr/n_array.h>

namespace satyr {
//------------------------------------------------------------------------------
// matrix
//------------------------------------------------------------------------------
template <Scalar Scalar>
using matrix = n_array<Scalar, 2, general_structure>;

//------------------------------------------------------------------------------
// matrix_view
//------------------------------------------------------------------------------
template <Scalar Scalar>
using matrix_view = n_array_view<Scalar, 2, general_structure>;

//------------------------------------------------------------------------------
// matrix_cview
//------------------------------------------------------------------------------
template <Scalar Scalar>
using matrix_cview = n_array_cview<Scalar, 2, general_structure>;

//------------------------------------------------------------------------------
// upper_triangular_matrix
//------------------------------------------------------------------------------
template <Scalar Scalar>
using upper_triangular_matrix = n_array<Scalar, 2, upper_triangular_structure>;

//------------------------------------------------------------------------------
// upper_triangular_matrix_view
//------------------------------------------------------------------------------
template <Scalar Scalar>
using upper_triangular_matrix_view =
    n_array_view<Scalar, 2, upper_triangular_structure>;

//------------------------------------------------------------------------------
// upper_triangular_matrix_cview
//------------------------------------------------------------------------------
template <Scalar Scalar>
using upper_triangular_matrix_cview =
    n_array_cview<Scalar, 2, upper_triangular_structure>;

//------------------------------------------------------------------------------
// lower_triangular_matrix
//------------------------------------------------------------------------------
template <Scalar Scalar>
using lower_triangular_matrix = n_array<Scalar, 2, lower_triangular_structure>;

//------------------------------------------------------------------------------
// lower_triangular_matrix_view
//------------------------------------------------------------------------------
template <Scalar Scalar>
using lower_triangular_matrix_view =
    n_array_view<Scalar, 2, lower_triangular_structure>;

//------------------------------------------------------------------------------
// lower_triangular_matrix_cview
//------------------------------------------------------------------------------
template <Scalar Scalar>
using lower_triangular_matrix_cview =
    n_array_cview<Scalar, 2, lower_triangular_structure>;

//------------------------------------------------------------------------------
// symmetric_matrix
//------------------------------------------------------------------------------
template <Scalar Scalar>
using symmetric_matrix = n_array<Scalar, 2, symmetric_structure>;

//------------------------------------------------------------------------------
// symmetric_matrix_view
//------------------------------------------------------------------------------
template <Scalar Scalar>
using symmetric_matrix_view =
    n_array_view<Scalar, 2, symmetric_structure>;

//------------------------------------------------------------------------------
// symmetric_matrix_cview
//------------------------------------------------------------------------------
template <Scalar Scalar>
using symmetric_matrix_cview =
    n_array_cview<Scalar, 2, symmetric_structure>;
} // namespace satyr
