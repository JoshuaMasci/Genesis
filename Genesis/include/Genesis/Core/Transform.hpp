#pragma once

namespace Genesis
{
	class TransformF
	{
	public:
		TransformF(vector3F& position = vector3F(0.0f), quaternionF& orientation = quaternionF(1.0f, 0.0f, 0.0f, 0.0f), vector3F& scale = vector3F(1.0f));

		//Getters
		inline vector3F getPosition() const { return this->position; };
		inline quaternionF getOrientation() const { return this->orientation; };
		inline vector3F getScale() const { return this->scale; };

		inline vector3F getLeft() const { return this->orientation * vector3F(1.0f, 0.0f, 0.0f); };
		inline vector3F getUp() const { return this->orientation * vector3F(0.0f, 1.0f, 0.0f); };
		inline vector3F getForward() const { return this->orientation * vector3F(0.0f, 0.0f, 1.0f); };

		//Setters
		inline void setPosition(const vector3F& vec) { this->position = vec; };
		inline void setOrientation(const quaternionF& quat) { this->orientation = quat; };
		inline void setScale(const vector3F& vec) { this->scale = vec; };

		//Utils
		TransformF transformBy(const TransformF& transform1) const;
		matrix4F getModelMatrix() const;
		matrix4F getViewMatirx() const;
		matrix3F getNormalMatrix() const;

	private:
		vector3F position;
		quaternionF orientation;
		vector3F scale;
	};

	class TransformD
	{
	public:
		TransformD(vector3D position = vector3D(0.0), quaternionD orientation = quaternionD(1.0, 0.0, 0.0, 0.0), vector3D scale = vector3D(1.0));

		//Getters
		inline vector3D getPosition() const { return this->position; };
		inline quaternionD getOrientation() const { return this->orientation; };
		inline vector3D getScale() const { return this->scale; };

		inline vector3D getLeft() const { return this->orientation * vector3D(1.0, 0.0, 0.0); };
		inline vector3D getUp() const { return this->orientation * vector3D(0.0, 1.0, 0.0); };
		inline vector3D getForward() const { return this->orientation * vector3D(0.0, 0.0, 1.0); };

		//Setters
		inline void setPosition(const vector3D& vec) { this->position = vec; this->has_changed = true; };
		inline void setOrientation(const quaternionD& quat) { this->orientation = quat; this->has_changed = true; };
		inline void setScale(const vector3D& vec) { this->scale = vec; this->has_changed = true; };

		//Utils
		TransformD transformBy(const TransformD& transform1) const;

		matrix4F getModelMatrix(const vector3D& position_offset = vector3D(0.0));
		matrix3F getNormalMatrix();
		matrix4F getViewMatirx(const vector3D& position_offset = vector3D(0.0));

	private:
		void updateModelMatrix();

		vector3D position;
		quaternionD orientation;
		vector3D scale;

		bool has_changed;
		matrix4F untranslated_model_matrix;
	};

	class TransformUtil
	{
	public:
		static TransformD transformBy(const TransformD& parent, const TransformD& child);
		static TransformD transformBy(const TransformD& parent, const TransformF& child);
	};
};