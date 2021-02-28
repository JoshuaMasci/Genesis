#include "OpenglBackend.hpp"

#include <genesis_engine/debug/Assert.hpp>
#include <genesis_engine/debug/Log.hpp>

#include "Opengl_Include.hpp"

namespace genesis
{
	namespace Opengl
	{

		void GLClearError()
		{
			while (glGetError() != GL_NO_ERROR);
		}

		void GLCheckError()
		{
			while (GLenum error = glGetError())
			{
				GENESIS_ENGINE_ERROR("Opengl Error: {}", error);
			}
		}

#define DEBUG
#ifdef DEBUG
#define GLCall(x) GLClearError(); x; GLCheckError();
#else
#define GLCall(x) x
#endif

		OpenglBackend::OpenglBackend(SDL2_Window* window)
		{
			this->window = window;

			this->opengl_context = ((SDL2_Window*)window)->GL_CreateContext();

			GENESIS_ENGINE_ASSERT(glewInit() == GLEW_OK, "Glew couldn't be initialized");

			GLint major, minor;
			glGetIntegerv(GL_MAJOR_VERSION, &major);
			glGetIntegerv(GL_MINOR_VERSION, &minor);
			GENESIS_ENGINE_INFO("Using OpenGL Version: {}.{}", major, minor);

			if ((major > 4 || (major == 4 && minor >= 5)) || GLEW_ARB_clip_control)
			{
				//Matches Vulkan's clip space
				GLCall(glClipControl(GL_UPPER_LEFT, GL_ZERO_TO_ONE));
			}
			else
			{
				GENESIS_ENGINE_ERROR("glClipControl required, sorry!");
				exit(1);
			}

			glFrontFace(GL_CW);
		}

		OpenglBackend::~OpenglBackend()
		{
			this->window->GL_DeleteContext(this->opengl_context);
		}

		vec2u OpenglBackend::getScreenSize()
		{
			return this->viewport_size;
		}

		void OpenglBackend::startFrame()
		{
			glEnable(GL_DEPTH_TEST);
			glDepthMask(GL_TRUE);

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			vec2u window_size = this->window->getWindowSize();

			if (window_size.y == 0) // Prevent A Divide By Zero By
			{
				window_size.y = 1;// Making Height Equal One
			}

			glViewport(0, 0, window_size.x, window_size.y);
			this->viewport_size = window_size;

			this->current_program = nullptr;
		}

		void OpenglBackend::endFrame()
		{
			this->window->GL_UpdateBuffer();

			this->last_frame_stats = current_frame_stats;
			current_frame_stats = { 0, 0 };
		}

		GLenum getVertexElementType(VertexElementType type)
		{
			switch (type)
			{
			case VertexElementType::float_1:
			case VertexElementType::float_2:
			case VertexElementType::float_3:
			case VertexElementType::float_4:
				return GL_FLOAT;
			case VertexElementType::unorm8_1:
			case VertexElementType::unorm8_2:
			case VertexElementType::unorm8_3:
			case VertexElementType::unorm8_4:
				return GL_UNSIGNED_BYTE;
			case VertexElementType::uint8_1:
			case VertexElementType::uint8_2:
			case VertexElementType::uint8_3:
			case VertexElementType::uint8_4:
				return GL_BYTE;
			case VertexElementType::uint16_1:
			case VertexElementType::uint16_2:
			case VertexElementType::uint16_3:
			case VertexElementType::uint16_4:
				return GL_UNSIGNED_SHORT;
			case VertexElementType::uint32_1:
			case VertexElementType::uint32_2:
			case VertexElementType::uint32_3:
			case VertexElementType::uint32_4:
				return GL_UNSIGNED_INT;
			}

			return 0;
		}

