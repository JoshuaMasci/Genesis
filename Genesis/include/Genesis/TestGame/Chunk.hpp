#pragma once

namespace Genesis
{
	typedef unsigned int BlockID;

	template<unsigned int x, unsigned int y, unsigned int z>
	class Chunk
	{
	public:
		BlockID blocks[x][y][z];
		bool has_changed = true;

		Chunk()
		{
			for (int i = 0; i < x; i++)
			{
				for (int j = 0; j < y; j++)
				{
					for (int k = 0; k < z; k++)
					{
						this->blocks[i][j][k] = 0;
					}
				}
			}
		};

		void setBlock(const vector3U& pos, BlockID id)
		{
			this->blocks[pos.x][pos.y][pos.z] = id;
			this->has_changed = true;
		};

		BlockID getBlock(const vector3U& pos)
		{
			return this->blocks[pos.x][pos.y][pos.z];
		};
	};

	typedef Chunk<32, 32, 32> DefaultChunk;
}