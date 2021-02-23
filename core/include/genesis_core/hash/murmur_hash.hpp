// MurmurHash2A, by Austin Appleby
#pragma once

#include <inttypes.h>

#define mmix(h,k) { k *= m; k ^= k >> r; k *= m; h *= m; h ^= k; }

namespace genesis
{
	class MurmurHash2
	{
	public:
		MurmurHash2(uint32_t seed = 0)
		{
			this->begin(seed);
		};

		void begin(uint32_t seed = 0)
		{
			m_hash = seed;
			m_tail = 0;
			m_count = 0;
			m_size = 0;
		};

		void add_data(const uint8_t* data, uint32_t len)
		{
			m_size += len;

			mix_tail(data, len);

			while (len >= 4)
			{
				uint32_t k = *(uint32_t*)data;

				mmix(m_hash, k);

				data += 4;
				len -= 4;
			}

			mix_tail(data, len);
		};

		template<typename T>
		void add(T value)
		{
			this->add_data((const uint8_t*)&value, sizeof(T));
		};

		uint32_t end(void)
		{
			mmix(m_hash, m_tail);
			mmix(m_hash, m_size);

			m_hash ^= m_hash >> 13;
			m_hash *= m;
			m_hash ^= m_hash >> 15;

			return m_hash;
		};

	private:

		static const uint32_t m = 0x5bd1e995;
		static const int r = 24;

		void mix_tail(const uint8_t*& data, uint32_t& len)
		{
			while (len && ((len < 4) || m_count))
			{
				m_tail |= (*data++) << (m_count * 8);

				m_count++;
				len--;

				if (m_count == 4)
				{
					mmix(m_hash, m_tail);
					m_tail = 0;
					m_count = 0;
				}
			}
		};

		uint32_t m_hash;
		uint32_t m_tail;
		uint32_t m_count;
		uint32_t m_size;
	};
}