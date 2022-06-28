#pragma once

#include "Constants.hpp"

namespace lpin
{
	namespace opencv
	{
		/*
			비트맵 데이터 변환을 위한 static 멤버 함수들을 노출합니다.
			이 버전은 변환 전/후 이미지의 크기가 각각 일정할 때 사용하는 버전입니다. (가장 빠름)
			
			Note: ImageConverter::Convert()는 언제나 새 cv:Mat object를 만들어 return합니다.
			      Byte buffer에 비트맵 데이터를 담아 사용하는 경우 Convert() 내용물 실행이 끝난 이후 시점부터 바로 buffer를 재사용할 수 있습니다.

			source_width/height: 변환 전 이미지의 가로/세로 길이입니다. 크기가 일정하지 않은 경우 0으로 지정해 주세요.
			target_width/height: 변환 후 이미지의 가로/세로 길이입니다. 변환 도중 resize를 하지 않으려는 경우 0으로 지정해 주세요.
			source_type: 변환 전 이미지의 색상 정보 표현 방식입니다. Constants<mode>에 선언된 멤버를 참조하여 지정해 주세요. 변환 전 이미지의 형식을 해당 object에서 가져오려는 경우 음수로 지정할 수 있습니다. 이 경우 byte buffer에서 비트맵 데이터를 가져오는 것이 제한됩니다.
			convertCode: Colorspace 변환 방식을 나타내는 값입니다. Constants<mode>에 선언된 멤버를 참조하여 지정해 주세요. Colorspace를 변환하지 않으려는 경우 음수로 지정해 주세요.
			interpolation: Resize 방식을 나타내는 값입니다. Constants<mode>에 선언된 멤버를 참조하여 지정해 주세요.
		*/
		template <int source_width_, int source_height_,
			int target_width_, int target_height_,
			int source_type_, int convertCode_, int interpolation_>
		class ImageConverter_Base
		{
		protected:
			static_assert(source_width_ >= 0 && source_height_ >= 0 && target_width_ >= 0 && target_height_ >= 0,
				"Invalid size parameter found during instantiating ImageConverter. "
				"새 버전의 ImageConverter를 사용할 때 template 인수로 지정하는 이미지 가로/세로 길이는 음수가 아니어야 합니다. "
				"코드의 이 부분(static_assert 선언이 있는 부분) 말고 가장 최근에 ImageConverter 인스턴스화를 시도한 코드를 확인해 주세요.");

			static_assert((source_width_ > 0) == (source_height_ > 0) && (target_width_ > 0) == (target_height_ > 0),
				"Invalid size parameter found during instantiating ImageConverter. "
				"새 버전의 ImageConverter를 사용할 때 template 인수로 지정하는 이미지 가로/세로 길이는 둘 다 양수거나 둘 다 0이어야 합니다. "
				"코드의 이 부분(static_assert 선언이 있는 부분) 말고 가장 최근에 ImageConverter 인스턴스화를 시도한 코드를 확인해 주세요.");

			ImageConverter_Base() = delete;
			ImageConverter_Base(const ImageConverter_Base &) = delete;
			ImageConverter_Base(ImageConverter_Base &&) = delete;
			~ImageConverter_Base() = delete;

		public:
			static constexpr int source_width = source_width_;

			static constexpr int source_height = source_height_;

			// Resize를 안 하기 위해 변환 후 이미지의 가로 길이를 0으로 지정해 둔 경우 source_width 값을 그대로 인용
			static constexpr int target_width = target_width_ ? target_width_ : source_width_;

			// Resize를 안 하기 위해 변환 후 이미지의 세로 길이를 0으로 지정해 둔 경우 source_height 값을 그대로 인용
			static constexpr int target_height = target_height_ ? target_height_ : source_height_;

			static constexpr int source_type = source_type_;

			static constexpr int convertCode = convertCode_;

			static constexpr int interpolation = interpolation_;

		protected:
			/*
				변환 관련 note

				Stretch 및 레터박스 적용 안 함. 따라서 변환 도중 크기를 키울 때도 종횡비 차이에 따라 crop이 발생할 수 있음
				Crop은 항상 중앙을 기준으로 두고 귀퉁이를 자름
				항상 crop을 resize보다 먼저 함 - 극단적인 종횡비 변화에 취약하나 resize 속도가 빠른 편
			*/

			// convertCode가 음수가 아닌 경우 colorspace 변환 필요
			static constexpr bool needToConvertColorspace = convertCode >= 0;

