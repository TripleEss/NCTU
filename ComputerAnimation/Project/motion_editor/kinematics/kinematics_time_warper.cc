#include "kinematics_time_warper.h"
#include <utility>
#include "boost/numeric/conversion/cast.hpp"
#include "math_utils.h"

namespace kinematics {

// public func.

TimeWarper::TimeWarper()
    :original_motion_sequence_(new math::SpatialTemporalVector6d_t),
    hard_constraint_coll_(new TimeWarpHardConstraintColl_t),
    time_step_(double{0.0}),
    min_time_step_(double{0.0}),
    max_time_step_(double{0.0})
{
}

TimeWarper::~TimeWarper()
{
}

double TimeWarper::time_step() const
{
    return time_step_;
}

double TimeWarper::min_time_step() const
{
    return min_time_step_;
}

double TimeWarper::max_time_step() const
{
    return max_time_step_;
}

void TimeWarper::Configure(
        const math::SpatialTemporalVector6d_t &original_motion_sequence,
        const double time_step,
        const double min_time_step,
        const double max_time_step
        )
{
    *original_motion_sequence_ = original_motion_sequence;
    time_step_ = time_step;
    min_time_step_ = min_time_step;
    max_time_step_ = max_time_step;
}

math::SpatialTemporalVector6d_t TimeWarper::ComputeWarpedMotion(
        const TimeWarpHardConstraintColl_t &hard_constraint_coll
        )
{
    *hard_constraint_coll_ = hard_constraint_coll;

	// warp[frame_idx] = new play_second for oringinal motion capture frame
	double *warp_playsec = new double[original_motion_sequence_->temporal_size()];

	for (int i = 0; i < hard_constraint_coll_->size(); ++i) {
		LOGMSG << "constraint " << i << ": frame " << hard_constraint_coll_->at(i).frame_idx << " at " << hard_constraint_coll_->at(i).play_second << "s" << std::endl;
	}

	for (int i =1; i < hard_constraint_coll_->size(); ++i) {
		int total_frame = hard_constraint_coll_->at(i).frame_idx - hard_constraint_coll_->at(i - 1).frame_idx;
		double total_time = hard_constraint_coll_->at(i).play_second - hard_constraint_coll_->at(i - 1).play_second;
		// uniformly sampled
		double time_step = total_time / total_frame;
		for (int t = 0; t <= total_frame; ++t) {
			int idx = hard_constraint_coll_->at(i - 1).frame_idx + t;
			warp_playsec[idx] = hard_constraint_coll_->at(i - 1).play_second + t*time_step;
		}
	}

	math::SpatialTemporalVector6d_t new_motion_sequence(original_motion_sequence_->spatial_size(), original_motion_sequence_->temporal_size());
	
	int scanned = 0;
	for (int t = 0; t < original_motion_sequence_->temporal_size(); ++t) {
		double now = t*time_step_;

		// warp_playsec[frame1] <= now < warp_playsec[frame2]
		for (; scanned < original_motion_sequence_->temporal_size() && now > warp_playsec[scanned + 1]; ++scanned) {}		
		int frame1 = (scanned < original_motion_sequence_->temporal_size()) ? scanned : original_motion_sequence_->temporal_size() - 1;
		int frame2 = (frame1 < original_motion_sequence_->temporal_size() - 1) ? frame1 + 1 : original_motion_sequence_->temporal_size() - 1;

		double ratio = 1;
		if (warp_playsec[frame2] - warp_playsec[frame1] > 0) {
			ratio = (now - warp_playsec[frame1]) / (warp_playsec[frame2] - warp_playsec[frame1]);
		}
		//cout << t << ", " << now << ", (" << frame1 << ":" << warp_playsec[frame1] << "," << frame2 << ":" << warp_playsec[frame2] << "), (" << 1 - ratio << "," << ratio << ")" << endl;

		for (int s = 0; s < original_motion_sequence_->spatial_size(); ++s) {
			math::Vector6d_t amc1 = original_motion_sequence_->element(s, frame1);
			math::Vector6d_t amc2 = original_motion_sequence_->element(s, frame2);
			// translation linear interpolation
			math::Vector3d_t t1(amc1[3], amc1[4], amc1[5]);
			math::Vector3d_t t2(amc2[3], amc2[4], amc2[5]);
			math::Vector3d_t mixt = math::Lerp(t1, t2, ratio);
			// rotation spherical linear interpolation
			math::Vector3d_t r1(amc1[0], amc1[1], amc1[2]);
			math::Vector3d_t r2(amc2[0], amc2[1], amc2[2]);
			// euler angle -> quaternion
			math::Quaternion_t q1 = math::ComputeQuaternionXyz(math::ToRadian(r1[0]), math::ToRadian(r1[1]), math::ToRadian(r1[2]));
			math::Quaternion_t q2 = math::ComputeQuaternionXyz(math::ToRadian(r2[0]), math::ToRadian(r2[1]), math::ToRadian(r2[2]));
			// slerp(q1,q2,r) = [sin((1-r)*thata)/sin(thata)]*q1 + [sin(r*theta)/sin(thata)]*q2
			math::Quaternion_t mixq = math::Slerp(q1, q2, ratio);
			// quaternion -> rotation matrix -> euler angle
			math::Vector3d_t mixr = math::ToDegree(math::ComputeEulerAngleXyz(math::ComputeRotMat(mixq)));
			//math::Vector3d_t mixr = math::Lerp(r1, r2, ratio);
			math::Vector6d_t mixamc(mixr, mixt);

			new_motion_sequence.set_element(s, t, mixamc);
		}
	}

	delete warp_playsec;

    return new_motion_sequence;
}

// protected func.

// private func.

} // namespace kinematics {
