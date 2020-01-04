#include "kinematics_forward_solver.h"
#include <algorithm>
#include "math_utils.h"
#include "acclaim_skeleton.h"
#include "acclaim_motion.h"
#include "helper_forward_kinematics.h"
#include "kinematics_artic_idx.h"
#include "kinematics_pose.h"

#include <iostream>

using namespace std;

namespace kinematics {

// public func.

ForwardSolver::ForwardSolver()
    :skeleton_(nullptr),
    motion_(nullptr),
    artic_path_(new ArticIdxColl_t),
    helper_fk_(new helper::ForwardKinematics)
{
}

ForwardSolver::~ForwardSolver()
{
}

std::shared_ptr<acclaim::Skeleton> ForwardSolver::skeleton() const
{
    return skeleton_;
}

std::shared_ptr<acclaim::Motion> ForwardSolver::motion() const
{
    return motion_;
}

void ForwardSolver::set_skeleton(const std::shared_ptr<acclaim::Skeleton> &skeleton)
{
    skeleton_ = skeleton;
    helper_fk_->set_skeleton(skeleton_);
}

void ForwardSolver::set_motion(const std::shared_ptr<acclaim::Motion> &motion)
{
    motion_ = motion;
    helper_fk_->set_skeleton(skeleton_);
}

void ForwardSolver::ConstructArticPath()
{
    helper_fk_->ConstructArticPath();
}

PoseColl_t ForwardSolver::ComputeSkeletonPose(const int32_t frame_idx)
{
    return this->ComputeSkeletonPose(motion_->joint_spatial_pos(frame_idx));
}

// R(a->b): rotation matrix from local_coord_a to local_coord_b
// R_asf(i): rotation matrix from local_coord_i to local_coord_i'parent
// R_amc(i): rotation matrix for i in local_coord_i at a particular frame
// unitV(i)/V(i): unit vector/vector from local_coord_i_origin to local_coord_i'child_origin
// L(i): length of i
// T(i): start poisition of i in global_coord
PoseColl_t ForwardSolver::ComputeSkeletonPose(const math::Vector6dColl_t &joint_spatial_pos)
{
	//PoseColl_t helper_pose = helper_fk_->ComputeSkeletonPose(joint_spatial_pos);

	// R(i+1->i) = R_asf(i)*R_acm(i)
	math::RotMat3d_t *rot_cb2pb = new math::RotMat3d_t[skeleton_->bone_num()];
	for (int i = 0; i < skeleton_->bone_num(); ++i) {
		const acclaim::Bone* cb = skeleton_->bone_ptr(i);

		math::Vector3d_t amc_r = math::ToRadian(math::Vector3d_t(joint_spatial_pos[i][0], joint_spatial_pos[i][1], joint_spatial_pos[i][2]));
		math::RotMat3d_t amc_rot = math::ComputeRotMatXyz(amc_r[0], amc_r[1], amc_r[2]);
		rot_cb2pb[i] = math::ToRotMat(cb->rot_parent_current).transpose()*amc_rot;
	}

	PoseColl_t fk_pose(skeleton_->bone_num());

	// root
	math::Vector3d_t root_pos = math::Vector3d_t(joint_spatial_pos[0][3], joint_spatial_pos[0][4], joint_spatial_pos[0][5]);
	fk_pose[0].set_start_pos(root_pos);
	fk_pose[0].set_end_pos(fk_pose[0].start_pos());
	fk_pose[0].set_rotation(rot_cb2pb[0]);

	for (int i = 1; i < skeleton_->bone_num(); ++i) {
		const acclaim::Bone* cb = skeleton_->bone_ptr(i);
		const acclaim::Bone* pb = cb->parent;

		// R(i-1->g) = R(0->g)*R(1->0)*R(2->1)*...*R(i-1->i-2)
		math::RotMat3d_t rot_pb2global = fk_pose[pb->idx].rotation();
		// V(i-1) = unitV(i-1)*L(i-1)
		math::Vector3d_t dir_pb = pb->dir*pb->length;
		// T(i-1)
		math::Vector3d_t startpos_pb = fk_pose[pb->idx].start_pos();

		// T(i) = R(i-1->g)*V(i-1) + T(i-1)
		fk_pose[i].set_start_pos(rot_pb2global*dir_pb + startpos_pb);
		// R(i->g) = R(0->g)*R(1->0)*R(2->1)*...*R(i-1->i-2)*R(i->i-1) = R(i-1->g)*R(i->i-1)
		fk_pose[i].set_rotation(rot_pb2global*rot_cb2pb[i]);
		// T(i).end = T(i+1) = R(i+1-1->g)*V(i+1-1) + T(i+1-1) = R(i->g)*V(i) + T(i)
		fk_pose[i].set_end_pos(fk_pose[i].rotation()*(cb->dir*cb->length) + fk_pose[i].start_pos());
	}
	
	/*
	for (int i = 0; i < skeleton_->bone_num(); ++i) {
		cout << "----- Bone " << i << endl;
		cout << "fk_pose.start     " << fk_pose[i].start_pos() << endl;
		cout << "fk_pose.end       " << fk_pose[i].end_pos() << endl;
		cout << "fk_pose.rot" << endl << fk_pose[i].rotation() << endl << endl;

		cout << "helper_pose.start " << helper_pose[i].start_pos() << endl;
		cout << "helper_pose.end   " << helper_pose[i].end_pos() << endl;
		cout << "helper_pose.rot" << endl << helper_pose[i].rotation() << endl;

		if (fk_pose[i].start_pos() != helper_pose[i].start_pos())
			LOGERR << "start err " << fk_pose[i].start_pos() - helper_pose[i].start_pos() << endl;
		if (fk_pose[i].end_pos() != helper_pose[i].end_pos())
			LOGERR << "end err" << fk_pose[i].end_pos() - helper_pose[i].end_pos() << endl;
		if (fk_pose[i].rotation() != helper_pose[i].rotation())
			LOGERR << "rot err" << fk_pose[i].rotation() - helper_pose[i].rotation() << endl;
	}
	*/
	
	return fk_pose;
	//return helper_fk_->ComputeSkeletonPose(joint_spatial_pos);
}

// protected func.

// private func.

} // namespace kinematics {