			// 종횡비가 서로 다른 경우 crop 필요
			static constexpr bool needToCrop = (long long)source_width * target_height != (long long)target_width * source_height;

			// 종횡비가 더 작아질(위아래로 길어질) 예정인 경우 좌우를 crop해야 함
			static constexpr bool cropLeftAndRight = (long long)source_width * target_height > (long long)target_width * source_height;

			// Crop 이후의 이미지 가로 길이: 상하 crop인 경우 원본과 동일, 좌우 crop인 경우 전/후 세로 길이 비율을 사용하여 역산
			static constexpr int width_afterCrop = cropLeftAndRight ? (long long)target_width * source_height / target_height : source_width;

			// Crop 이후의 이미지 세로 길이: 좌우 crop인 경우 원본과 동일, 상하 crop인 경우 전/후 가로 길이 비율을 사용하여 역산
			static constexpr int height_afterCrop = cropLeftAndRight ? source_height : (long long)target_height * source_width / target_width;

			static constexpr int x0_crop = ( source_width - width_afterCrop ) / 2;
			static constexpr int x1_crop = x0_crop + width_afterCrop;
			static constexpr int y0_crop = ( source_height - height_afterCrop ) / 2;
			static constexpr int y1_crop = y0_crop + height_afterCrop;

			// Crop 이후에는 종횡비가 동일해지므로 가로 길이만 보고 resize 여부 판단 가능
			static constexpr bool needToResize = width_afterCrop != target_width;

		public:
			// Crop -> ConvertColorspace -> Resize
			static cv::Mat Convert(cv::Mat origin) requires (needToConvertColorspace && needToCrop && needToResize)
			{
				cv::Mat recolored;
				cv::Mat resized;

				cv::cvtColor(origin(cv::Range(y0_crop, y1_crop), cv::Range(x0_crop, x1_crop)), recolored, convertCode);

				cv::resize(recolored, resized, { target_width, target_height }, 0, 0, interpolation);

				return resized;
			}

			// Crop -> ConvertColorspace
			static cv::Mat Convert(cv::Mat source) requires (needToConvertColorspace && needToCrop && !needToResize)
			{
				cv::Mat recolored;

				cv::cvtColor(source(cv::Range(y0_crop, y1_crop), cv::Range(x0_crop, x1_crop)), recolored, convertCode);

				return recolored;
			}

			// ConvertColorspace -> Resize
			static cv::Mat Convert(cv::Mat source) requires (needToConvertColorspace && !needToCrop && needToResize)
			{
				cv::Mat recolored;
				cv::Mat resized;

				cv::cvtColor(source, recolored, convertCode);

				cv::resize(recolored, resized, { target_width, target_height }, 0, 0, interpolation);

				return resized;
			}

			// ConvertColorspace only
			static cv::Mat Convert(cv::Mat source) requires (needToConvertColorspace && !needToCrop && !needToResize)
			{
				cv::Mat recolored;

				cv::cvtColor(source, recolored, convertCode);

				return recolored;
			}

			// Crop -> Resize
			static cv::Mat Convert(cv::Mat source) requires (!needToConvertColorspace && needToCrop && needToResize)
			{
				cv::Mat resized;

				cv::resize(source(cv::Range(y0_crop, y1_crop), cv::Range(x0_crop, x1_crop)), resized, { target_width, target_height }, 0, 0, interpolation);

				return resized;
			}

			// Crop only
			static cv::Mat Convert(cv::Mat source) requires (!needToConvertColorspace && needToCrop && !needToResize)
			{
				return source(cv::Range(y0_crop, y1_crop), cv::Range(x0_crop, x1_crop)).clone();
			}

			// Resize only
			static cv::Mat Convert(cv::Mat source) requires (!needToConvertColorspace && !needToCrop && needToResize)
			{
				cv::Mat resized;

				cv::resize(source, resized, { target_width, target_height }, 0, 0, interpolation);

				return resized;
			}

			// Clone only
			static cv::Mat Convert(cv::Mat source) requires (!needToConvertColorspace && !needToCrop && !needToResize)
			{
				return source.clone();
			}

			static cv::Mat Convert(void *ptr) requires (source_type >= 0)
			{
                //std::cout<<"Convert ptr :"<<sizeof(ptr)<<"\n";
				return Convert(cv::Mat{source_height, source_width, source_type, ptr});
			}

