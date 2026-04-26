#pragma once

#include <deque>
#include <vector>

#include "chunk.h"


template<typename T>
class ChunkContainer {
protected:
	struct ChunkSlot {
		Chunk<T> chunk;
		uint32_t version = 0;
		bool active = false;
	};

	std::vector<ChunkSlot> chunks;

	std::vector<std::uint32_t> open_indices;

public:
	struct ChunkID {
		uint32_t index;
		uint32_t version;
	};

	auto CreateChunk(size_t _capacity) -> ChunkID;
	void DestroyChunk(ChunkID _id);
	auto GetChunk(ChunkID _id) -> Chunk<T>*;
	auto GetChunk(ChunkID _id) const -> const Chunk<T>* { return GetChunk(_id); };

	auto begin() { return chunks.begin(); };
	auto end() { return chunks.end(); };

	auto begin() const { return chunks.begin(); };
	auto end() const { return chunks.end(); };
};

#include "chunk_container.inl"
