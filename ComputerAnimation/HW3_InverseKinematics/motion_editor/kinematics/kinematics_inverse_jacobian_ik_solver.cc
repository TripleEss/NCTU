#include "kinematics_inverse_jacobian_ik_solver.h"
#include <limits>
#include "console_log.h"
#include "math_utils.h"
#include "math_linear_system_solver.h"
#include "acclaim_skeleton.h"
#include "acclaim_motion.h"
#include "kinematics_forward_solver.h"
#include "kinematics_pose.h"

namespace kinematics {

InverseJacobianIkSolver::InverseJacobianIkSolver()
    :skeleton_(nullptr),
    fk_solver_(new ForwardSolver),
    step_(double{0.0}),
    distance_epsilon_(double{0.0}),
    max_iteration_num_(0),
    linear_system_solver_(nullptr)
{
}

InverseJacobianIkSolver::~InverseJacobianIkSolver()
{
}

void InverseJacobianIkSolver::Configure(
        const std::shared_ptr<acclaim::Skeleton> &skeleton,
        const std::shared_ptr<math::LinearSystemSolver> &linear_system_solver,
        const double step,
        const double distance_epsilon,
        const int32_t max_iteration_num
        )
{
    skeleton_ = skeleton;
    fk_solver_->set_skeleton(skeleton_);
    fk_solver_->ConstructArticPath();

    linear_system_solver_ = linear_system_solver;

    step_ = step;
    distance_epsilon_ = distance_epsilon;
    max_iteration_num_ = max_iteration_num;
}

math::Vector6dColl_t InverseJacobianIkSolver::Solve(
        const math::Vector3d_t &target_pos,
        const int32_t start_bone_idx,
        const int32_t end_bone_idx,
        const math::Vector6dColl_t &original_whole_body_joint_pos6d
        )
{
	// construct path from start_bone to end_bone
	std::vector<int> path;
	int dofnum = 0;
	for (int current = end_bone_idx; current != start_bone_idx; current = skeleton_->bone_ptr(current)->parent->idx) {
		path.insert(path.begin(), current);
		dofnum += skeleton_->bone_ptr(current)->dof;
	}
	path.insert(path.begin(), start_bone_idx);
	dofnum += skeleton_->bone_ptr(start_bone_idx)->dof;
	
	// inverse kinematics
	math::Vector6dColl_t amc = original_whole_body_joint_pos6d;
	PoseColl_t pose = fk_solver_->ComputeSkeletonPose(amc);
	math::Vector3d_t end_effect = pose[end_bone_idx].end_pos();
	for (int it = 0; (end_effect - target_pos).norm() >= distance_epsilon_ && it < max_iteration_num_; ++it) {
		pose = fk_solver_->ComputeSkeletonPose(amc);
		end_effect = pose[end_bone_idx].end_pos();

		// there are dofnum dofs in the path
		// Jacobian = [J1 J2 ... Jdofnum]
		// Ji = ai x (end_effect-pi)
		// ai: the unit vector of the rotation axis of dof i
		// pi: the start position of bone with dof i
		math::MatrixN_t Jacobian(3, dofnum);
		int dof = 0;
		for (int i = 0; i < path.size(); ++i) {
			int current = path[i];
			const acclaim::Bone *cb = skeleton_->bone_ptr(current);

			math::Vector3d_t delta_pos = end_effect - pose[current].start_pos();
			if (cb->dofx) {
				Jacobian.col(dof) = pose[current].rotation().col(0).normalized().cross(delta_pos);
				dof++;
			}
			if (cb->dofy) {
				Jacobian.col(dof) = pose[current].rotation().col(1).normalized().cross(delta_pos);
				dof++;
			}
			if (cb->dofz) {
				Jacobian.col(dof) = pose[current].rotation().col(2).normalized().cross(delta_pos);
				dof++;
			}
		}

		// J*d_theta = d_endeffect
		// d_theta = J^-1*d_endeffect
		// theta' = theta+step*d_theta
		math::VectorNd_t d_endeffect = target_pos - end_effect;
		math::VectorNd_t d_theta = linear_system_solver_->Solve(Jacobian, d_endeffect);

		dof = 0;
		for (int i = 0; i < path.size(); ++i) {
			int current = path[i];
			const acclaim::Bone *cb = skeleton_->bone_ptr(current);

			if (cb->dofx) {
				amc[current][0] += step_*math::ToDegree(d_theta[dof]);
				dof++;
			}
			if (cb->dofy) {
				amc[current][1] += step_*math::ToDegree(d_theta[dof]);
				dof++;
			}
			if (cb->dofz) {
				amc[current][2] += step_*math::ToDegree(d_theta[dof]);
				dof++;
			}
		}
	}
	
    return amc;
}

} // namespace kinematics {

