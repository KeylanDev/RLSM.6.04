#include "BufferPool.h"
#include <stdexcept>

namespace rslm {
    namespace util {

        // -- BufferPool --
        BufferPool::BufferPool(size_t bufferSize, size_t maxPoolSize)
            : m_bufferSize(bufferSize)
            , m_maxPoolSize(maxPoolSize) {
            if (bufferSize == 0) {
                throw std::runtime_error("BufferPool: bufferSize must be > 0");
            }
        }

        BufferPool::BorrowedBuffer BufferPool::Borrow() {
            std::lock_guard<std::mutex> lock(m_mutex);

            std::vector<uint8_t> buffer;
            if (!m_pool.empty()) {
                buffer = std::move(m_pool.front());
                m_pool.pop();
                buffer.clear(); // reset mais garde la capacité
            }
            else {
                buffer.resize(m_bufferSize);
            }

            return BorrowedBuffer(*this, std::move(buffer));
        }

        void BufferPool::ReturnBuffer(std::vector<uint8_t> buffer) {
            std::lock_guard<std::mutex> lock(m_mutex);

            if (m_pool.size() < m_maxPoolSize) {
                buffer.clear();
                m_pool.push(std::move(buffer));
            }
        }

        // -- BorrowedBuffer --
        BufferPool::BorrowedBuffer::BorrowedBuffer(BufferPool& pool, std::vector<uint8_t> buffer)
            : m_pool(&pool)
            , m_buffer(std::move(buffer)) {
        }

        BufferPool::BorrowedBuffer::~BorrowedBuffer() {
            if (m_pool) {
                m_pool->ReturnBuffer(std::move(m_buffer));
            }
        }

        BufferPool::BorrowedBuffer::BorrowedBuffer(BorrowedBuffer&& other) noexcept
            : m_pool(other.m_pool)
            , m_buffer(std::move(other.m_buffer)) {
            other.m_pool = nullptr;
        }

        BufferPool::BorrowedBuffer& BufferPool::BorrowedBuffer::operator=(BorrowedBuffer&& other) noexcept {
            if (this != &other) {
                if (m_pool) {
                    m_pool->ReturnBuffer(std::move(m_buffer));
                }
                m_pool = other.m_pool;
                m_buffer = std::move(other.m_buffer);
                other.m_pool = nullptr;
            }
            return *this;
        }

    } // namespace util
} // namespace rslm