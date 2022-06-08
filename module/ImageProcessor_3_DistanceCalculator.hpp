#pragma once

#include "ImageProcessor_Base.hpp"
#include <limits>

namespace lpin
{
	namespace opencv
	{
		/*
			거리 측정 기능을 구성해 둔 버전입니다.
			Static 버전이며 생성자 등은 모두 삭제되어 있습니다.

			이미지 두 개와 metadata(double 값 세 개)를 필요로 하며,
			이들은 각각 PutBaseImage(), PutQueryImage(), PutMetadata()를 통해 입력할 수 있습니다.
			입력 후 CalculateDistance()를 호출하여 거리를 계산할 수 있습니다.

			- 입력용 함수의 호출 순서는 자유입니다.
			- 계산 이후에도 입력해 둔 이미지 및 metadata는 남아 있습니다.
			  원한다면 이들 중 일부만 새로 입력한 다음 두 번째 계산을 수행할 수 있습니다.
		*/
		template <>
		struct ImageProcessor<3> : ImageProcessor_Base
		{
		protected:
			/*
				Note.
				
				특정 형식 object 생성을 아무도 못 하게 만들고 싶을 때는 해당 형식 정의 안에 아래와 같은 느낌으로 = delete를 붙인 함수 선언 6개를 적어 두면 됩니다. (public인지 여부는 상관 없음)
			*/
			ImageProcessor() = delete;
			ImageProcessor(const ImageProcessor &) = delete;
			ImageProcessor(ImageProcessor &&) = delete;
			~ImageProcessor() = delete;

			ImageProcessor &operator =(const ImageProcessor &) = delete;
			ImageProcessor &operator =(ImageProcessor &&) = delete;


			using Constants = Constants<3>;
			using ImageConverter = ImageConverter_Base<Constants::img_input_width, Constants::img_input_height, Constants::img_input_width, Constants::img_input_height, Constants::external_img_type, Constants::img_convertCode_in_grayscale, 0>;
			//using ImageComparator = ImageComparator<3>;
			using ImageCache = ImageCache<2>;

			static inline ImageCache cache;
			static constexpr int idx_img_base = 0;
			static constexpr int idx_img_query = 1;

			static inline double horizontalDistance_base;
			static inline double height_base;
			static inline double angle_base;

		public:
			static int Reset()
			{
				cache.Clear();

				return 0;
			}

			static int PutBaseImage(void *bitmap)
			{
				cache[idx_img_base] = ImageConverter::Convert(bitmap);

				return 0;
			}

			static int PutQueryImage(void *bitmap)
			{
				cache[idx_img_query] = ImageConverter::Convert(bitmap);

				return 0;
			}

			static int PutMetadata(double horizontalDistance_base, double height_base, double angle_base)
			{
				ImageProcessor::horizontalDistance_base = horizontalDistance_base;
				ImageProcessor::height_base = height_base;
				ImageProcessor::angle_base = angle_base;

				return 0;
			}

			static double CalculateDistance()
			{
				return std::numeric_limits<double>::quiet_NaN();
			}
		};
	}
}

