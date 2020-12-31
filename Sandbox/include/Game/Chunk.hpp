#pragma once

namespace Genesis
{
	typedef unsigned int BlockID;

	//template<size_t SIZE_X, size_t SIZE_Y, size_t SIZE_Z>
	class Chunk
	{
	public:
		static const size_t size_x = 32;
		static const size_t size_y = 32;
		static const size_t size_z = 32;

		BlockID blocks[size_x][size_y][size_z];
		bool has_changed = true;

		Chunk()
		{
			for (int i = 0; i < size_x; i++)
			{
				for (int j = 0; j < size_y; j++)
				{
					for (int k = 0; k < size_z; k++)
					{
						this->blocks[i][j][k] = 0;
					}
				}
			}
		};

		vector3I getSize() const { return vector3I((int32_t)this->size_x, (int32_t)this->size_y, (int32_t)this->size_z); };

		void setBlock(const vector3I& pos, BlockID id)
		{
			this->blocks[pos.x][pos.y][pos.z] = id;
			this->has_changed = true;
		};

		BlockID getBlock(const vector3I& pos) const
		{
			return this->blocks[pos.x][pos.y][pos.z];
		};

		bool hasBlock(const vector3I& pos) const
		{
			if (pos.x > 0 && pos.x < size_x)
			{
				if (pos.y > 0 && pos.y < size_y)
				{
					if (pos.z > 0 && pos.z < size_z)
					{
						return this->blocks[pos.x][pos.y][pos.z] != 0;
					}
				}
			}

			return false;
		}
	};

	typedef Chunk DefaultChunk;
}