#pragma once
#include <vector>
#include <queue>
#include <mutex>
#include <cstdint>

namespace rslm {
	namespace util {

		class BufferPool {
		public:
			explicit BufferPool(size_t bufferSize, size_t maxPoolSize = 50);

			class BorrowedBuffer {
			public:
				BorrowedBuffer(BufferPool& pool, std::vector<uint8_t> buffer);
				~BorrowedBuffer();

				BorrowedBuffer(const BorrowedBuffer&) = delete;
				BorrowedBuffer& operator=(const BorrowedBuffer&) = delete;
				
				BorrowedBuffer(BorrowedBuffer&& other) noexcept;
				BorrowedBuffer& operator=(BorrowedBuffer&& other) noexcept;

				std::vector<uint8_t>& data() { return m_buffer; }
				size_t size() const { return m_buffer.size(); }
				uint8_t* ptr() { return m_buffer.data(); }

			private:
				BufferPool* m_pool;
				std::vector<uint8_t> m_buffer;
			};
			
			BorrowedBuffer Borrow();

		private:
			friend class BorrowBuffer;
			void ReturnBuffer(std::vector<uint8_t> buffer);

			size_t m_bufferSize;
			size_t m_maxPoolSize;
			std::queue<std::vector<uint8_t>> m_pool;
			std::mutex m_mutex;
		};
	}//namespace util
}//namespace rslm