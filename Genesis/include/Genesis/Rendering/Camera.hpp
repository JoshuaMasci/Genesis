#pragma once

namespace Genesis
{
	struct Camera
	{
		Camera(float fov = 95.0f)
		{
			this->frame_of_view = fov;
		};

		float frame_of_view = 95.0f;
		float z_near = 0.1f;
		float z_far = 1000.0f;

		matrix4F get_projection_matrix(float aspect_ratio)
		{
			float fovy = atan(tan(glm::radians(this->frame_of_view) / 2.0f) / aspect_ratio) * 2.0f;
			matrix4F matrix = glm::perspective(fovy, aspect_ratio, this->z_near, this->z_far);
			matrix[1][1] *= -1.0f;
			return matrix;
		};

		matrix4F get_projection_matrix(vector2U image_size)
		{
			return this->get_projection_matrix(((float)image_size.x) / ((float)image_size.y));
		};

		matrix4F get_infinite_projection_no_flip(vector2U image_size)
		{
			float aspect_ratio = (float)image_size.x / (float)image_size.y;
			float fovy = atan(tan(glm::radians(this->frame_of_view) / 2.0f) / aspect_ratio) * 2.0f;
			return glm::infinitePerspective(fovy, aspect_ratio, this->z_near);
		};
	};
}