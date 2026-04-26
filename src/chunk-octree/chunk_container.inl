#pragma once

template<typename T>
auto ChunkContainer<T>::CreateChunk(size_t _capacity) -> ChunkID {
    uint32_t index;

    if (!open_indices.empty()) {
        index = open_indices.back();
        open_indices.pop_back();

    	ChunkSlot& chunk = chunks[index];
    	chunk.chunk = Chunk<T>(_capacity);
    	chunk.active = true;

    	return ChunkID{ index, chunk.version };
    }

    chunks.emplace_back(_capacity);

	index = chunks.size();

	chunks.push_back( ChunkSlot{
		Chunk<T>(_capacity),
		0,
		true,
	});

    return ChunkID{ index, 0 };
}


template<typename T>
void ChunkContainer<T>::DestroyChunk(ChunkID _id) {
    if (_id.index >= chunks.size()) return;

    ChunkSlot& chunk = chunks[_id.index];

	if (!chunk.active || chunk.version != _id.version)
		return;

	chunk.chunk.Clear();
	chunk.active = false;
	++chunk.version;

	open_indices.push_back(_id.index);
}


template<typename T>
auto ChunkContainer<T>::GetChunk(ChunkID _id) -> Chunk<T>* {
	if (_id.index >= chunks.size())
		return nullptr;

	ChunkSlot& chunk = chunks[_id.index];

	if (!chunk.active || chunk.version != _id.version)
		return nullptr;

	return &chunk.chunk;
}
