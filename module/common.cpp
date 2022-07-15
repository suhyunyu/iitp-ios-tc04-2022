#include "topmost.hpp"

#if LPIN_OPENCV_MODE == 3

#include "common.hpp"

#include <cstdio>
#include <cmath>
#include <limits>

using Constants = lpin::opencv::Constants<3>;
using ImageProcessor = lpin::opencv::ImageProcessor<3>;


static Constants::GetState_StateCode state = Constants::Not_Defined;
static Constants::GetState_StateCode state_old = Constants::Not_Defined;

static union
{
	void *raw;
	double(*parr)[];

	static_assert(sizeof(void *) == sizeof(double(*)[]), "Pointer size mismatch.");
} ptr_metadataBlock;

static constexpr int idx_metadata_horizontalDistance_base = 0;
static constexpr int idx_metadata_height_base = 1;
static constexpr int idx_metadata_angle_base = 2;
static constexpr int idx_metadata_actualDistance = 3;


static int count_trials;


static char buffer_status[256];
static char buffer_logLine[256];
static char buffer_fullResult[128 * Constants::numberOfTrials + 1];
static char buffer_todo[256];

static int length_status;
static int length_logLine;
static int length_fullResult;
static int length_todo;


// std::sprintf() 경고 회피를 위한 전처리기화
#define Format_Status_Initialize_Failed				"Initialize() failed."
#define Format_Status_Initialize_Successful			"Initialize() success. Ready to start."
#define Format_Status_Start_Failed					"Start/Restart() failed."
#define Format_Status_Start_Successful				"Start/Restart() success. Waiting for First Base Image..."
#define Format_Status_PutImage_Failed				"Trial#%.4d: PutImage() failed."
#define Format_Status_PutImage_First_Successful		"Trial#%.4d: PutImage() success. Waiting for Query Image..."
#define Format_Status_PutImage_Second_Successful	"Trial#%.4d: PutImage() success. Waiting for Byte Block..."
#define Format_Status_PutByteBlock_Failed			"Trial#%.4d: PutByteBlock() failed."
#define Format_Status_PutByteBlock_Successful		"Trial#%.4d: PutByteBlock() success. Ready to run Process()."
#define Format_Status_Process_Failed				"Trial#%.4d: Process() failed."
#define Format_Status_Process_Successful			"Trial#%.4d: Process() success. Waiting for Next Base Image..."
#define Format_Status_Process_Last_Successful		"Trial#%.4d: Process() success. Test complete."

#define Format_Todo_After_Initialize				"Press [Start Test] button"
#define Format_Todo_After_Start						"Please wait..."
#define Format_Todo_After_Process_Last				"Press [Export Result] button"

#define Format_LogLine								"Trial#%.4d | Calculated: %.2fm, Actual: %.2fm, Diff: %.2fm | Result: %s"

#define Format_FullResult_Header					"Trial#,Calculated distance,Actual distance,Difference,Result\r\n"
#define Format_FullResult_Row						"%d,%.8f,%.8f,%.8f,%d\r\n"

#define Format(tag) Format_##tag



int lpin::opencv::GetState()
{
	return state;
}

char *lpin::opencv::GetPtrOfString(int requestCode)
{
	switch ( requestCode )
	{
	case ::Constants::Gimme_Status:
		return buffer_status;
	case ::Constants::Gimme_LogLine:
		return buffer_logLine;
	case ::Constants::Gimme_FullResult:
		return buffer_fullResult;
	case ::Constants::Gimme_Todo:
		return buffer_todo;
	default:
		return nullptr;
	}
}

int lpin::opencv::GetLengthOfString(int requestCode)
{
	switch ( requestCode )
	{
	case ::Constants::Gimme_Status:
		return length_status;
	case ::Constants::Gimme_LogLine:
		return length_logLine;
	case ::Constants::Gimme_FullResult:
		return length_fullResult;
	case ::Constants::Gimme_Todo:
		return length_todo;
	default:
		return 0;
	}
}

