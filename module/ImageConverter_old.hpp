#pragma once

#include "Constants.hpp"

namespace lpin
{
	namespace opencv
	{
		/*
			모듈 외부에서 가져온 비트맵 데이터를 가져와 다루기 위한 static 함수들을 노출하는 클래스입니다.
			Note: ImageConverter::Convert()는 항상 비트맵 데이터의 사본을 만듭니다.
		*/
		template<int img_width, int img_height>
		class ImageConverter_Old
		{
		private:
			ImageConverter_Old() = delete;
			~ImageConverter_Old() noexcept = delete;
		public:
			static cv::Mat Convert(cv::Mat origin, bool convertToGrayscale)
			{
				cv::Mat gray;

				// 필요한 경우 grayscale화
				if ( convertToGrayscale && origin.type() != CV_8UC1 )
				{
					cv::cvtColor(origin, gray, Constants<mode>::img_convertCode_in_grayscale);
				}
				else
				{
					gray = origin.clone();
				}

				// 크기 조정이 불필요한 경우 바로 return
				if ( gray.cols == img_width && gray.rows == img_height )
				{
					return gray;
				}
				

				cv::Mat result;

				// 종횡비 동일한 경우 크기만 조정
				if ( (double)gray.rows / gray.cols == (double)img_height / img_width )
				{
					cv::resize(gray, result, { img_width, img_height });
				}
				// 종횡비 다른 경우 Crop 및 크기 조정
				else
				{
					int crop_start_row = gray.rows > gray.cols ?
						(gray.rows - gray.cols) / 2 :
						0;

					int crop_start_column = crop_start_row == 0 ?
						(gray.cols - gray.rows) / 2 :
						0;

					int crop_end_row = crop_start_row == 0 ?
						gray.rows :
						crop_start_row + gray.cols;

					int crop_end_column = crop_start_column == 0 ?
						gray.cols :
						crop_start_column + gray.rows;

					cv::resize(gray(cv::Range(crop_start_row, crop_end_row), cv::Range(crop_start_column, crop_end_column)), result, { img_width, img_height });
				}

				return result;
			}

			static cv::Mat Convert(void *ptr, int width, int height, bool convertToGrayscale)
			{
				return Convert(cv::Mat(height, width, Constants<mode>::external_img_type, ptr), convertToGrayscale);
			}
		};

	}
}