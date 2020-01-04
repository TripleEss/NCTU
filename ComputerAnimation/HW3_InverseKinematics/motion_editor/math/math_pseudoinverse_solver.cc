#include "math_pseudoinverse_solver.h"

namespace math {

// public func.

const char *PseudoinverseSolver::tag()
{
    return "PseudoinverseSolver";
}

PseudoinverseSolver::PseudoinverseSolver()
    :LinearSystemSolver()
{
}

PseudoinverseSolver::~PseudoinverseSolver()
{
}

std::string PseudoinverseSolver::id() const
{
    return std::string(PseudoinverseSolver::tag());
}

math::VectorNd_t PseudoinverseSolver::Solve(
        const math::MatrixN_t &coef_mat,
        const math::VectorNd_t &desired_vector
        ) const
{
	// de: d_endeffect; J: Jacobian m x n matrix; dt: d_theta
	// solve dt from de = Jdt

	// if columns of J are linearly independent (m>=n) then J*J is invertible
	// J* de = J* J dt
	// (J*J)^-1 J* de = (J*J)^-1 J* J dt = J^-1 J*^-1 J* J dt = dt
	//return (coef_mat.transpose()*coef_mat).inverse()*coef_mat.transpose()*desired_vector;

	// if rows of J are linearly independent (m<=n) then JJ* is invertible
	// (JJ*)^-1 de = (JJ*)^-1 J dt
	// J* (JJ*)^-1 de = J* (JJ*)^-1 J dt = J* J*^-1 J^-1 J dt = dt
	//return coef_mat.transpose()*(coef_mat*coef_mat.transpose()).inverse()*desired_vector;

	Eigen::JacobiSVD<Eigen::MatrixXd> svd(Eigen::MatrixXd(coef_mat), Eigen::ComputeThinU | Eigen::ComputeThinV);
	Eigen::Vector3d rhs(desired_vector);
	return svd.solve(rhs);
}

// protected func.

// private func.

} // namespace math {
