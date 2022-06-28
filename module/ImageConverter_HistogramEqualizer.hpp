//#pragma once
//
//#include "ImageConverter.hpp"
//
//namespace lpin
//{
//	namespace opencv
//	{
//		/*
//			CLAHE (Contrast Limited Adaptive Histogram Equalization) 방식을 사용하여 grayscale 이미지의 밝기 분포를 조정하는 버전입니다.
//			이 버전은 변환 도중 가로/세로 길이를 변경하지 않으며, colorspace 변환을 하지 않습니다.
//			CLAHE에 대한 자세한 내용은 https://en.wikipedia.org/wiki/Histogram_equalization 을 참고해 주세요.
//
//			주의:
//			- 변환 전 이미지는 grayscale 이미지여야 합니다. 그렇지 않은 경우 정상적으로 동작하지 않을 수 있습니다.
//			> 구체적으로는 이미지의 type 값이 CV_8UC1 또는 CV_16UC1이어야 합니다. 보편적인 Constants 설정을 사용하는 경우 grayscale 변환을 미리 해 두기만 하면 됩니다.
//
//			tile_width/height: 이미지를 여러 tile의 나열로 나눌 때 각 tile의 크기를 지정합니다. 기본값은 8x8입니다.
//			clipLimit: Histogram에 대한 clip을 수행할 한계 값을 지정합니다. 기본값은 40.0입니다.
//		*/
//		template <int tile_width = 8, int tile_height = 8, double clipLimit = 40.0>
//		class ImageConverter_HistogramEqualizer : protected ImageConverter_Base<0, 0, 0, 0, -1, -1, 0>
//		{
//		protected:
//			inline static cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(clipLimit, { tile_width, tile_height });
//		public:
//			// CLAHE 적용
//			static cv::Mat Convert(cv::Mat source)
//			{
//				cv::Mat result;
//				clahe->apply(source, result);
//				return result;
//			}
//		};
//	}
//}