			[[deprecated("변환 전 이미지의 크기가 고정되어 있는 버전의 ImageConverter를 사용하고 있습니다. Convert(void *) 버전을 대신 사용하세요. 인수로 담은 width, height 값은 무시됩니다.")]]
			static cv::Mat Convert(void *ptr, int width, int height) requires (source_type >= 0)
			{
				return Convert(ptr);
			}
		};

		/*
			비트맵 데이터 변환을 위한 static 멤버 함수들을 노출합니다.
			이 버전은 변환 전 이미지의 크기가 일정하지 않을 때 사용하는 버전입니다. (가장 느림)

			Note: ImageConverter::Convert()는 언제나 새 cv:Mat object를 만들어 return합니다.
				  Byte buffer에 비트맵 데이터를 담아 사용하는 경우 Convert() 내용물 실행이 끝난 이후 시점부터 바로 buffer를 재사용할 수 있습니다.

			target_width/height: 변환 후 이미지의 가로/세로 길이입니다. 변환 도중 resize를 하지 않으려는 경우 0으로 지정해 주세요.
			source_type: 변환 전 이미지의 색상 정보 표현 방식입니다. Constants<mode>에 선언된 멤버를 참조하여 지정해 주세요. 변환 전 이미지의 형식을 해당 object에서 가져오려는 경우 음수로 지정할 수 있습니다. 이 경우 byte buffer에서 비트맵 데이터를 가져오는 것이 제한됩니다.
			convertCode: Colorspace 변환 방식을 나타내는 값입니다. Constants<mode>에 선언된 멤버를 참조하여 지정해 주세요. Colorspace를 변환하지 않으려는 경우 음수로 지정해 주세요.
			interpolation: Resize 방식을 나타내는 값입니다. Constants<mode>에 선언된 멤버를 참조하여 지정해 주세요.
		*/
		template <int target_width_, int target_height_,
			int source_type_, int convertCode_, int interpolation_> requires (target_width_ > 0 && target_height_ > 0)
		class ImageConverter_Base<0, 0, target_width_, target_height_, source_type_, convertCode_, interpolation_>
		{
		protected:
			static_assert(target_width_ >= 0 && target_height_ >= 0,
				"Invalid size parameter found during instantiating ImageConverter. "
				"새 버전의 ImageConverter를 사용할 때 template 인수로 지정하는 이미지 가로/세로 길이는 음수가 아니어야 합니다. "
				"코드의 이 부분(static_assert 선언이 있는 부분) 말고 가장 최근에 ImageConverter 인스턴스화를 시도한 코드를 확인해 주세요.");

			static_assert((target_width_ > 0) == (target_height_ > 0),
				"Invalid size parameter found during instantiating ImageConverter. "
				"새 버전의 ImageConverter를 사용할 때 template 인수로 지정하는 이미지 가로/세로 길이는 둘 다 양수거나 둘 다 0이어야 합니다. "
				"코드의 이 부분(static_assert 선언이 있는 부분) 말고 가장 최근에 ImageConverter 인스턴스화를 시도한 코드를 확인해 주세요.");

			ImageConverter_Base() = delete;
			ImageConverter_Base(const ImageConverter_Base &) = delete;
			ImageConverter_Base(ImageConverter_Base &&) = delete;
			~ImageConverter_Base() = delete;

		public:
			static constexpr int source_width = 0;

			static constexpr int source_height = 0;

			static constexpr int target_width = target_width_;

			static constexpr int target_height = target_height_;

			static constexpr int source_type = source_type_;

			static constexpr int convertCode = convertCode_;

			static constexpr int interpolation = interpolation_;

		protected:
			/*
				변환 관련 note

				Stretch 및 레터박스 적용 안 함. 따라서 변환 도중 크기를 키울 때도 종횡비 차이에 따라 crop이 발생할 수 있음
				Crop은 항상 중앙을 기준으로 두고 귀퉁이를 자름
				항상 crop을 resize보다 먼저 함 - 극단적인 종횡비 변화에 취약하나 resize 속도가 빠른 편
			*/

			// convertCode가 음수가 아닌 경우 colorspace 변환 필요
			static constexpr bool needToConvertColorspace = convertCode >= 0;

