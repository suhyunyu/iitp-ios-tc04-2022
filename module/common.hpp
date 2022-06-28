#pragma once

#include "topmost.hpp"

#if LPIN_OPENCV_MODE == 3

#include "ImageProcessor_3_DistanceCalculator.hpp"

namespace lpin
{
	namespace opencv
	{
		/*
			Note.

			common.hpp / common.cpp에는 native가 아닌 다른 환경에서 동작하는 모듈 외부 코드와의 인터페이스를 구성하기 위한 코드들이 담깁니다.

			common.hpp에는 외부에서 직접 호출할 함수들에 대한 함수 선언을,
			common.cpp에는 해당 함수들에 대한 함수 정의, 그리고 그 정의 내용물을 유효하게 실행하기 위한 부수적 선언/정의(아마도 대부분 Data 선언)를 적어 주세요.

			- 외부 코드용 프로젝트 구성시 컴파일 목록에는 .cpp 파일만 적으면 됩니다.

			- 외부에서 요구하는 기능이 그 때 그 때 다를 수 있으며, 따라서 이 두 파일은 매 응용마다 새로 작성한다고 생각하는 것이 좋습니다.
		*/

		// 모듈의 현재 상태를 나타내는 값을 return합니다.
		// 값의 의미는 enum lpin::opencv::Constants<3>::GetState_StateCode 정의를 확인해 주세요.
		int GetState();

		// 인수 requestCode로 특정되는 글자 버퍼 영역에 대한 포인터 값을 return합니다.
		// 실패시 0을 return합니다.
		// 유효한 requestCode 값은 enum lpin::opencv::Constants<3>::GetPtrOfString_RequestCode 정의를 확인해 주세요.
		// TODO - 내부 작업 도중에는 GetState() 호출식이 2(Busy)를 return할 수 있으며, 그 동안에는 버퍼 영역의 값이 변경될 수 있습니다.
		char *GetPtrOfString(int requestCode);

		// 인수 requestCode로 특정되는 글자 버퍼 영역에 담겨 있는 글자 수를 return합니다.
		// 실패시 0을 return합니다.
		// 유효한 requestCode 값은 enum lpin::opencv::Constants<3>::GetPtrOfString_RequestCode 정의를 확인해 주세요.
		// - OpenCV 모듈은 항상 문자열 끝을 0(널 문자)으로 표시해 두므로 이 함수는 memcpy() 등을 사용하여 버퍼 영역을 통째로 복사할 때 정도에만 사용됩니다.
		int GetLengthOfString(int requestCode);

		// 0. 인수 taskCode로 특정되는 작업을 수행하기 위해 모듈을 초기화합니다.
		// 실패시 0이 아닌 값을 return합니다.
		// - 현재 common.cpp 코드는 단일 작업(거리 측정)만을 위해 구성되어 있으며 해당 작업에 대한 taskCode는 0입니다.
		int Initialize(int taskCode = 0);

		// 6. 다음 세트를 진행하기 위한 정리 작업을 수행합니다.
		// 실패시 0이 아닌 값을 return합니다.
		int Restart();

		// 1. 첫 세트를 진행하기 위한 정리 작업을 수행합니다.
		// 실패시 0이 아닌 값을 return합니다.
		inline int Start() { return Restart(); }

		// 2, 3. 비트맵 데이터를 입력합니다.
		// 실패시 0이 아닌 값을 return합니다.
		// - 거리 측정에는 이미지 두 개와 메타데이터 한 덩어리가 필요합니다.
		int PutImage(void *bitmap);

		// 4. 메타데이터가 담긴 byte block을 입력합니다.
		// 실패시 0이 아닌 값을 return합니다.
		// - 거리 측정에는 이미지 두 개와 메타데이터 한 덩어리가 필요합니다.
		int PutByteBlock(void *data);

		// 5. 거리 측정을 진행합니다.
		// 실패시 0이 아닌 값을 return합니다.
		int Process();
	}
}

#endif
