#include "rgba_image.h"
#include "wrapper_utils.hpp"

namespace blink {
	RGBAImage::RGBAImage()
	{
		
	}
	
	RGBAImage::RGBAImage(int width, int height)
	{
		m_width = width;
		m_height = height;
		size_t buf_size = (size_t)m_width * (size_t)m_height * 4;
		m_buffer.resize(buf_size);
	
	}
	
	RGBAImage* RGBAImage::Create(int width, int height)
	{
		return MakeGarbageCollected<RGBAImage>(width, height);
	}
	
	void RGBAImage::dispose()
	{
	
	}
	
	const uint8_t* RGBAImage::get_data(int& width, int& height) const
	{
		width = m_width;
		height = m_height;
		return m_buffer.data();
	}
	
}