		public:
			// Crop(opt.) -> ConvertColorspace -> Resize(opt.)
			static cv::Mat Convert(cv::Mat source) requires needToConvertColorspace
			{
				cv::Mat recolored;
				cv::Mat resized;

				int source_width = source.cols;
				int source_height = source.rows;

				// 종횡비가 서로 다른 경우 crop 필요
				if ( (long long)source_width * target_height != (long long)target_width * source_height )
				{
					// 종횡비가 더 작아질(위아래로 길어질) 예정인 경우 좌우를 crop해야 함
					bool cropLeftAndRight = (long long)source_width * target_height > (long long)target_width * source_height;

					// Crop 이후의 이미지 가로 길이: 상하 crop인 경우 원본과 동일, 좌우 crop인 경우 전/후 세로 길이 비율을 사용하여 역산
					int width_afterCrop = cropLeftAndRight ? (long long)target_width * source_height / target_height : source_width;

					// Crop 이후의 이미지 세로 길이: 좌우 crop인 경우 원본과 동일, 상하 crop인 경우 전/후 가로 길이 비율을 사용하여 역산
					int height_afterCrop = cropLeftAndRight ? source_height : (long long)target_height * source_width / target_width;

					int x0_crop = (source_width - width_afterCrop) / 2;
					int x1_crop = x0_crop + width_afterCrop;
					int y0_crop = (source_height - height_afterCrop) / 2;
					int y1_crop = y0_crop + height_afterCrop;


					// Crop 이후에는 종횡비가 동일해지므로 가로 길이만 보고 resize 여부 판단 가능
					if ( width_afterCrop != target_width )
					{
						cv::cvtColor(source(cv::Range(y0_crop, y1_crop), cv::Range(x0_crop, x1_crop)), recolored, convertCode);
						cv::resize(recolored, resized, { target_width, target_height }, 0, 0, interpolation);
					}
					else
					{
						cv::cvtColor(source(cv::Range(y0_crop, y1_crop), cv::Range(x0_crop, x1_crop)), resized, convertCode);
					}

				}
				else
				{
					// Resize 여부 확인
					if ( source_width != target_width || source_height != target_height )
					{
						cv::cvtColor(source, recolored, convertCode);

						cv::resize(recolored, resized, { target_width, target_height }, 0, 0, interpolation);
					}
					else
					{
						cv::cvtColor(source, resized, convertCode);
					}
				}

				return resized;
			}

			// Crop(opt.) -> Resize(opt.)
			static cv::Mat Convert(cv::Mat source) requires (!needToConvertColorspace)
			{
				cv::Mat resized;

				int source_width = source.cols;
				int source_height = source.rows;

				// 종횡비가 서로 다른 경우 crop 필요
				if ( (long long)source_width * target_height != (long long)target_width * source_height )
				{
					// 종횡비가 더 작아질(위아래로 길어질) 예정인 경우 좌우를 crop해야 함
					bool cropLeftAndRight = (long long)source_width * target_height > (long long)target_width * source_height;

					// Crop 이후의 이미지 가로 길이: 상하 crop인 경우 원본과 동일, 좌우 crop인 경우 전/후 세로 길이 비율을 사용하여 역산
					int width_afterCrop = cropLeftAndRight ? (long long)target_width * source_height / target_height : source_width;

					// Crop 이후의 이미지 세로 길이: 좌우 crop인 경우 원본과 동일, 상하 crop인 경우 전/후 가로 길이 비율을 사용하여 역산
					int height_afterCrop = cropLeftAndRight ? source_height : (long long)target_height * source_width / target_width;

					int x0_crop = (source_width - width_afterCrop) / 2;
					int x1_crop = x0_crop + width_afterCrop;
					int y0_crop = (source_height - height_afterCrop) / 2;
					int y1_crop = y0_crop + height_afterCrop;


					// Crop 이후에는 종횡비가 동일해지므로 가로 길이만 보고 resize 여부 판단 가능
					if ( width_afterCrop != target_width )
					{
						cv::resize(source(cv::Range(y0_crop, y1_crop), cv::Range(x0_crop, x1_crop)), resized, { target_width, target_height }, 0, 0, interpolation);
					}
					else
					{
						resized = source(cv::Range(y0_crop, y1_crop), cv::Range(x0_crop, x1_crop)).clone();
					}

				}
				else
				{
					// Resize 여부 확인
					if ( source_width != target_width || source_height != target_height )
					{
						cv::resize(source, resized, { target_width, target_height }, 0, 0, interpolation);
					}
					else
					{
						resized = source.clone();
					}
				}

				return resized;
			}

			static cv::Mat Convert(void *ptr, int width, int height) requires (source_type >= 0)
			{
				return Convert(cv::Mat{ height, width, source_type, ptr });
			}
		};


