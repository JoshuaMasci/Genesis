#pragma once

#include "Genesis/Rendering/RenderingTypes.hpp"
#include "Genesis/Core/Array.hpp"
#include "Genesis/Core/MurmurHash2.hpp"

namespace Genesis
{
	class FramebufferLayout
	{
	public:
		FramebufferLayout() 
		{
			this->color_formats = Array<ImageFormat>(0);
			this->depth_format = ImageFormat::Invalid;
			this->hash_value = 0;
		};

		FramebufferLayout(Array<ImageFormat> color_formats, ImageFormat depth_format)
		{
			this->color_formats = color_formats;
			this->depth_format = depth_format;

			MurmurHash2 hash;
			hash.begin();

			for (size_t i = 0; i < color_formats.size(); i++)
			{
				hash.add(color_formats[i]);
			}

			if (depth_format != ImageFormat::Invalid)
			{
				hash.add(depth_format);
			}
			
			this->hash_value = hash.end();
		};

		inline uint32_t getHash() { return this->hash_value; }

		inline size_t getColorCount() { return this->color_formats.size(); };
		inline ImageFormat getColor(size_t i) { return this->color_formats[i]; };
		inline ImageFormat getDepth() { return this->depth_format; };

	private:
		Array<ImageFormat> color_formats;
		ImageFormat depth_format;

		uint32_t hash_value;
	};
}