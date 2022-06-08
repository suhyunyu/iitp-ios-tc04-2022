#pragma once

#include "topmost.hpp"
#include "opencv2/imgproc.hpp"

// LLVM library issue: 나중에 아래 조건을 LLVM에서도 만족하게 된 경우 수정해 주세요.
#if __cpp_lib_concepts >= 202002L
#include <concepts>
#else
#include <type_traits>
#endif

namespace lpin
{
	namespace opencv
	{
		struct Constants_Base
		{
			Constants_Base() = delete;
			Constants_Base(const Constants_Base &) = delete;
			Constants_Base(Constants_Base &&) = delete;
			~Constants_Base() = delete;

			/*
				Note.
				
				img_type은 각 픽셀들에 대한 색상 정보를 어떤 형식으로 표현할 것인지를 결정합니다.

				예를 들어, CV_8UC1은 각 픽셀을 unsigned 8b 형식(예: unsigned char) 값 하나(1채널)로 표현함을 의미합니다.
				convertCode를 변경하려는 경우(예: Colorspace를 RGB 대신 RGBA, CSV 등을 사용하려는 경우) 그에 맞추어 img_type들도 변경해 주어야 하며,
				이 모듈에서는 convertCode와 img_type 사이의 호환성에 대해 직접 검사하지 않습니다.

				모듈 외부 라이브러리(Android 등)는 CV_8UC1과 같은 1채널 방식이나 CV_16FC4와 같은 floating point 형식을 지원하지 않을 수 있습니다.
				따라서 모듈 외부로 이미지 데이터를 보낼 때는 아래에 적어 둔 external_img_type 형식으로 변환하여 보내는 것을 권장합니다.

				자세한 내용은 각 단어 또는 이름에 대한 정의/선언에 있는 설명을 참고해 주세요.
			*/
			
			static constexpr int internal_img_type_grayscale = CV_8UC1;
			static constexpr int internal_img_type = CV_8UC3;

			static constexpr int interpolation = cv::InterpolationFlags::INTER_LINEAR;

#ifdef WIN64
			static constexpr int external_img_type = CV_8UC3;

			static constexpr int img_convertCode_in = cv::ColorConversionCodes::COLOR_BGR2RGB;
			static constexpr int img_convertCode_in_grayscale = cv::ColorConversionCodes::COLOR_BGR2GRAY;

			static constexpr int img_convertCode_out = cv::ColorConversionCodes::COLOR_RGB2BGR;
			static constexpr int img_convertCode_out_grayscale = cv::ColorConversionCodes::COLOR_GRAY2BGR;
#endif
#ifdef JNI
			// 설명에는 ARGB라 적혀 있으나 실제로는 RGBA 순으로 나열되어 있음
			static constexpr int external_img_type = CV_8UC4;

			static constexpr int img_convertCode_in = cv::ColorConversionCodes::COLOR_RGBA2RGB;
			static constexpr int img_convertCode_in_grayscale = cv::ColorConversionCodes::COLOR_RGBA2GRAY;

			static constexpr int img_convertCode_out = cv::ColorConversionCodes::COLOR_RGB2RGBA;
			static constexpr int img_convertCode_out_grayscale = cv::ColorConversionCodes::COLOR_GRAY2RGBA;
#endif
#ifdef IOS
			// 설명에는 ARGB라 적혀 있으나 실제로는 RGBA 순으로 나열되어 있음
			static constexpr int external_img_type = CV_8UC4;

			static constexpr int img_convertCode_in = cv::ColorConversionCodes::COLOR_RGBA2RGB;
			static constexpr int img_convertCode_in_grayscale = cv::ColorConversionCodes::COLOR_RGBA2GRAY;

			static constexpr int img_convertCode_out = cv::ColorConversionCodes::COLOR_RGB2RGBA;
			static constexpr int img_convertCode_out_grayscale = cv::ColorConversionCodes::COLOR_GRAY2RGBA;
#endif
		};

		template <int>
		struct Constants : Constants_Base
		{
			~Constants() = delete;
		};

// LLVM library issue: 나중에 아래 조건을 LLVM에서도 만족하게 된 경우 수정해 주세요.
#if __cpp_lib_concepts >= 202002L

		template <class T>
		concept RequirementsForConstants = requires {
			{ T::internal_img_type_grayscale } -> std::convertible_to<int>;
			{ T::internal_img_type } -> std::convertible_to<int>;
			{ T::interpolation } -> std::convertible_to<int>;
			{ T::external_img_type } -> std::convertible_to<int>;
			{ T::img_convertCode_in_grayscale } -> std::convertible_to<int>;
			{ T::img_convertCode_out_grayscale } -> std::convertible_to<int>;
			{ T::img_convertCode_in } -> std::convertible_to<int>;
			{ T::img_convertCode_out } -> std::convertible_to<int>;
		} && std::derived_from<T, Constants_Base>;

#else

		template <class T>
		concept CanConvertToInt = std::is_convertible_v<T, int> && requires {
			static_cast<int>(std::declval<T>());
		};

		template <class T>
		concept DerivedFromConstants_Base = std::is_base_of_v<Constants_Base, T> &&
			std::is_convertible_v<const volatile T *, const volatile Constants_Base *>;

