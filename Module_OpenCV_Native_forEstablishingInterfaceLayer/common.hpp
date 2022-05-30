#pragma once

#define WIN64
//#define JNI
//#define IOS


#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"

namespace lpin
{
	namespace opencv
	{
		/*
		* 모듈 내에서 사용하는 각종 상수 값들을 지정하는 클래스입니다.
		*/
		class Constants
		{
		private:
			Constants() = delete;
			~Constants() noexcept = delete;

		public:
			static constexpr int numberOfTrials = 10000;



			// 모듈 내부에서 사용하는 이미지의 가로 길이입니다.
			static constexpr int base_img_width = 256;

			// 모듈 내부에서 사용하는 이미지의 세로 길이입니다.
			static constexpr int base_img_height = 256;

#ifdef WIN64
			static constexpr int external_img_type = CV_8UC3;

			static constexpr int external_img_convertCode_in = cv::COLOR_BGR2GRAY;
			static constexpr int external_img_convertCode_out = cv::COLOR_GRAY2BGR;
#endif
#ifdef JNI
			// Byte array로 외부 image Data가 들어오는 경우 해당 array의 형식을 나타내는 값입니다.
			// 대부분의 경우 24b bitmap으로 들어올 것으로 예상되며 그에 맞추어 initialize해 두었습니다. 필요시 변경해야 할 수도 있습니다.
			static constexpr int external_img_type = CV_8UC4;

			static constexpr int external_img_convertCode_in = cv::COLOR_RGBA2GRAY;
			static constexpr int external_img_convertCode_out = cv::COLOR_GRAY2RGBA;
#endif
#ifdef IOS
			static constexpr int external_img_type = CV_8UC4;

			static constexpr int external_img_convertCode_in = cv::COLOR_RGBA2GRAY;
			static constexpr int external_img_convertCode_out = cv::COLOR_GRAY2RGBA;
#endif
		};

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
		};

		// Returns module's current state. See definition of lpin::opencv::GetState_StateCode for meaning of the return value.
		int GetState();

		// Returns a pointer to the char buffer specified by requestCode, or 0 if failed. See definition of lpin::opencv::GetPtrOfString_RequestCode to check available values.
		// TODO - Don't read buffer when GetState() returns 2(Busy). It may be changed during internal process. 
		char *GetPtrOfString(int requestCode);

		// Returns length(byte count) of the char buffer specified by requestCode, or 0 if failed.
		// NOTE - Since all strings are null-terminated, you might call this function when you have to make copy of them.
		int GetLengthOfString(int requestCode);

		// 0. Initialize module for task specified by taskCode. Returns non-zero if failed.
		// TODO - Put 0 when calling this function. Otherwise it will fail.
		int Initialize(int taskCode = 0);

		// 6. Start/restart a set of trials. Returns non-zero if failed.
		int Restart();

		// 1. Start/restart a set of trials. Returns non-zero if failed.
		inline int Start() { return Restart(); }

		// 2, 3. Send bitmap data to module. Returns non-zero if failed.
		int PutImage(void *bitmap, int width, int height);

		// 4. Send byte block to module. Returns non-zero if failed.
		int PutByteBlock(void *data, int length);

		// 5. Start a trial. Returns non-zero if failed.
		int Process();
	}
}