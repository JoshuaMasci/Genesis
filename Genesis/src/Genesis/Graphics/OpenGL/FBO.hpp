#ifndef FBO_HPP
#define FBO_HPP

#include "Genesis/Graphics/OpenGL/OpenGL_Include.hpp"
#include "Genesis/Core/VectorTypes.hpp"

namespace Genesis
{
	class FBO
	{
		virtual GLuint getFBO() = 0;
		virtual vector2U getBufferSize() = 0;
		virtual void clearBuffer() = 0;
	};
}

#endif //FBO_HPP