#pragma once

#include <iostream>
#include <queue>
#include <thread>
#include <mutex>
#include <semaphore>

#include "ImageProcessor_Base.hpp"

namespace lpin
{
	namespace opencv
	{
		/*
			이미지 병렬 변환 기능을 구성해 둔 버전입니다.
			Non-static 버전이며 각 object들은 독립적으로 동작합니다.

			파일 일괄 변환을 수행할 때를 전제로 구성되어 있습니다.
			- 파일에서 읽는 thread와 파일에 기록하는 thread가 서로 다름
			- 읽는 순서와 기록하는 순서는 서로 달라도 됨
			- 읽는 속도와 기록하는 속도가 둘 다 일정하지 않음

			이 클래스 정의에는 std::mutex와 std::countable_semaphore를 사용하여 concurrency 확보를 수행하는 방법이 포함되어 있습니다.
		*/
		template <>
		class ImageProcessor<1> : ImageProcessor_Base 
		{
		protected:
			/*
				내부에서 사용할 Constants, ImageConverter 버전 선택
			*/
			using Constants = Constants<1>;

			using ImageConverter_In = DefaultImageConverter_In<Constants::img_output_width, Constants::img_output_height, Constants::convertToGrayscale>;
			using ImageConverter_Out = DefaultImageConverter_Out<Constants::img_output_width, Constants::img_output_height, Constants::img_output_width, Constants::img_output_height, Constants::convertToGrayscale>;

			struct TaskInfo
			{
				Constants::ID_t id;
				cv::Mat img;
			};


			std::jthread thrs_worker[Constants::numberOfWorkerThreads];

			Constants::ID_t id_next{};

			std::queue<TaskInfo> queue_in;
			std::mutex m_queue_in;
			std::counting_semaphore<> smph_queue_in{ 0 };

			std::queue<TaskInfo> queue_out;
			std::mutex m_queue_out;
			std::counting_semaphore<> smph_queue_out{ 0 };
			
			template <bool skipOutwardConversion = Constants::skipOutwardConversion>
			static void run_worker(std::stop_token token, ImageProcessor<1> &self)
			{
				TaskInfo info;
				
				while ( token.stop_requested() == false )
				{
					self.smph_queue_in.acquire();

					if ( token.stop_requested() )
					{
						self.smph_queue_in.release();
						break;
					}

					std::unique_lock lock_queue_in{ self.m_queue_in };
					info = self.queue_in.front();
					self.queue_in.pop();
					lock_queue_in.unlock();

					info.img = ImageConverter_In::Convert(info.img);

					if ( token.stop_requested() )
					{
						break;
					}

					std::unique_lock lock_queue_out{ self.m_queue_out };
					self.queue_out.emplace(std::move(info));
					lock_queue_out.unlock();

					self.smph_queue_out.release();
				}
			}

			template <>
			static void run_worker<false>(std::stop_token token, ImageProcessor<1> &self)
			{
				TaskInfo info;

				while ( token.stop_requested() == false )
				{
					self.smph_queue_in.acquire();

					if ( token.stop_requested() )
					{
						self.smph_queue_in.release();
						break;
					}

					std::unique_lock lock_queue_in{ self.m_queue_in };
					info = std::move(self.queue_in.front());
					self.queue_in.pop();
					lock_queue_in.unlock();

					info.img = ImageConverter_Out::Convert(ImageConverter_In::Convert(info.img));

					if ( token.stop_requested() )
					{
						break;
					}

					std::unique_lock lock_queue_out{ self.m_queue_out };
					self.queue_out.emplace(std::move(info));
					lock_queue_out.unlock();

					self.smph_queue_out.release();
				}
			}

		public:
			ImageProcessor()
			{
				for ( int idx = 0; idx < Constants::numberOfWorkerThreads; idx++ )
				{
					thrs_worker[idx] = std::jthread{ run_worker<>, std::ref(*this) };
				}
			}

			~ImageProcessor() noexcept
			{
				for ( auto &thr : thrs_worker )
				{
					if ( thr.joinable() )
					{
						thr.request_stop();
					}
				}

				smph_queue_in.release(Constants::numberOfWorkerThreads);

				for ( auto &thr : thrs_worker )
				{
					if ( thr.joinable() )
					{
						thr.join();
					}
				}
			}

			int PutSourceImage(cv::Mat mat)
			{
				std::unique_lock lock{ m_queue_in };
				int id = id_next++;
				queue_in.emplace(id, std::move(mat));
				lock.unlock();

				smph_queue_in.release();

				return id;
			}

			int PutSourceImage(void *ptr, int width, int height)
			{
				return PutSourceImage(cv::Mat{ height, width, Constants::external_img_type, ptr });
			}

			template <bool waitForResultIfNotReady = Constants::waitForResultIfNotReady>
			TaskInfo GetNextResult()
			{
				smph_queue_out.acquire();

				std::unique_lock lock{ m_queue_out };
				TaskInfo result = std::move(queue_out.front());
				queue_out.pop();
				lock.unlock();

				return result;
			}

			template <>
			TaskInfo GetNextResult<false>()
			{
				if ( smph_queue_out.try_acquire() )
				{
					std::unique_lock lock{ m_queue_out };
					TaskInfo result = std::move(queue_out.front());
					queue_out.pop();
					lock.unlock();

					return result;
				}

				return { -1, {} };
			}
		};
	}
}