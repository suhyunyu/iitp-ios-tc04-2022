#pragma once

#include "ImageComparator_Base.hpp"

#include "opencv2/features2d.hpp"
#include "opencv2/calib3d.hpp"

#include <vector>

namespace lpin
{
	namespace opencv
	{
		/*
			(주로 OpenCV 기본 제공 형식들을 사용하여) 두 이미지에 대한 Transformation matrix를 찾아 비교를 수행하는 버전입니다.
			결과값 생성을 위한 코드는 포함되어 있지 않으며 각 모드를 구성할 때는 이 버전을 바탕으로 새 버전을 구성해서 활용할 수 있습니다.

			Detector_t: 각 이미지에서 특징점들을 찾을 때 사용할 형식입니다. cv::Feature2D 형식의 파생 형식이어야 합니다.
			matcherTypeCode: 두 특징점 목록을 비교할 때 어떤 방식을 사용할 것인지를 지정합니다. features2d.hpp에 있는 enum cv::DescriptorMatcher::MatcherType 정의를 참고해 주세요.
			numberOfImagesToUse: 비교 과정에서 몇 개의 이미지를 사용할 것인지를 지정합니다. 기본값은 2입니다.
		*/
		template <class Detector_t, cv::DescriptorMatcher::MatcherType matcherTypeCode, int numberOfImagesToUse = 2>
		class ImageComparator_Transformative : ImageComparator_Base
		{
		protected:
			static_assert(std::is_base_of_v<cv::Feature2D, Detector_t>,
				"Invalid detector type. "
				"ImageComparator_Transformative의 템플릿 인수 Detector_t가 조건에 맞지 않습니다. "
				"Detector_t 형식은 cv::Feature2D 형식의 파생 형식이어야 합니다.");

			cv::Ptr<Detector_t> detector;
			cv::Ptr<cv::DescriptorMatcher> matcher;

			struct Data
			{
				cv::Mat img;
				std::vector<cv::KeyPoint> keypoints;
				cv::Mat descriptors;
			};
			
			Data data[numberOfImagesToUse];
			bool isDataChanged = true;

			ImageComparator_Transformative() :
				detector(Detector_t::create()),
				matcher(cv::DescriptorMatcher::create(matcherTypeCode))
			{
			}

			~ImageComparator_Transformative() = default;


			void ExtractMetadataForImage(int idx)
			{
//                std::cout<<"ExtractMetadataForImage:"<<idx<<"\n";
				Data &data = this->data[idx];
                //std::cout<<"data size:"<<sizeof(data)<<"\n";
//                std::cout<<"data size:"<<sizeof(data.img)<<"\n";
//                std::cout<<"data keypoints:"<<&data.keypoints<<"\n";
//                std::cout<<"data descriptors :"<<data.descriptors<<"\n";
                //std::cout<<"data detector size:"<<detector<<"\n";
//                std::cout<<"data keypoints size:"<<data.img.size()<<"\n";
//                std::cout<<"data keypoints size:"<<data.keypoints.size()<<"\n";
//                std::cout<<"data descriptors size:"<<data.descriptors.size()<<"\n";
				detector->detectAndCompute(data.img, cv::noArray(), data.keypoints, data.descriptors);
//                std::cout<<"ExtractMetadataForImage detector"<<(detector)<<"\n";
                
			}

			/*
				Note.
				
				k == 2일 때 기준으로 작성되어 있음.
				변경하려는 경우 match 필터링을 수행하는 for문의 내용물(thresholdForSecondMatch 값을 사용하는 부분)을 변경해 주어야 함
			*/
			static constexpr int k_forKnnMatch = 2;
			static constexpr float thresholdForSecondMatch = 0.75f;
			static constexpr int min_matchesToFindHomography = 4;
			static constexpr int findHomography_mode = cv::USAC_PARALLEL;

			template <int idx_img_base, int idx_img_query>
			cv::Mat FindHomography()
			{
				std::vector<std::vector<cv::DMatch>> knn_matches;
//                std::cout<<"cv::Mat FindHomography\n";
//                std::cout<<"data[idx_img_query].descriptors"<<data[idx_img_query].descriptors<<"\n";
//                std::cout<<"data[idx_img_base].descriptors"<<data[idx_img_base].descriptors<<"\n";
//                std::cout<<"knn_matches:"<<knn_matches<<"\n";
//                std::cout<<"k_forKnnMatch"<<sizeof(k_forKnnMatch)<<"\n";
				matcher->knnMatch(data[idx_img_query].descriptors, data[idx_img_base].descriptors, knn_matches, k_forKnnMatch);
//                std::cout<<"cv::Mat FindHomography2\n";
				std::vector<cv::DMatch> matches_filtered;

				for ( auto &match : knn_matches )
					if ( match[0].distance < thresholdForSecondMatch * match[1].distance )
						matches_filtered.emplace_back(match[0]);

				if ( matches_filtered.size() <= min_matchesToFindHomography )
					return {};

				std::vector<cv::Point2f> mappedPoints_base;
				std::vector<cv::Point2f> mappedPoints_query;

				for ( auto &match : matches_filtered )
				{
					mappedPoints_base.push_back(data[idx_img_base].keypoints[match.trainIdx].pt);
					mappedPoints_query.push_back(data[idx_img_query].keypoints[match.queryIdx].pt);
				}

				cv::Mat result = cv::findHomography(mappedPoints_query, mappedPoints_base, findHomography_mode);

				return result;
			}

		public:
			static constexpr int idx_img_base = 0;
			static constexpr int idx_img_query = 1;

			/*
				Note.

				유효한 index 값은 static Data 멤버로 지정되어 있음
			*/
			template <int idx_img> requires ( idx_img >= 0 && idx_img < numberOfImagesToUse )
			void PutImage(cv::Mat img)
			{
                //std::cout<<"PutImage img:"<<sizeof(img)<<"\n";
				data[idx_img].img = img;
                //std::cout<<"PutImage data[idx_img]:"<<sizeof(data[idx_img])<<"\n";
				ExtractMetadataForImage(idx_img);
                //std::cout<<"ExtractMetadataForImage"<<idx_img<<", ";
				isDataChanged = true;
                //std::cout<<"isDataChanged:"<<isDataChanged<<"\n";
			}
		};
	}
}