		template <class T>
		concept RequirementsForConstants = requires {
			{ T::internal_img_type_grayscale } -> CanConvertToInt;
			{ T::internal_img_type } -> CanConvertToInt;
			{ T::interpolation } -> CanConvertToInt;
			{ T::external_img_type } -> CanConvertToInt;
			{ T::img_convertCode_in_grayscale } -> CanConvertToInt;
			{ T::img_convertCode_out_grayscale } -> CanConvertToInt;
			{ T::img_convertCode_in } -> CanConvertToInt;
			{ T::img_convertCode_out } -> CanConvertToInt;
		} && DerivedFromConstants_Base<T>;

#endif









		static_assert(mode != -1, "2. struct Constants");

		template <>
		struct Constants<-1> : Constants_Base
		{
			~Constants() = delete;

			/*
				새 모드를 만들 때 할 일#2

				- 여기 보이는 <-1> 버전 Constants 정의 전체를 복사해서 이 아래에 (같은 scope에, 파일 맨 아래에 보이는 static_assert 선언보다는 위에) 붙여넣고
				  <-1> 대신 새로 정한 모드 값을 적어 새 버전 정의 만들기

				- 새 모드에서 야기할 기초적인 Data 흐름들(초기 설정용 Data 입력, 이미지 및 metadata 입력/출력 등)을 미리 노트해 두기

				- 노트해 둔 내용을 바탕으로, 아래 나열된 다른 버전 정의들을 참고하면서
				  새 작업에서 사용할 ImageConverter, ImageComparator, ImageProcessor 등을 커스터마이즈할 때 활용할 상수 값들에 대한 적당한 선언을 적어 두기
				  (적을 때 static 붙여 두어야 함. 그리고 가급적 constexpr 붙여 두는 것을 권장)

				> 파괴자 명시적 삭제는 안 해도 되지만 안 하면 컴파일러 옵션에 따라 경고를 낼 수 있으므로 삭제 권장

				> 모듈 외부에서 신경쓸 필요 없는 상수 값들(예: 내부 cache의 몇 번째 칸을 무슨 용도로 쓸 것인지)에 대한 선언들은 ImageProcessor 정의, 또는 외부 인터페이스 구성용 .cpp 파일 안에 적어 두는 것을 권장
			*/
		};



		template <>
		struct Constants<0> : Constants_Base
		{
			~Constants() = delete;

			// 아무 기능도 제공하지 않음(코드 정리용)
		};



		template <>
		struct Constants<1> : Constants_Base
		{
			~Constants() = delete;

			// 변환 결과 이미지의 가로 길이
			static constexpr int img_output_width = 256;

			// 변환 결과 이미지의 세로 길이
			static constexpr int img_output_height = 256;

			// 변환시 이미지를 grayscale화할 것인지 여부
			static constexpr bool convertToGrayscale = true;

			// 외부 포맷으로의 변환을 생략할 것인지 여부(모듈 코드에서 직접 파일 출력을 진행하는 경우 생략 권장)
			static constexpr bool skipOutwardConversion = true;

			// 변환 결과 이미지가 준비되어 있지 않을 때 GetNextResult()를 호출한 thread의 실행 흐름을 막을지 여부(막지 않는다면 이미지 미준비시 id 값이 -1인 
			static constexpr bool waitForResultIfNotReady = true;

			// 변환에 사용할 내부 스레드 수
			static constexpr int numberOfWorkerThreads = 8;

			// 이미지별 일련번호를 다룰 때 사용하는 형식. 요구조건 있음. int 형식이 너무 작다 싶을 때는 (unsigned) long long 형식으로 지정 권장
			using ID_t = int;
		};

		template <class ID_t>
		concept RequirementsForConstants_1_ID_t =
			( requires (ID_t id) { id = -1; } || Constants<1>::waitForResultIfNotReady ) &&
			requires (ID_t id) { ++id; };

		static_assert(RequirementsForConstants_1_ID_t<Constants<1>::ID_t>,
			"Definition of Constants<1>::ID_t couldn\'t fease requirements. "
			"Constants<1>::ID_t에 대한 using 선언이 적절하지 않습니다. "
			"Constants<1>::ID_t 형식은 1씩 더하는 것이 가능한 형식이어야 하며 "
			"Constants<1>::waitForResultIfNotReady == false인 경우 literal -1에 해당하는 값을 다룰 수 있는 형식이어야 합니다.");




		template <>
		struct Constants<3> : Constants_Base
		{
			~Constants() = delete;
			
			enum GetState_StateCode : int
			{
				Not_Defined = 0,
				Busy = 1,
				Initialized = 2,
				WaitFor_BaseImage = 3,
				WaitFor_QueryImage = 4,
				WaitFor_ByteBlock = 5,
				Ready_ToStartTrial = 6,
				Completed = Initialized,
			};

			enum GetPtrOfString_RequestCode : int
			{
				Gimme_Status = 0,
				Gimme_LogLine = 1,
				Gimme_FullResult = 2,
				Gimme_Todo = 3,
			};

			static constexpr int numberOfTrials = 10000;

			static constexpr int img_input_width = 256;
			static constexpr int img_input_height = 256;
		};




		static_assert(RequirementsForConstants<Constants<mode>>,
			"Definition of struct Constants couldn\'t fease requirements. "
			"현재 모드에 맞는 버전의 struct Constants<> 정의가 필수 조건을 만족하지 않습니다. "
			"Constants_Base 형식을 기본 형식으로 잘 지정하고 있는지, Constants_Base 형식 소속 이름을 부적절하게 가리고 있지는 않은지 확인해 주세요.");
	}
}