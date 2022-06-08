#pragma once

#include <algorithm>


// TODO - 실행 환경에 맞는 #define을 골라 걔만 주석 해제
//#define WIN64
//#define JNI
#define IOS


namespace lpin
{
	namespace opencv
	{
		constexpr int mode = 3;

#define LPIN_OPENCV_MODE 3


		static_assert(mode != -1, "1. Set mode first");

		/*
			새 모드를 만들 때 할 일#1

			- 새 모드의 번호를 정한 다음
			  아래에 적혀 있는 availableModes 선언의 initializer 중괄호 안에 추가해 두기

			- 위에 적혀 있는 mode 선언의 initializer 자리에 방금 정한 숫자 적어 두기

			- mode 값에 따른 선택적 컴파일 기능을 사용하려는 경우(여러 모드에 대한 외부 인터페이스 코드를 같은 파일에 적어두려는 경우)
			  단어 LPIN_OPENCV_MODE에 대한 #define directive의 주석을 해제하고 mode 선언과 동일한 숫자를 적어 두기
			  (mode 선언과 #define directive는 해석 시점이 서로 다르므로 그냥 'mode'라 적으면 안 됨)
		*/

		// 현재 유효한 mode 값 목록 - 최종 수정: 2022년 6월 1일
		constexpr int availableModes[] = {
			-1, // 튜토리얼용 assertion 오류 발생. 오류 목록에서 위치를 확인하여 도움말 열람 가능
			0, // 아무 기능도 제공하지 않음(코드 정리용)
			1, // 이미지 일괄 변환
			//2, // 공간이미지 유사도 측정
			3, // 사용자 위치측위 거리 측정
			//4, // CNN image evaluation
		};

// LLVM library issue: 나중에 아래 조건을 LLVM에서도 만족하게 된 경우 수정해 주세요.
#if __cpp_lib_ranges >= 201911L
		static_assert(std::ranges::find(availableModes, mode) != std::ranges::end(availableModes),
			"Invalid mode. "
			"지정되어 있는 mode 값이 availableModes 목록에 포함되어 있지 않습니다.");
#else
		consteval bool isModeValid()
		{
			for ( int m : availableModes )
				if ( mode == m )
					return true;

			return false;
		}

		static_assert(isModeValid(),
			"Invalid mode. "
			"지정되어 있는 mode 값이 availableModes 목록에 포함되어 있지 않습니다.");
#endif


#ifdef LPIN_OPENCV_MODE
#ifdef mode
#if LPIN_OPENCV_MODE == mode
#error LPIN_OPENCV_MODE를 정의할 때는 이름 mode를 적는 대신 숫자를 직접 적어 주어야 합니다.
#endif
#else
#define mode LLONG_MAX
#if LPIN_OPENCV_MODE == mode
#error LPIN_OPENCV_MODE를 정의할 때는 이름 mode를 적는 대신 숫자를 직접 적어 주어야 합니다.
#endif
#undef mode
#endif
		static_assert(mode == LPIN_OPENCV_MODE,
			"Invalid mode. "
			"선택적 컴파일을 위한 LPIN_OPENCV_MODE 값과 템플릿 버전 선택을 위한 mode 값이 일치하지 않습니다. "
			"두 값을 일치시키거나, 선택적 컴파일 기능이 필요하지 않은 경우 단어 LPIN_OPENCV_MODE에 대한 #define directive를 주석처리해 주세요. ");
#endif
	}
}
