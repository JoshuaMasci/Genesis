#pragma once

namespace Genesis
{
	struct Camera
	{
		Camera(float fov = 75.0f)
		{
			this->frame_of_view = fov;
		};

		float frame_of_view = 75.0f;
		float z_near = 0.1f;
		float z_far = 1000.0f;

		matrix4F getProjectionMatrix(float aspect_ratio)
		{
			float fovy = atan(tan(glm::radians(this->frame_of_view) / 2.0f) / aspect_ratio) * 2.0f;
			matrix4F matrix = glm::perspective(fovy, aspect_ratio, this->z_near, this->z_far);
			matrix[1][1] *= -1.0f;
			return matrix;
		};
	};
}