		VertexBuffer OpenglBackend::createVertexBuffer(void* data, uint64_t data_size, const VertexInputDescriptionCreateInfo& vertex_description)
		{
			OpenglVertexBuffer* vertex_buffer = new OpenglVertexBuffer();
			glGenVertexArrays(1, &vertex_buffer->vertex_array_object);
			glBindVertexArray(vertex_buffer->vertex_array_object);

			glGenBuffers(1, &vertex_buffer->vertex_buffer);
			glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer->vertex_buffer);
			glBufferData(GL_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);

			uint32_t total_size = 0;
			vector<size_t> offsets(vertex_description.input_elements_count);
			for (uint32_t i = 0; i < vertex_description.input_elements_count; i++)
			{
				offsets[i] = total_size;
				total_size += VertexElementTypeInfo::getInputElementSizeByte(vertex_description.input_elements[i]);
			}

			for (uint32_t i = 0; i < vertex_description.input_elements_count; i++)
			{
				glEnableVertexAttribArray(i);
				glVertexAttribPointer(
					i,                  // attribute location
					VertexElementTypeInfo::getInputElementCount(vertex_description.input_elements[i]), // count
					getVertexElementType(vertex_description.input_elements[i]), // type
					GL_FALSE,           //normalized TODO: NEED TO ADD SUPPORT
					total_size,     // stride
					(void*)offsets[i] // array buffer offset
				);
			}

			glBindVertexArray(0);
			glBindBuffer(GL_ARRAY_BUFFER, 0);
			for (uint32_t i = 0; i < vertex_description.input_elements_count; i++)
			{
				glDisableVertexAttribArray(i);
			}

			return vertex_buffer;
		}

		void OpenglBackend::destoryVertexBuffer(VertexBuffer buffer)
		{
			OpenglVertexBuffer* vertex_buffer = (OpenglVertexBuffer*)buffer;
			glDeleteVertexArrays(1, &vertex_buffer->vertex_array_object);
			glDeleteBuffers(1, &vertex_buffer->vertex_buffer);
			delete vertex_buffer;
		}

