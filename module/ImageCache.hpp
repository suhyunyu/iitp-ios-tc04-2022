#pragma once

#include "opencv2/core.hpp"

#include <bitset>
#include <exception>

namespace lpin
{
	namespace opencv
	{
		/*
			Index 범위 체크 기능을 선택적으로 제공하는 이미지 배열입니다.
		
			size_: 이미지를 몇 개 다룰 것인지 정합니다.
			validateIndex: [ ] 연산자 사용시 index 값의 유효성을 체크하려는 경우 true입니다. 기본값은 false입니다.
			               이 값이 true인 경우 [ ] 연산자는 유효하지 않은 index에 대해 ImageCache<>::empty에 대한 reference를 return합니다.
		*/
		template <int size_, bool validateIndex = false>
		class ImageCache 
		{
		protected:
			static_assert(size_ > 0, "ImageCache::size must be positive.");

			cv::Mat images[size_];

			inline static cv::Mat empty_obj;

		public:
			static constexpr int size = size_;

			inline static const cv::Mat &empty = empty_obj;

			constexpr cv::Mat &operator [](int idx) & requires validateIndex
			{
				if ( idx < 0 || idx >= size_ )
					return empty_obj;

				return images[idx];
			}

			constexpr cv::Mat &operator [](int idx) & requires (!validateIndex)
			{
				return images[idx];
			}

			constexpr const cv::Mat &operator [](int idx) const & requires validateIndex
			{
				if ( idx < 0 || idx >= size_ )
					return empty_obj;

				return images[idx];
			}

			constexpr const cv::Mat &operator [](int idx) const & requires (!validateIndex)
			{
				return images[idx];
			}

			void Clear()
			{
				for ( cv::Mat &img : images )
					img.release();
			}
		};
	}
}