		/*
			비트맵 데이터 변환을 위한 static 멤버 함수들을 노출합니다.
			이 버전은 변환 전 이미지의 크기가 일정하지 않으며 변환 도중 resize를 하지 않을 때 사용하는 버전입니다. (Resize 하는 버전보다는 빠르나 그 용도가 한정됨)

			Note: ImageConverter::Convert()는 언제나 새 cv:Mat object를 만들어 return합니다.
				  Byte buffer에 비트맵 데이터를 담아 사용하는 경우 Convert() 내용물 실행이 끝난 이후 시점부터 바로 buffer를 재사용할 수 있습니다.

			source_type: 변환 전 이미지의 색상 정보 표현 방식입니다. Constants<mode>에 선언된 멤버를 참조하여 지정해 주세요. 변환 전 이미지의 형식을 해당 object에서 가져오려는 경우 음수로 지정할 수 있습니다. 이 경우 byte buffer에서 비트맵 데이터를 가져오는 것이 제한됩니다.
			convertCode: Colorspace 변환 방식을 나타내는 값입니다. Constants<mode>에 선언된 멤버를 참조하여 지정해 주세요. Colorspace를 변환하지 않으려는 경우 음수로 지정해 주세요.
			interpolation_notUsed: Resize 방식을 나타내는 값입니다. 이 버전에서는 사용하지 않습니다. 아무 값이나 지정해도 됩니다.
		*/
		template <int source_type_, int convertCode_, int interpolation_notUsed>
		class ImageConverter_Base<0, 0, 0, 0, source_type_, convertCode_, interpolation_notUsed>
		{
		protected:
			ImageConverter_Base() = delete;
			ImageConverter_Base(const ImageConverter_Base &) = delete;
			ImageConverter_Base(ImageConverter_Base &&) = delete;
			~ImageConverter_Base() = delete;

		public:
			static constexpr int source_width = 0;

			static constexpr int source_height = 0;

			static constexpr int target_width = 0;

			static constexpr int target_height = 0;

			static constexpr int source_type = source_type_;

			static constexpr int convertCode = convertCode_;

			static constexpr int interpolation = -1;

		protected:
			/*
				변환 관련 note

				Stretch 및 레터박스 적용 안 함. 따라서 변환 도중 크기를 키울 때도 종횡비 차이에 따라 crop이 발생할 수 있음
				Crop은 항상 중앙을 기준으로 두고 귀퉁이를 자름
				항상 crop을 resize보다 먼저 함 - 극단적인 종횡비 변화에 취약하나 resize 속도가 빠른 편
			*/

			// convertCode가 음수가 아닌 경우 colorspace 변환 필요
			static constexpr bool needToConvertColorspace = convertCode >= 0;

		public:
			// ConvertColorspace only
			static cv::Mat Convert(cv::Mat source) requires needToConvertColorspace
			{
				cv::Mat recolored;

				cv::cvtColor(source, recolored, convertCode);

				return recolored;
			}

			// Clone only
			static cv::Mat Convert(cv::Mat source) requires (!needToConvertColorspace)
			{
				return source.clone();
			}

			static cv::Mat Convert(void *ptr, int width, int height) requires (source_type >= 0)
			{
				return Convert(cv::Mat{ height, width, source_type, ptr });
			}
		};


		/*
			임의의 크기를 가진 외부 이미지를 가져올 때 사용할 수 있는, Constants<mode> 내용을 바탕으로 설정하는 가장 기초적인 버전의 ImageConverter입니다.

			img_internal_width/height: 내부 이미지의 가로/세로 길이입니다. 변환 도중 resize를 하지 않으려는 경우 0으로 지정해 주세요.
			convertToGrayscale: Grayscale 이미지로 변환하려는 경우 true입니다.
		*/
		template <int img_internal_width, int img_internal_height, bool convertToGrayscale>
		using DefaultImageConverter_In = ImageConverter_Base<0, 0, img_internal_width, img_internal_height,
			Constants<mode>::external_img_type,	convertToGrayscale ? Constants<mode>::img_convertCode_in_grayscale : Constants<mode>::img_convertCode_in, Constants<mode>::interpolation>;

