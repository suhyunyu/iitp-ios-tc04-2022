#pragma once

#include "topmost.hpp"

#if LPIN_OPENCV_MODE == 3

#include "ImageComparator_Transformative.hpp"

#include <limits>

namespace lpin
{
	namespace opencv
	{
		template <>
		class ImageComparator<3> : public ImageComparator_Transformative<cv::SIFT, cv::DescriptorMatcher::FLANNBASED>
		{
		protected:
			double distance_last_calculated = std::numeric_limits<double>::quiet_NaN();

			// TODO 계수 보정 필요
			double dataForK[3*3] = { 2552.15303467009, 0, 1163.42456160096, 0, 2552.15303467009, 1579.25590667042, 0, 0, 1 };
			cv::Mat K = cv::Mat{ 3, 3, CV_64FC1, dataForK };
		public:
			cv::Mat H;

			double CalculateDistance(double horizontalDistance_base, double height_base, double angle_base)
			{
				if ( isDataChanged == false )
					return distance_last_calculated;

				H = FindHomography<idx_img_base, idx_img_query>();

				if ( H.data == nullptr )
				{
					distance_last_calculated = std::numeric_limits<double>::quiet_NaN();
					isDataChanged = false;

					return distance_last_calculated;
				}

				std::vector<cv::Mat> rotations;
				std::vector<cv::Mat> translations;
				std::vector<cv::Mat> normals;

				int numberOfSolutions = cv::decomposeHomographyMat(H, K, rotations, translations, normals);

				if ( numberOfSolutions == 0 )
				{
					distance_last_calculated = std::numeric_limits<double>::quiet_NaN();
					isDataChanged = false;

					return distance_last_calculated;
				}

				double sum_distances = 0.0;

				for ( int idx = 0; idx < numberOfSolutions; idx++ )
				{
					// TODO extrinsic factor 적용 부분 추가
					double tx = translations[idx].at<double>(0);
					double ty = translations[idx].at<double>(1);
					double tz = translations[idx].at<double>(2);
					double squaredDistance = tx * tx + ty * ty + tz * tz;
					sum_distances += cv::sqrt(squaredDistance);
				}

				distance_last_calculated = sum_distances / numberOfSolutions;
				isDataChanged = false;

				return distance_last_calculated;
			}
		};
	}
}

#endif