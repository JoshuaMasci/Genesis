#ifndef G_BUFFER_HPP
#define G_BUFFER_HPP

#include "Genesis/Graphics/OpenGL/FBO.hpp"
#include "Genesis/Graphics/OpenGL/OpenGL_Include.hpp"
#include "Genesis/Core/VectorTypes.hpp"

namespace Genesis
{
	class G_Buffer : public FBO
	{
	public:
		G_Buffer(vector2U buffer_size, bool multisample = false, uint16_t number_of_samples = 0);
		~G_Buffer();

		virtual GLuint getFBO();
		virtual vector2U getBufferSize();
		virtual void clearBuffer();

		GLuint getAlbedoTexture() { return this->albedo_texture; };
		GLuint getNormalTexture() { return this->normal_texture; };
		GLuint getPositionTexture() { return this->position_texture; };
		GLuint getDepthTexture() { return this->depth_texture; };

	private:
		GLuint frame_buffer;
		GLuint albedo_texture;
		GLuint normal_texture;
		GLuint position_texture;
		GLuint depth_texture;

		vector2U size;
		bool multisample;
		int number_of_samples;
	};
}
#endif //G_BUFFER_HPP