		/*
			모듈 내부 이미지를 외부로 보낼 때 사용할 수 있는, Constants<mode> 내용을 바탕으로 설정하는 가장 기초적인 버전의 ImageConverter입니다.

			img_internal_width/height: 내부 이미지의 가로/세로 길이입니다. 외부 이미지를 가져올 때 resize를 하지 않은 경우 0으로 지정해 주세요.
			img_external_width/height: 변환 후 외부로 보낼 이미지의 가로/세로 길이입니다. 변환 도중 resize를 하지 않으려는 경우 0으로 지정해 주세요.
			isSourceGrayscale: 외부 이미지를 가져올 때 grayscale로 변환한 경우 true입니다.
		*/
		template <int img_internal_width, int img_internal_height, int img_external_width, int img_external_height, bool isSourceGrayscale>
		using DefaultImageConverter_Out = ImageConverter_Base<img_internal_width, img_internal_height, img_external_width, img_external_height,
			-1, isSourceGrayscale ? Constants<mode>::img_convertCode_out_grayscale : Constants<mode>::img_convertCode_out, Constants<mode>::interpolation>;






		namespace for_tutorial
		{
			static_assert(mode != -1, "4. class ImageConverter_Base");

			/*
				새 모드를 만들 때 할 일#4

				- 우선 아래에 나열된 using 선언 예시들을 확인

				- 새 모드 동작 도중 이미지 변환이 언제, 어떻게 발생하는지 미리 노트해 두기

				- 노트해 둔 내용을 바탕으로, 새 모드용 ImageProcessor 정의에 해당 모드에서 사용할 ImageConverter들에 대한 using 선언 적어 두기
				  (using 선언을 여러 번 적어도 성능(속도, 용량 등)에는 거의 영향을 주지 않으므로 용도별로 각각 선언해 두는 것이 편함)

				> Visual Studio, xCode, Android Studio들은 모두 < > 안 내용을 적을 때 적절한 툴팁 설명을 보여줄 수 있음. 해당 툴팁을 보여주는 단축키를 확인하여 눌러가며 적으면 편함
			*/

			/*
				입력용 세팅 예시#1: 1920x1080 기본 포맷 외부 이미지 -> 256x256 grayscaled 이미지
			*/
			using TutorialImageConverter_FixedSize = ImageConverter_Base<
				1920, 1080,
				256, 256,
				Constants<mode>::external_img_type,
				Constants<mode>::img_convertCode_in_grayscale,
				Constants<mode>::interpolation>;


			/*
				입력용 세팅 예시#2: ? x ? 기본 포맷 외부 이미지 -> 256x256 RGB 포맷 이미지
			*/
			using TutorialImageConverter_UnknownInputSize = ImageConverter_Base<
				0, 0,
				256, 256,
				Constants<mode>::external_img_type,
				Constants<mode>::img_convertCode_in,
				Constants<mode>::interpolation>;

			// 좀 더 쉬운 방법
			using TutorialImageConverter_UnknownInputSize_Alt = DefaultImageConverter_In<256, 256, false>;

			/*
				입력용 세팅 예시#3: ? x ? 기본 포맷 외부 이미지 -> 동일 크기 grayscaled 이미지
			*/
			using TutorialImageConverter_SameSize = ImageConverter_Base<
				0, 0,
				0, 0,
				Constants<mode>::external_img_type,
				Constants<mode>::img_convertCode_in_grayscale,
				Constants<mode>::interpolation>;

			// 좀 더 쉬운 방법
			using TutorialImageConverter_SameSize_Alt = DefaultImageConverter_In<0, 0, true>;

			/*
				출력용 세팅 예시#1: 256x256 RGB 이미지 -> 동일 크기 기본 포맷 이미지
			*/
			using TutorialImageConverter_FixedSize2 = ImageConverter_Base<
				256, 256,
				0, 0,
				Constants<mode>::internal_img_type, // -1 가능
				Constants<mode>::img_convertCode_out,
				Constants<mode>::interpolation>;

			// 좀 더 쉬운 방법
			using TutorialImageConverter_FixedSize2_Alt = DefaultImageConverter_Out<256, 256, 0, 0, false>;

			/*
				출력용 세팅 예시#2: ? x ? grayscaled 이미지 -> 동일 크기 기본 포맷 이미지
			*/
			using TutorialImageConverter_SameSize2 = ImageConverter_Base<
				0, 0,
				0, 0,
				Constants<mode>::internal_img_type_grayscale, // -1 가능
				Constants<mode>::img_convertCode_out_grayscale,
				Constants<mode>::interpolation>;

			// 좀 더 쉬운 방법
			using TutorialImageConverter_SameSize2_Alt = DefaultImageConverter_Out<0, 0, 0, 0, false>;
		};
	}
}
