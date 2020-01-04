#ifndef _KINEMATICS_CARTOON_FILTER_H_
#define _KINEMATICS_CARTOON_FILTER_H_

#include "kinematics_def.h"
#include <memory>
#include <utility>

#include "kinematics_type.h"
#include "kinematics_pose.h"

namespace kinematics {

	class CartoonFilter final
	{

	public:

		CartoonFilter();
		CartoonFilter(const CartoonFilter &) = delete;
		virtual ~CartoonFilter();
		CartoonFilter &operator=(const CartoonFilter &) = delete;

		void Configure(kinematics::PoseCollColl_t original_pose_sequence);

		void set_time_step(const double time_step);
		void set_amplitude(const double amplitude);
		void set_sigma_window_size(const int sigma_window_size_);

		kinematics::PoseCollColl_t ComputeFilteredMotion();

	protected:

	private:

		double time_step_;
		double amplitude_;
		int sigma_window_size_;

		kinematics::PoseCollColl_t original_pose_sequence_;

		math::Vector3d_t GenerateSigmaQuaternion(math::Vector3dColl_t dx);
		math::Vector3d_t GenerateSigmaEuler(math::Vector3dColl_t dx);
	};
}

#endif