int lpin::opencv::Initialize(int taskCode)
{
	if ( taskCode != 0 || state != ::Constants::Not_Defined )
	{
		length_status = std::sprintf(buffer_status, Format(Status_Initialize_Failed));
		return -1;
	}

	state = ::Constants::Busy;


	length_status = std::sprintf(buffer_status, Format(Status_Initialize_Successful));
	length_todo = std::sprintf(buffer_todo, Format(Todo_After_Initialize));
	state = ::Constants::Initialized;

	return 0;
}

int lpin::opencv::PutImage(void *bitmap)
{
//    std::cout<<"PutImage()\n";
//    std::cout<<"bitmap size"<<sizeof(bitmap)<<"\n";
//    std::cout<<"state_old :"<<state_old<<", "<<sizeof(state_old)*8<<"\n";
//    std::cout<<"state :"<<state_old<<", "<<sizeof(state)*8<<"\n";
//    std::cout<<"bitmap :"<<sizeof(bitmap)<<"\n";
    
    
	if ( state != ::Constants::WaitFor_BaseImage && state != ::Constants::WaitFor_QueryImage )
	{
//        std::cout<<"if ( state != ::Constants::WaitFor_BaseImage && state != ::Constants::WaitFor_QueryImage )\n";
		length_status = std::sprintf(buffer_status, Format(Status_PutImage_Failed), count_trials);
//        std::cout<<"length_status"<<length_status<<",";
//        std::cout<<"length_status"<<sizeof(length_status)<<" ";
		return -1;
	}

	if ( state == ::Constants::WaitFor_BaseImage )
	{
//        std::cout<<"if ( state == ::Constants::WaitFor_BaseImage )\n";
		state_old = state;
		state = ::Constants::Busy;
        
//        std::cout<<"state_old :"<<sizeof(state_old)*8<<"\n";
//        std::cout<<"state :"<<sizeof(state)*8<<"\n";
//        std::cout<<"bitmap :"<<sizeof(bitmap)<<"\n";

		int ret = ::ImageProcessor::PutBaseImage(bitmap);
//        std::cout<<"ret :"<<sizeof(ret)*8<<"\n";
        

		if ( ret != 0 )
		{
//            std::cout<<"ret:"<<ret<<"\n";
			length_status = std::sprintf(buffer_status, Format(Status_PutImage_Failed), count_trials);
			state = state_old;
//            std::cout<<"buffer_status :"<<sizeof(buffer_status)<<",";
//            std::cout<<"length_status :"<<sizeof(length_status)<<" ";
//            std::cout<<"length_status"<<length_status<<",";
//            std::cout<<"length_status"<<sizeof(length_status)<<" ";
//            std::cout<<"state"<<state<<"\n";
			return ret;
		}

		length_status = std::sprintf(buffer_status, Format(Status_PutImage_First_Successful), count_trials);
		state = ::Constants::WaitFor_QueryImage;
//        std::cout<<"if outside buffer_status :"<<sizeof(buffer_status)<<",";
//        std::cout<<"length_status :"<<length_status<<" length_status size :"<<sizeof(length_status)<<" ";
//        std::cout<<"state"<<state<<"\n";
	}
	else
	{
		state_old = state;
		state = ::Constants::Busy;

		int ret = ::ImageProcessor::PutQueryImage(bitmap);
        //std::cout<<"else"<<ret<<"\n";
		if ( ret != 0 )
		{
			length_status = std::sprintf(buffer_status, Format(Status_PutImage_Failed), count_trials);
			state = state_old;
//            std::cout<<"buffer_status :"<<sizeof(buffer_status)<<",";
//            std::cout<<"length_status"<<length_status<<",";
//            std::cout<<"length_status"<<sizeof(length_status)<<" ";
//            std::cout<<"state"<<state<<"\n";
			return ret;
		}
        
		length_status = std::sprintf(buffer_status, Format(Status_PutImage_Second_Successful), count_trials);
		state = ::Constants::WaitFor_ByteBlock;
//        std::cout<<"else if outside buffer_status :"<<sizeof(buffer_status)<<",";
//        std::cout<<"length_status"<<length_status<<",";
//        std::cout<<"length_status"<<sizeof(length_status)<<" ";
//        std::cout<<"state"<<state<<"\n";
	}
    
//    std::cout<<"bitmap size"<<sizeof(bitmap)<<"\n";

	return 0;
}

