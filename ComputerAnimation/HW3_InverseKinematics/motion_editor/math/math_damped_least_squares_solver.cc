#include "math_damped_least_squares_solver.h"

namespace math {

// public func.

const char *DampedLeastSquaresSolver::tag()
{
    return "DampedLeastSquaresSolver";
}

DampedLeastSquaresSolver::DampedLeastSquaresSolver()
    :LinearSystemSolver(),
    damping_constant_(double{0.0})
{
}

DampedLeastSquaresSolver::DampedLeastSquaresSolver(const double damping_constant)
    :LinearSystemSolver(),
    damping_constant_(damping_constant)
{
}

DampedLeastSquaresSolver::~DampedLeastSquaresSolver()
{
}

std::string DampedLeastSquaresSolver::id() const
{
    return std::string(DampedLeastSquaresSolver::tag());
}

double DampedLeastSquaresSolver::damping_constant() const
{
    return damping_constant_;
}

void DampedLeastSquaresSolver::set_damping_constant(const double damping_constant)
{
    damping_constant_ = damping_constant;
}

math::VectorNd_t DampedLeastSquaresSolver::Solve(
	const math::MatrixN_t &coef_mat,
	const math::VectorNd_t &desired_vector
) const
{
	// find argmin_dt(|Jdt-de|^2+k^2*|dt|^2)
	// k: a non-zero real damping constant
	// dt = J* (JJ*+k^2I)^-1 de
	math::MatrixN_t I(coef_mat.rows(), coef_mat.rows());
	I.setIdentity();

	return coef_mat.transpose()*(coef_mat*coef_mat.transpose() + pow(damping_constant_, 2)*I).inverse()*desired_vector;
}
// protected func.

// private func.

} // namespace math {
