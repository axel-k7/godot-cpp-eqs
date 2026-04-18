#pragma once

#include <deque>
#include <vector>

#include "chunk.h"


template<typename T>
class ChunkContainer {
private:
	static constexpr size_t INVALID_CHUNK_INDEX = SIZE_MAX;

	std::deque<Chunk<T>> chunks; //deque for stable addresses
	
	std::vector<size_t> id_to_index;
	std::vector<size_t> index_to_id;
	std::vector<size_t> free_ids;
	
public:
	auto CreateChunk(size_t _capacity) -> size_t;
	void DestroyChunk(size_t _id);
	auto GetChunk(size_t _id) -> Chunk<T>*;
	auto GetChunk(size_t _id) const -> const Chunk<T>*;
};

#include "chunk_container.inl"