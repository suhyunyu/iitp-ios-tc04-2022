#pragma once

#include "topmost.hpp"
#include "ImageCache.hpp"
#include "ImageConverter.hpp"
#include "ImageComparator_Base.hpp"
#include "ImageEvaluator_Base.hpp"

namespace lpin
{
	namespace opencv
	{
		class ImageProcessor_Base
		{
		protected:
			~ImageProcessor_Base() noexcept = default;

			ImageProcessor_Base(const ImageProcessor_Base &) = delete;
			ImageProcessor_Base(ImageProcessor_Base &&) = delete;
			ImageProcessor_Base &operator =(const ImageProcessor_Base &) = delete;
			ImageProcessor_Base &operator =(ImageProcessor_Base &&) = delete;
		public:
			ImageProcessor_Base() = default;
		};

		template <int>
		struct ImageProcessor : ImageProcessor_Base
		{
		};

		template <>
		struct ImageProcessor<0> : ImageProcessor_Base
		{
			// 아무 기능도 제공하지 않음(코드 정리용)
		};
	}
}


static_assert(lpin::opencv::mode != -1, "3. struct ImageProcessor");


// 복사 시작 위치
#pragma once
#include "ImageProcessor_Base.hpp"

namespace lpin
{
	namespace opencv
	{
		template <>
		struct ImageProcessor<-1> : ImageProcessor_Base
		{
			/*
				새 모드를 만들 때 할 일#3

				- 새로운 버전의 ImageProcessor 정의를 적기 위한 새 .hpp 파일 만들기
				  (파일 이름은 다른 파일들 이름을 참고하여 적절히 정하기)

				- 여기서 10여 줄 위에 보이는 '복사 시작 위치'부터 그 아래 코드들을 전부 복사하여 방금 만든 새 .hpp 파일에 붙여넣기

				- 새 모드의 동작을 구성하는 Code 실행 흐름들(어느 시점에 어떤 함수가 호출될 것인지, 어떤 thread가 호출할 것인지 등)을 미리 노트해 두기

				- 노트해 둔 내용을 바탕으로, 각 파일들에 담긴 다른 버전 정의들을 참고하면서
				  새 모드의 동작에 필요한 멤버 선언 및 정의 작성하기

				> #3이라 적어 두기는 했지만 가장 오래 걸림. 적당히 구경해 보고 #4 먼저 확인
			*/
		};
	}
}