int lpin::opencv::PutByteBlock(void *data)
{
//    std::cout<<"PutByteBlock()\n";
	if ( state != ::Constants::WaitFor_ByteBlock )
	{
		length_status = std::sprintf(buffer_status, Format(Status_PutByteBlock_Failed), count_trials);
		return -1;
	}

	state_old = state;
	state = ::Constants::Busy;

	ptr_metadataBlock.raw = data;

	int ret = ::ImageProcessor::PutMetadata((*ptr_metadataBlock.parr)[idx_metadata_horizontalDistance_base], (*ptr_metadataBlock.parr)[idx_metadata_height_base], (*ptr_metadataBlock.parr)[idx_metadata_angle_base]);

	if ( ret != 0 )
	{
		length_status = std::sprintf(buffer_status, Format(Status_PutByteBlock_Failed), count_trials);
		state = state_old;
		return ret;
	}

	length_status = std::sprintf(buffer_status, Format(Status_PutByteBlock_Successful), count_trials);
	state = ::Constants::Ready_ToStartTrial;

	return 0;
}

int lpin::opencv::Process()
{
//    std::cout<<"Process()\n";
	if ( state != ::Constants::Ready_ToStartTrial )
	{
		length_status = std::sprintf(buffer_status, Format(Status_Process_Failed), count_trials);
		return -1;
	}

	state_old = state;
	state = ::Constants::Busy;

	double calculatedDistance = ::ImageProcessor::CalculateDistance();
	double actualDistance = (*ptr_metadataBlock.parr)[idx_metadata_actualDistance];
	double diff = calculatedDistance - actualDistance;

	if ( diff < 0 )
		diff = -diff;

	if ( std::isnan(calculatedDistance) )
		diff = std::numeric_limits<double>::infinity();
    

	length_logLine = std::sprintf(buffer_logLine, Format(LogLine), count_trials, calculatedDistance, actualDistance, diff, diff < 5.0 ? "PASS" : "FAIL");
	length_fullResult += std::sprintf(buffer_fullResult + length_fullResult, Format(FullResult_Row), count_trials, calculatedDistance, actualDistance, diff, diff < 5.0 ? 1 : 0);
//    std::cout<<"buffer_logLine size: "<<sizeof(buffer_logLine)<<"\n";
//    std::cout<<"length_logLine:"<<length_logLine<<", length_logLine size:"<<sizeof(length_logLine)<<"\n";
//
//
//    std::cout<<"buffer_fullResult size: "<<sizeof(buffer_fullResult)<<"\n"; //"buffer_fullResult : "<<buffer_fullResult<<
//    std::cout<<"length_fullResult:"<<length_fullResult<<", length_fullResult size:"<<sizeof(length_fullResult)<<"\n";
    
    

	++count_trials;

	if ( count_trials < ::Constants::numberOfTrials )
	{
		length_status = std::sprintf(buffer_status, Format(Status_Process_Successful), count_trials - 1);
		state = ::Constants::WaitFor_BaseImage;
	}
	else
	{
		length_status = std::sprintf(buffer_status, Format(Status_Process_Last_Successful), count_trials - 1);
		length_todo = std::sprintf(buffer_todo, Format(Todo_After_Process_Last));
		state = ::Constants::Completed;
	}

	return 0;
}

int lpin::opencv::Restart()
{
	if ( state != ::Constants::Initialized )
	{
		length_status = std::sprintf(buffer_status, Format(Status_Start_Failed));
		return -1;
	}

	state_old = state;
	state = ::Constants::Busy;

	int ret = ::ImageProcessor::Reset();

	if ( ret != 0 )
	{
		length_status = std::sprintf(buffer_status, Format(Status_Start_Failed));
		state = state_old;
		return ret;
	}

	count_trials = 0;

	length_status = std::sprintf(buffer_status, Format(Status_Start_Successful));
	length_todo = std::sprintf(buffer_todo, Format(Todo_After_Start));
	length_fullResult = std::sprintf(buffer_fullResult, Format(FullResult_Header));
	state = ::Constants::WaitFor_BaseImage;

	return 0;
}

#endif
