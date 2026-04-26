#pragma once

#include "godot_cpp/variant/variant.hpp"

template<typename T>
struct Chunk {
	struct ChunkView {
		T* first;
		size_t count;

		T* begin() 		const 	 { return first; }
		T* end() 		const 	 { return first + count; }
		size_t size() 	const 	 { return count; }
		T& operator[](size_t _i) { return first[_i]; }
	};


	Chunk(size_t _capacity);
	~Chunk();

	//disable copying, only allowing moving
	Chunk(const Chunk&) = delete;
	auto operator=(const Chunk&) -> Chunk& = delete;

	Chunk(Chunk&& _source) noexcept;
	auto operator=(Chunk&& _source) noexcept -> Chunk&;

	void* chunk_buffer;

	size_t count = 0;
	size_t capacity;

	template<typename... Args>
	auto Push(Args&&... _args) -> T&;
	void PopBack();
	void Clear();
	void SwapPop(size_t _index);
	auto MoveFrom(Chunk& _source, size_t _source_index) -> T&;

	//loop over this like a C array using count as the limit
	//maybe return this in a cleaner way?
	auto GetAll() ->  T* { return static_cast<T*>(chunk_buffer); }
	auto GetAll() const -> const T* { return static_cast<const T*>(chunk_buffer); }

	auto GetAt(size_t _index) -> T& { return GetAll()[_index]; }
	auto GetAt(size_t _index) const -> const T& { return GetAll()[_index]; }

	auto GetRaw(size_t _index) const -> void* { return static_cast<uint8_t*>(chunk_buffer) + (sizeof(T) * _index); }

	auto begin() -> T* { return GetAll(); }
	auto begin() const -> const T* { return GetAll(); }

	auto end() 	 -> T* { return GetAll() + count; }
	auto end() 	 const -> const T* { return GetAll() + count; }

	auto Items() -> ChunkView { return { GetAll(), count }; }
	auto Items() const -> ChunkView { return { GetAll(), count }; }

	bool IsFull() const { return count == capacity; }
	bool IsEmpty() const { return count == 0; }
};

#include "chunk.inl"
