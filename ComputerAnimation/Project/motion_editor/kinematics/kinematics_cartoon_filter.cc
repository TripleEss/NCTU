#include "kinematics_cartoon_filter.h"
#include <utility>
#include "boost/numeric/conversion/cast.hpp"
#include "math_utils.h"

#include "acclaim_skeleton.h"
#include "acclaim_motion.h"
#include "kinematics_type.h"
#include "kinematics_forward_solver.h"
#include "kinematics_pose.h"

namespace kinematics {

	// public func.

	CartoonFilter::CartoonFilter()
	{
	}

	CartoonFilter::~CartoonFilter()
	{
	}
	
	void CartoonFilter::Configure(kinematics::PoseCollColl_t original_pose_sequence)
	{
		original_pose_sequence_ = original_pose_sequence;
	}

	void CartoonFilter::set_time_step(const double time_step)
	{
		time_step_ = time_step;
	}

	void CartoonFilter::set_amplitude(const double amplitude)
	{
		amplitude_ = amplitude;
	}

	void CartoonFilter::set_sigma_window_size(const int sigma_window_size)
	{
		sigma_window_size_ = sigma_window_size;
	}

	kinematics::PoseCollColl_t CartoonFilter::ComputeFilteredMotion()
	{
		int frame_num = original_pose_sequence_.size();
		int bone_num = original_pose_sequence_[0].size();

		kinematics::PoseCollColl_t filtered_pose_sequence(frame_num, kinematics::PoseColl_t(bone_num));
		// dx = x'
		// ddx = x''
		std::vector<math::Vector3dColl_t> dx(frame_num, math::Vector3dColl_t(bone_num));
		std::vector<math::Vector3dColl_t> ddx(frame_num, math::Vector3dColl_t(bone_num));
		// compute dx
		for (int f = 0; f < frame_num - 1; ++f) {
			for (int b = 0; b < bone_num; ++b) {
				dx[f][b] = (original_pose_sequence_[f + 1][b].end_pos() - original_pose_sequence_[f][b].end_pos());
			}
		}
		for (int b = 0; b < bone_num; ++b) {
			dx[frame_num - 1][b] = dx[frame_num - 2][b];
		}
		// compute ddx
		for (int f = 0; f < frame_num - 1; ++f) {
			for (int b = 0; b < bone_num; ++b) {
				ddx[f][b] = (dx[f + 1][b] - dx[f][b]);
			}
		}
		for (int b = 0; b < bone_num; ++b) {
			ddx[frame_num - 1][b] = ddx[frame_num - 2][b];
		}

		double delta_t = 0;
		std::vector<std::vector<math::Vector3d_t>> Gaussain(frame_num, std::vector<math::Vector3d_t>(bone_num));
		for (int f = 0; f < frame_num; ++f) {

			int start = f - sigma_window_size_ / 2;
			int end = f + sigma_window_size_ / 2 + sigma_window_size_ % 2 - 1;

			for (int b = 0; b < bone_num; ++b) {

				math::Vector3dColl_t windowed_dx(sigma_window_size_);
				for (int i = start; i <= end; ++i) {
					if (i < 0 || i >= sigma_window_size_)
						windowed_dx[i - start].setZero();
					else
						windowed_dx[i - start] = dx[i][b];
				}
				
				double t = (double)f*time_step_;
				math::Vector3d_t sigma;
				//sigma.setOnes();
				//sigma = GenerateSigmaQuaternion(windowed_dx);
				sigma = GenerateSigmaEuler(windowed_dx);

				for (int i = 0; i < 3; ++i) {
					Gaussain[f][b][i] = exp(-pow(t / sigma[i] + delta_t, 2));
				//std::cout << Gaussain[f] << std::endl;
				}
			}
		}

		for (int f = 0; f < frame_num; ++f) {
			for (int b = 0; b < bone_num; ++b) {
				math::Vector3d_t delta_x;
				delta_x.setZero();
				for (int ff = 0; ff <= f; ++ff) {
					for (int i = 0; i < 3; ++i) {
						delta_x[i] += ddx[ff][b][i] * amplitude_*Gaussain[f - ff][b][i];
					}
				}
				filtered_pose_sequence[f][b].set_end_pos(original_pose_sequence_[f][b].end_pos() - delta_x);
			}
		}

		return filtered_pose_sequence;
	}

	math::Vector3d_t CartoonFilter::GenerateSigmaQuaternion(math::Vector3dColl_t dx)
	{
		
		std::vector<double> norm(sigma_window_size_);

		for (int i = 0; i < sigma_window_size_; ++i) {
			math::Vector4d_t Q;
			Q.setZero();
			for (int j = 0; j < sigma_window_size_; j++) {
				double theta = 2 * M_PI * i * j / sigma_window_size_;
				Q[0] += -dx[j][0] * -sin(theta);
				Q[1] += dx[j][0] * cos(theta);
				Q[2] += dx[j][1] * cos(theta) + (dx[j][2] * -sin(theta));
				Q[3] += dx[j][2] * cos(theta) - (dx[j][1] * -sin(theta));
			}
			norm[i] = Q.norm();
		}

		double maxnorm = 0;
		for (int i = 0; i < sigma_window_size_; ++i) {
			if (norm[i] > maxnorm) {
				maxnorm = norm[i];
			}
		}

		math::Vector3d_t sigma;
		sigma.setConstant(2 * M_PI / maxnorm);

		return sigma;
	}

	math::Vector3d_t CartoonFilter::GenerateSigmaEuler(math::Vector3dColl_t dx)
	{

		std::vector<double> normx(sigma_window_size_);
		std::vector<double> normy(sigma_window_size_);
		std::vector<double> normz(sigma_window_size_);

		for (int i = 0; i < sigma_window_size_; ++i) {
			double sumreal[3]; sumreal[0] = 0; sumreal[1] = 0; sumreal[2] = 0;
			double sumimag[3]; sumimag[0] = 0; sumimag[1] = 0; sumimag[2] = 0;
			for (int j = 0; j < sigma_window_size_; j++) {
				double theta = 2 * M_PI * i * j / sigma_window_size_;
				sumreal[0] += dx[j][0] * cos(theta);
				sumimag[0] += dx[j][0] * -sin(theta);
				sumreal[1] += dx[j][1] * cos(theta);
				sumimag[1] += dx[j][1] * -sin(theta);
				sumreal[2] += dx[j][2] * cos(theta);
				sumimag[2] += dx[j][2] * -sin(theta);

			}
			normx[i] = sqrt(sumreal[0] * sumreal[0] + sumimag[0] * sumimag[0]);
			normy[i] = sqrt(sumreal[1] * sumreal[1] + sumimag[1] * sumimag[1]);
			normz[i] = sqrt(sumreal[2] * sumreal[2] + sumimag[2] * sumimag[2]);
		}

		double maxnormx = 0; double maxnormy = 0; double maxnormz = 0;
		for (int i = 0; i < sigma_window_size_; ++i) {
			if (normx[i] > maxnormx) {
				maxnormx = normx[i];
			}
		}

		for (int i = 0; i < sigma_window_size_; ++i) {
			if (normy[i] > maxnormy) {
				maxnormy = normy[i];
			}
		}
		for (int i = 0; i < sigma_window_size_; ++i) {
			if (normz[i] > maxnormz) {
				maxnormz = normz[i];
			}
		}

		math::Vector3d_t sigma;
		sigma.set_x(2 * M_PI / maxnormx);
		sigma.set_y(2 * M_PI / maxnormy);
		sigma.set_z(2 * M_PI / maxnormz);
		return sigma;
	}
}