		IndexBuffer OpenglBackend::createIndexBuffer(void* data, uint64_t data_size, IndexType type)
		{
			OpenglIndexBuffer* index_buffer = new OpenglIndexBuffer();

			glGenBuffers(1, &index_buffer->index_buffer);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index_buffer->index_buffer);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, data_size, data, GL_STATIC_DRAW);

			index_buffer->type = type;

			return (IndexBuffer)index_buffer;
		}

		void OpenglBackend::destoryIndexBuffer(IndexBuffer buffer)
		{
			OpenglIndexBuffer* index_buffer = (OpenglIndexBuffer*)buffer;
			glDeleteBuffers(1, &index_buffer->index_buffer);
			delete index_buffer;
		}

		GLImageFormat getFormat(LegacyImageFormat format)
		{
			switch (format)
			{
			case genesis::LegacyImageFormat::R_8:
				return { GL_R8, GL_RED, GL_UNSIGNED_BYTE };
			case genesis::LegacyImageFormat::RG_8:
				return { GL_RG8, GL_RG, GL_UNSIGNED_BYTE };
			case genesis::LegacyImageFormat::RGB_8:
				return { GL_RGB8, GL_RGB, GL_UNSIGNED_BYTE };
			case genesis::LegacyImageFormat::RGBA_8:
			case genesis::LegacyImageFormat::RGBA_8_Unorm:
				return { GL_RGBA8, GL_RGBA, GL_UNSIGNED_BYTE };
			case genesis::LegacyImageFormat::R_16_Float:
				return { GL_R16F, GL_RED, GL_HALF_FLOAT };
			case genesis::LegacyImageFormat::RG_16_Float:
				return { GL_RG16F, GL_RG, GL_HALF_FLOAT };
			case genesis::LegacyImageFormat::RGB_16_Float:
				return { GL_RGB16F, GL_RGB, GL_HALF_FLOAT };
			case genesis::LegacyImageFormat::RGBA_16_Float:
				return { GL_RGBA16F, GL_RGBA, GL_HALF_FLOAT };
			case genesis::LegacyImageFormat::R_32_Float:
				return { GL_R32F, GL_RED, GL_FLOAT };
			case genesis::LegacyImageFormat::RG_32_Float:
				return { GL_RG32F, GL_RG, GL_FLOAT };
			case genesis::LegacyImageFormat::RGB_32_Float:
				return { GL_RGB32F, GL_RGB, GL_FLOAT };
			case genesis::LegacyImageFormat::RGBA_32_Float:
				return { GL_RGBA32F, GL_RGBA, GL_FLOAT };
			case genesis::LegacyImageFormat::D_16_Unorm:
				return { GL_DEPTH_COMPONENT16, GL_DEPTH_COMPONENT, GL_UNSIGNED_SHORT };
			case genesis::LegacyImageFormat::D_32_Float:
				return { GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT };
			case genesis::LegacyImageFormat::Invalid:
			default:
				return GLImageFormat();
			}
		}

		GLenum getDepthFormat(LegacyDepthFormat format)
		{
			switch (format)
			{
			case LegacyDepthFormat::depth_16:
				return GL_DEPTH_COMPONENT16;
			case LegacyDepthFormat::depth_24:
				return GL_DEPTH_COMPONENT24;
			case LegacyDepthFormat::depth_32:
				return GL_DEPTH_COMPONENT32;
			case LegacyDepthFormat::depth_32f:
				return GL_DEPTH_COMPONENT32F;
			}
			return 0;
		}

		Texture2D OpenglBackend::createTexture(const TextureCreateInfo& create_info, void* data)
		{
			OpenglTexture2D* texture = new OpenglTexture2D();

			glGenTextures(1, &texture->texture_handle);
			glBindTexture(GL_TEXTURE_2D, texture->texture_handle);

			GLImageFormat gl_format = getFormat(create_info.format);

			texture->format = gl_format;

			GLenum gl_wrap_mode;
			switch (create_info.wrap_mode)
			{
			case TextureWrapMode::Repeat:
				gl_wrap_mode = GL_REPEAT;
				break;
			case TextureWrapMode::Mirrored_Repeat:
				gl_wrap_mode = GL_MIRRORED_REPEAT;
				break;
			case TextureWrapMode::Clamp_Edge:
				gl_wrap_mode = GL_CLAMP;
				break;
			case TextureWrapMode::Clamp_Border:
				gl_wrap_mode = GL_CLAMP_TO_BORDER;
				break;
			}

			GLenum gl_filter;
			switch (create_info.filter_mode)
			{
			case TextureFilterMode::Linear:
				gl_filter = GL_LINEAR;
				break;
			case TextureFilterMode::Nearest:
				gl_filter = GL_NEAREST;
				break;
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap_mode);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap_mode);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);

			GLCall(glTexImage2D(GL_TEXTURE_2D, 0, gl_format.internal_format, create_info.size.x, create_info.size.y, 0, gl_format.format, gl_format.type, data));

			return (Texture2D)texture;
		}

		void OpenglBackend::destoryTexture(Texture2D texture)
		{
			glDeleteTextures(1, &((OpenglTexture2D*)texture)->texture_handle);
			delete (OpenglTexture2D*)texture;
		}

		ShaderProgram OpenglBackend::createShaderProgram(const char* vert_data, uint32_t vert_size, const char* frag_data, uint32_t frag_size)
		{
			ShaderStageInfo info[] = { {vert_data, vert_size, GL_VERTEX_SHADER}, {frag_data, frag_size, GL_FRAGMENT_SHADER} };
			return (ShaderProgram)new OpenglShaderProgram(info, _countof(info));
		}

		ShaderProgram OpenglBackend::createComputeShader(const char* data, uint32_t size)
		{
			ShaderStageInfo info = { data, size, GL_COMPUTE_SHADER };
			return(ShaderProgram)new OpenglShaderProgram(&info, 1);
		}

		void OpenglBackend::destoryShaderProgram(ShaderProgram program)
		{
			delete (OpenglShaderProgram*)program;
		}

		Framebuffer OpenglBackend::createFramebuffer(const FramebufferCreateInfo& create_info)
		{
			OpenglFramebuffer* framebuffer = new OpenglFramebuffer();
			framebuffer->size = create_info.size;
			framebuffer->is_multisampled = false;
			framebuffer->has_depth = false;

			glGenFramebuffers(1, &framebuffer->frame_buffer);
			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer->frame_buffer);

			framebuffer->attachements.resize(create_info.attachment_count);

			for (size_t i = 0; i < framebuffer->attachements.size(); i++)
			{
				GLuint attachment;
				glGenTextures(1, &attachment);

				GLImageFormat gl_format = getFormat(create_info.attachments[i].format);

				if (create_info.attachments[i].samples == MultisampleCount::Sample_1)
				{
					glBindTexture(GL_TEXTURE_2D, attachment);
					glTexImage2D(GL_TEXTURE_2D, 0, gl_format.internal_format, create_info.size.x, create_info.size.y, 0, gl_format.format, gl_format.type , NULL);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
					glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
					glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)(GL_COLOR_ATTACHMENT0 + i), GL_TEXTURE_2D, attachment, 0);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, attachment);
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, (GLsizei)create_info.attachments[i].samples, gl_format.internal_format, create_info.size.x, create_info.size.y, true);
					glFramebufferTexture2D(GL_FRAMEBUFFER, (GLenum)(GL_COLOR_ATTACHMENT0 + i), GL_TEXTURE_2D_MULTISAMPLE, attachment, 0);
				}

				framebuffer->attachements[i].texture_handle = attachment;
				framebuffer->attachements[i].format = gl_format;
			}

			if (create_info.depth_attachment != nullptr)
			{
				framebuffer->has_depth = true;

				GLenum depth_format = getDepthFormat(create_info.depth_attachment->format);

				GLuint depth_attachment;
				glGenTextures(1, &depth_attachment);

				if (create_info.depth_attachment->samples == MultisampleCount::Sample_1)
				{
					glBindTexture(GL_TEXTURE_2D, depth_attachment);
					glTexStorage2D(GL_TEXTURE_2D, 1, depth_format, create_info.size.x, create_info.size.y);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_attachment, 0);
				}
				else
				{
					glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, depth_attachment);
					glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, (GLsizei)create_info.depth_attachment->samples, depth_format, create_info.size.x, create_info.size.y, true);
					glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, depth_attachment, 0);
				}

				framebuffer->depth_attachement.texture_handle = depth_attachment;
			}

			glBindFramebuffer(GL_FRAMEBUFFER, 0);

			return framebuffer;
		}

		void OpenglBackend::destoryFramebuffer(Framebuffer framebuffer)
		{
			if (framebuffer == nullptr)
			{
				return;
			}

			OpenglFramebuffer* opengl_framebuffer = (OpenglFramebuffer*)framebuffer;

			for (size_t i = 0; i < opengl_framebuffer->attachements.size(); i++)
			{
				glDeleteTextures(1, &opengl_framebuffer->attachements[i].texture_handle);
			}

			if (opengl_framebuffer->has_depth)
			{
				glDeleteTextures(1, &opengl_framebuffer->depth_attachement.texture_handle);
			}

			glDeleteFramebuffers(1, &opengl_framebuffer->frame_buffer);

			delete opengl_framebuffer;
		}

		Texture2D OpenglBackend::getFramebufferColorAttachment(Framebuffer framebuffer, uint32_t index)
		{
			return (Texture2D)&((OpenglFramebuffer*)framebuffer)->attachements[index];
		}

		Texture2D OpenglBackend::getFramebufferDepthAttachment(Framebuffer framebuffer)
		{
			return (Texture2D)&((OpenglFramebuffer*)framebuffer)->depth_attachement;
		}

		void OpenglBackend::bindFramebuffer(Framebuffer framebuffer)
		{
			vec2u size;

			if (framebuffer == nullptr)
			{
				glBindFramebuffer(GL_FRAMEBUFFER, 0);
				size = this->window->getWindowSize();
			}
			else
			{
				//TODO TEMP
				glBindFramebuffer(GL_FRAMEBUFFER, ((OpenglFramebuffer*)framebuffer)->frame_buffer);
				size = ((OpenglFramebuffer*)framebuffer)->size;
			}

			if (size.y == 0) // Prevent A Divide By Zero By
			{
				size.y = 1;// Making Height Equal One
			}

			glViewport(0, 0, size.x, size.y);
			this->viewport_size = size;
		}

		void OpenglBackend::clearFramebuffer(bool color, bool depth, vec4f* clear_color, float* clear_depth)
		{
			if (clear_color != nullptr)
			{
				glClearColor(clear_color->x, clear_color->y, clear_color->z, clear_color->w);
			}

			if (clear_depth != nullptr)
			{
				glClearDepth(*clear_depth);
			}

			GLbitfield bits = 0;
			if (color)
			{
				bits |= GL_COLOR_BUFFER_BIT;
			}

			if (depth)
			{
				bits |= GL_DEPTH_BUFFER_BIT;
			}

			glClear(bits);
		}

		void OpenglBackend::setPipelineState(const PipelineSettings& pipeline_state)
		{
			switch (pipeline_state.cull_mode)
			{
			case CullMode::None:
				glDisable(GL_CULL_FACE);
				break;
			case CullMode::Front:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT);
				break;
			case CullMode::Back:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_BACK);
				break;
			case CullMode::All:
				glEnable(GL_CULL_FACE);
				glCullFace(GL_FRONT_AND_BACK);
				break;
			}

			switch (pipeline_state.depth_test)
			{
			case DepthTest::None:
				glDisable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				break;
			case DepthTest::Test_Only:
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_FALSE);
				break;
			case DepthTest::Test_And_Write:
				glEnable(GL_DEPTH_TEST);
				glDepthMask(GL_TRUE);
				break;
			}

			switch (pipeline_state.depth_op)
			{
			case DepthOp::Never:
				glDepthFunc(GL_NEVER);
				break;
			case DepthOp::Less:
				glDepthFunc(GL_LESS);
				break;
			case DepthOp::Equal:
				glDepthFunc(GL_EQUAL);
				break;
			case DepthOp::Less_Equal:
				glDepthFunc(GL_LEQUAL);
				break;
			case DepthOp::Greater:
				glDepthFunc(GL_GREATER);
				break;
			case DepthOp::Not_Equal:
				glDepthFunc(GL_NOTEQUAL);
				break;
			case DepthOp::Greater_Equal:
				glDepthFunc(GL_GEQUAL);
				break;
			case DepthOp::Always:
				glDepthFunc(GL_ALWAYS);
				break;
			}

			switch (pipeline_state.blend_op)
			{
			case BlendOp::None:
				glDisable(GL_BLEND);
				break;
			case BlendOp::Add:
				glEnable(GL_BLEND);
				glBlendEquation(GL_FUNC_ADD);
				break;
			case BlendOp::Subtract:
				glEnable(GL_BLEND);
				glBlendEquation(GL_FUNC_SUBTRACT);
				break;
			case BlendOp::Reverse_Subtract:
				glEnable(GL_BLEND);
				glBlendEquation(GL_FUNC_REVERSE_SUBTRACT);
				break;
			case BlendOp::Min:
				glEnable(GL_BLEND);
				glBlendEquation(GL_MIN);
				break;
			case BlendOp::Max:
				glEnable(GL_BLEND);
				glBlendEquation(GL_MAX);
				break;
			}

			{
				GLenum src_factor;
				switch (pipeline_state.src_factor)
				{
				case BlendFactor::Zero:
					src_factor = GL_ZERO;
					break;
				case BlendFactor::One:
					src_factor = GL_ONE;
					break;
				case BlendFactor::Color_Src:
					src_factor = GL_SRC_COLOR;
					break;
				case BlendFactor::One_Minus_Color_Src:
					src_factor = GL_ONE_MINUS_SRC_COLOR;
					break;
				case BlendFactor::Color_Dst:
					src_factor = GL_DST_COLOR;
					break;
				case BlendFactor::One_Minus_Color_Dst:
					src_factor = GL_ONE_MINUS_DST_COLOR;
					break;
				case BlendFactor::Alpha_Src:
					src_factor = GL_SRC_ALPHA;
					break;
				case BlendFactor::One_Minus_Alpha_Src:
					src_factor = GL_ONE_MINUS_SRC_ALPHA;
					break;
				case BlendFactor::Alpha_Dst:
					src_factor = GL_DST_ALPHA;
					break;
				case BlendFactor::One_Minus_Alpha_Dst:
					src_factor = GL_ONE_MINUS_DST_ALPHA;
					break;
				}

				GLenum dst_factor;
				switch (pipeline_state.dst_factor)
				{
				case BlendFactor::Zero:
					dst_factor = GL_ZERO;
					break;
				case BlendFactor::One:
					dst_factor = GL_ONE;
					break;
				case BlendFactor::Color_Src:
					dst_factor = GL_SRC_COLOR;
					break;
				case BlendFactor::One_Minus_Color_Src:
					dst_factor = GL_ONE_MINUS_SRC_COLOR;
					break;
				case BlendFactor::Color_Dst:
					dst_factor = GL_DST_COLOR;
					break;
				case BlendFactor::One_Minus_Color_Dst:
					dst_factor = GL_ONE_MINUS_DST_COLOR;
					break;
				case BlendFactor::Alpha_Src:
					dst_factor = GL_SRC_ALPHA;
					break;
				case BlendFactor::One_Minus_Alpha_Src:
					dst_factor = GL_ONE_MINUS_SRC_ALPHA;
					break;
				case BlendFactor::Alpha_Dst:
					dst_factor = GL_DST_ALPHA;
					break;
				case BlendFactor::One_Minus_Alpha_Dst:
					dst_factor = GL_ONE_MINUS_DST_ALPHA;
					break;
				}

				glBlendFunc(src_factor, dst_factor);
			}
		}

		void OpenglBackend::bindShaderProgram(ShaderProgram program)
		{
			this->current_program = (OpenglShaderProgram*)program;

			if (this->current_program != nullptr)
			{
				glUseProgram(current_program->getProgramID());
			}
			else
			{
				glUseProgram(0);
			}
		}

		void OpenglBackend::setUniform1i(const string& name, const int32_t& value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glUniform1i(current_program->getUniformLocation(name), value);
		}

		void OpenglBackend::setUniform1u(const string& name, const uint32_t& value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glUniform1ui(current_program->getUniformLocation(name), value);
		}

		void OpenglBackend::setUniform2u(const string& name, const vec2u& value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glUniform2ui(current_program->getUniformLocation(name), value.x, value.y);
		}

		void OpenglBackend::setUniform3u(const string& name, const vec3u& value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glUniform3ui(current_program->getUniformLocation(name), value.x, value.y, value.z);
		}

		void OpenglBackend::setUniform4u(const string& name, const vec4u& value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glUniform4ui(current_program->getUniformLocation(name), value.x, value.y, value.z, value.w);
		}

		void OpenglBackend::setUniform1f(const string& name, const float& value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glUniform1f(current_program->getUniformLocation(name), value);
		}

		void OpenglBackend::setUniform2f(const string& name, const vec2f& value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glUniform2f(current_program->getUniformLocation(name), value.x, value.y);
		}

		void OpenglBackend::setUniform3f(const string& name, const vec3f& value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glUniform3f(current_program->getUniformLocation(name), value.x, value.y, value.z);
		}

		void OpenglBackend::setUniform4f(const string& name, const vec4f& value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glUniform4f(current_program->getUniformLocation(name), value.x, value.y, value.z, value.w);
		}

		void OpenglBackend::setUniformMat3f(const string& name, const mat3f& value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glUniformMatrix3fv(current_program->getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
		}

		void OpenglBackend::setUniformMat4f(const string& name, const mat4f& value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glUniformMatrix4fv(current_program->getUniformLocation(name), 1, GL_FALSE, &value[0][0]);
		}

		void OpenglBackend::setUniformTexture(const string& name, const uint32_t texture_slot, Texture2D value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glActiveTexture(GL_TEXTURE0 + texture_slot);
			glBindTexture(GL_TEXTURE_2D, ((OpenglTexture2D*)value)->texture_handle);
			glUniform1i(current_program->getUniformLocation(name), texture_slot);
		}

		void OpenglBackend::setUniformTextureImage(const string& name, const uint32_t texture_slot, Texture2D value)
		{
			GENESIS_ENGINE_ASSERT(this->current_program != nullptr, "Shader Not Bound");
			glBindImageTexture(texture_slot, ((OpenglTexture2D*)value)->texture_handle, 0, GL_FALSE, 0, GL_READ_WRITE, ((OpenglTexture2D*)value)->format.internal_format);
			glUniform1i(current_program->getUniformLocation(name), texture_slot);
		}

		void OpenglBackend::setScissor(vec2i offset, vec2u extent)
		{
			glEnable(GL_SCISSOR_TEST);
			glScissor(offset.x, offset.y, extent.x, extent.y);
		}

		void OpenglBackend::clearScissor()
		{
			glDisable(GL_SCISSOR_TEST);
		}

		void OpenglBackend::bindVertexBuffer(VertexBuffer buffer)
		{
			this->vertex_buffer = (OpenglVertexBuffer*)buffer;

			if (this->vertex_buffer != nullptr)
			{
				glBindVertexArray(this->vertex_buffer->vertex_array_object);
			}
			else
			{
				glBindVertexArray(0);
			}
		}

		void OpenglBackend::bindIndexBuffer(IndexBuffer buffer)
		{
			this->index_buffer = (OpenglIndexBuffer*)buffer;

			if (this->index_buffer != nullptr)
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->index_buffer->index_buffer);
			}
			else
			{
				glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
			}
		}

		void OpenglBackend::drawIndex(uint32_t index_count, uint32_t index_offset)
		{
			if (this->index_buffer->type == IndexType::uint32)
			{
				glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_INT, (void*)(index_offset * sizeof(GLuint)));
			}
			else
			{
				glDrawElements(GL_TRIANGLES, index_count, GL_UNSIGNED_SHORT, (void*)(index_offset * sizeof(GLushort)));
			}

			this->current_frame_stats.draw_calls++;
			this->current_frame_stats.triangles_count += index_count / 3;
		}

		void OpenglBackend::draw(VertexBuffer vertex_buffer, IndexBuffer index_buffer, uint32_t triangle_count)
		{
			OpenglVertexBuffer* vertex = (OpenglVertexBuffer*)vertex_buffer;
			OpenglIndexBuffer* index = (OpenglIndexBuffer*)index_buffer;

			glBindVertexArray(vertex->vertex_array_object);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, index->index_buffer);

			glDrawElements(GL_TRIANGLES, triangle_count, (index->type == IndexType::uint32) ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT, 0);

			glBindVertexArray(0);
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

			this->current_frame_stats.draw_calls++;
			this->current_frame_stats.triangles_count += triangle_count / 3;
		}

		void OpenglBackend::dispatchCompute(uint32_t groups_x, uint32_t groups_y, uint32_t groups_z)
		{
			GLCall(glDispatchCompute((GLuint)groups_x, (GLuint)groups_y, (GLuint)groups_z));
			glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
		}

		FrameStats OpenglBackend::getLastFrameStats()
		{
			return this->last_frame_stats;
		}
	}
}