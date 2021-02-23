#pragma once

namespace genesis
{
	class TransformF
	{
	protected:
		vec3f position;
		quatf orientation;
		vec3f scale;

	public:
		TransformF(vec3f& position = vec3f(0.0f), quatf& orientation = quatf(1.0f, 0.0f, 0.0f, 0.0f), vec3f& scale = vec3f(1.0f));

		//Getters
		inline vec3f getPosition() const { return this->position; };
		inline quatf getOrientation() const { return this->orientation; };
		inline vec3f getScale() const { return this->scale; };

		inline vec3f getLeft() const { return this->orientation * vec3f(1.0f, 0.0f, 0.0f); };
		inline vec3f getUp() const { return this->orientation * vec3f(0.0f, 1.0f, 0.0f); };
		inline vec3f getForward() const { return this->orientation * vec3f(0.0f, 0.0f, 1.0f); };

		//Setters
		inline void setPosition(const vec3f& vec) { this->position = vec; };
		inline void setOrientation(const quatf& quat) { this->orientation = quat; };
		inline void setScale(const vec3f& vec) { this->scale = vec; };

		//Utils
		void setTransform(const mat4f& matrix);

		mat4f getModelMatrix() const;
		mat4f getViewMatirx() const;
		mat3f getNormalMatrix() const;
	};

	class TransformD
	{
	protected:
		
		vec3d position;
		quatd orientation;
		vec3d scale;
	public:
		TransformD(vec3d position = vec3d(0.0), quatd orientation = quatd(1.0, 0.0, 0.0, 0.0), vec3d scale = vec3d(1.0));

		//Getters
		inline vec3d getPosition() const { return this->position; };
		inline quatd getOrientation() const { return this->orientation; };
		inline vec3d getScale() const { return this->scale; };

		inline vec3d getLeft() const { return this->orientation * vec3d(1.0, 0.0, 0.0); };
		inline vec3d getUp() const { return this->orientation * vec3d(0.0, 1.0, 0.0); };
		inline vec3d getForward() const { return this->orientation * vec3d(0.0, 0.0, 1.0); };

		//Setters
		inline void setPosition(const vec3d& vec) { this->position = vec;};
		inline void setOrientation(const quatd& quat) { this->orientation = quat;};
		inline void setScale(const vec3d& vec) { this->scale = vec;};

		mat4f getModelMatrix(const vec3d& position_offset = vec3d(0.0));
		mat3f getNormalMatrix();
		mat4f getViewMatirx(const vec3d& position_offset = vec3d(0.0));

		void setTransform(const TransformD& new_transform);

		bool operator==(const TransformD& other);
		bool operator!=(const TransformD& other);
	};

	class TransformUtils
	{
	public:
		static void transformByInplace(TransformF& destination, const TransformF& origin, const TransformF& local);
		static void transformByInplace(TransformD& destination, const TransformD& origin, const TransformF& local);
		static void transformByInplace(TransformD& destination, const TransformD& origin, const TransformD& local);
		static TransformD transformBy(const TransformD& origin, const TransformF& local);
		static TransformD transformBy(const TransformD& origin, const TransformD& local);

		static void untransformByInplace(TransformD& destination, const TransformD& origin, const TransformD& global);

		static TransformD toTransformD(const TransformF& transform);
	};
};