#pragma once

#include "topmost.hpp"

namespace lpin
{
	namespace opencv
	{
		/*
			Note.

			ImageComparator 형식들은 기본적으로 non-static, 복사/이동 불가
			
			- ImageProcessor에서 object 생성하여 활용
			> 각 모드별 버전을 만들 때는 편의를 위해 ImageProcessor type / object에 대한 using선언 / reference 보관 가능

			- 비교 작업 도중 계산되는 중간 data들은 필요시 object 내부에 담아 둠
		*/
		class ImageComparator_Base
		{
		protected:
			ImageComparator_Base() = default;
			~ImageComparator_Base() noexcept = default;

			ImageComparator_Base(const ImageComparator_Base &) = delete;
			ImageComparator_Base(ImageComparator_Base &&) = delete;
			ImageComparator_Base &operator =(const ImageComparator_Base &) = delete;
			ImageComparator_Base &operator =(ImageComparator_Base &&) = delete;

			int state;

		public:
			int GetState() { return state; }
		};

		template <int>
		struct ImageComparator : ImageComparator_Base
		{
		};

		template <>
		struct ImageComparator<0> : ImageComparator_Base
		{
			// 아무 기능도 제공하지 않음(코드 정리용)
		};
	}
}