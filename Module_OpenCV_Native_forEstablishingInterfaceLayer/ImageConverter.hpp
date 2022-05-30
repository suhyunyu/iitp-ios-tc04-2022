#pragma once

#include "common.hpp"

namespace lpin
{
	namespace opencv
	{
		/*
		* ��� �ܺο��� ������ ��Ʈ�� �����͸� ������ �ٷ�� ���� static �Լ����� �����ϴ� Ŭ�����Դϴ�.
		* 
		* Note: ImageConverter::Convert()�� �׻� ��Ʈ�� �������� �纻�� ����ϴ�.
		*/
		template<int img_width, int img_height>
		class ImageConverter_Base
		{
		private:
			ImageConverter_Base() = delete;
			~ImageConverter_Base() noexcept = delete;
		public:
			static cv::Mat Convert(cv::Mat origin, bool convertToGrayscale)
			{
				cv::Mat gray;

				// �ʿ��� ��� grayscaleȭ
				if ( convertToGrayscale && origin.type() != CV_8UC1 )
				{
					cv::cvtColor(origin, gray, Constants::external_img_convertCode_in);
				}
				else
				{
					gray = origin.clone();
				}

				// ũ�� ������ ���ʿ��� ��� �ٷ� return
				if ( gray.cols == img_width && gray.rows == img_height )
				{
					return gray;
				}
				

				cv::Mat result;

				// ��Ⱦ�� ������ ��� ũ�⸸ ����
				if ( (double)gray.rows / gray.cols == (double)img_height / img_width )
				{
					cv::resize(gray, result, { img_width, img_height });
				}
				// ��Ⱦ�� �ٸ� ��� Crop �� ũ�� ����
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
				return Convert(cv::Mat(height, width, Constants::external_img_type, ptr), convertToGrayscale);
			}
		};

		typedef ImageConverter_Base<Constants::base_img_width, Constants::base_img_height> ImageConverter;

	}
}