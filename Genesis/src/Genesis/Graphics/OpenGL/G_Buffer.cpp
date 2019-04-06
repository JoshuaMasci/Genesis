#include "G_Buffer.hpp"

using namespace Genesis;

G_Buffer::G_Buffer(vector2U buffer_size, bool multisample, uint16_t number_of_samples)
{
	this->size = buffer_size;
	this->multisample = multisample;
	this->number_of_samples = number_of_samples;

	glGenFramebuffers(1, &this->frame_buffer);
	glBindFramebuffer(GL_FRAMEBUFFER, this->frame_buffer);

	if (this->multisample == true)
	{
		//color buffer
		glGenTextures(1, &this->albedo_texture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->albedo_texture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, this->number_of_samples, GL_RGBA, this->size.x, this->size.y, true);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, this->albedo_texture, 0);

		//normal color buffer
		glGenTextures(1, &this->normal_texture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->normal_texture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, this->number_of_samples, GL_RGB16F, this->size.x, this->size.y, true);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D_MULTISAMPLE, this->normal_texture, 0);

		//position color buffer + glow
		glGenTextures(1, &this->position_texture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->position_texture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, this->number_of_samples, GL_RGBA32F, this->size.x, this->size.y, true);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D_MULTISAMPLE, this->position_texture, 0);

		//Depth
		glGenTextures(1, &this->depth_texture);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, this->depth_texture);
		glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, this->number_of_samples, GL_DEPTH_COMPONENT32F, this->size.x, this->size.y, true);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D_MULTISAMPLE, this->depth_texture, 0);
	}
	else
	{
		this->number_of_samples = 0;

		//color buffer
		glGenTextures(1, &this->albedo_texture);
		glBindTexture(GL_TEXTURE_2D, this->albedo_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->size.x, this->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, this->albedo_texture, 0);

		//normal color buffer
		glGenTextures(1, &this->normal_texture);
		glBindTexture(GL_TEXTURE_2D, this->normal_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, this->size.x, this->size.y, 0, GL_RGB, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, this->normal_texture, 0);

		//position color buffer + glow
		glGenTextures(1, &this->position_texture);
		glBindTexture(GL_TEXTURE_2D, this->position_texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, this->size.x, this->size.y, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, this->position_texture, 0);

		//Depth
		glGenTextures(1, &this->depth_texture);
		glBindTexture(GL_TEXTURE_2D, this->depth_texture);
		glTexStorage2D(GL_TEXTURE_2D, 1, GL_DEPTH_COMPONENT32F, this->size.x, this->size.y);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, this->depth_texture, 0);
	}

	unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
	glDrawBuffers(3, attachments);

	GLenum fbo_error = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);
	if (fbo_error != GL_FRAMEBUFFER_COMPLETE)
	{
		//Logger::getInstance()->logError("FrameBuffer not Complete: %x\n", fbo_error);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

G_Buffer::~G_Buffer()
{
	glDeleteTextures(1, &this->position_texture);
	glDeleteTextures(1, &this->normal_texture);
	glDeleteTextures(1, &this->albedo_texture);
	glDeleteTextures(1, &this->depth_texture);

	glDeleteFramebuffers(1, &this->frame_buffer);
}

GLuint G_Buffer::getFBO()
{
	return this->frame_buffer;
}

vector2U G_Buffer::getBufferSize()
{
	return size;
}

void G_Buffer::clearBuffer()
{
	glBindFramebuffer(GL_FRAMEBUFFER, this->frame_buffer);
	glClearDepth(0.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}
