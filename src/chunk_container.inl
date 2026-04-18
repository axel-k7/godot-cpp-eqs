#include "chunk_container.h"

template<typename T>
auto ChunkContainer<T>::CreateChunk(size_t _capacity) -> size_t {
    size_t id;

    if (!free_ids.empty()) {
        id = free_ids.back();
        free_ids.pop_back();
    } else {
        id = id_to_index.size();
        id_to_index.push_back(0);
        index_to_id.push_back(0);
    }

    chunks.emplace_back(_capacity);

    size_t index = chunks.size() - 1;
    id_to_index[id] = index;

    if (index >= index_to_id.size())
        index_to_id.push_back(id);
    else
        index_to_id[index] = id;

    return id;
}


template<typename T>
void ChunkContainer<T>::DestroyChunk(size_t _id) {
    if (chunks.empty()) return;
    if (_id >= id_to_index.size()) return;

    size_t index = id_to_index[_id];
    if (index == INVALID_CHUNK_INDEX) return;

    size_t last = chunks.size()-1;

    if (index != last) {
        chunks[index] = std::move(chunks[last]);
    
        size_t moved = index_to_id[last];

        id_to_index[moved] = index;
        index_to_id[index] = moved;
    }

    chunks.pop_back();

    id_to_index[_id] = INVALID_CHUNK_INDEX;
    free_ids.push_back(_id);
}


template<typename T>
auto ChunkContainer<T>::GetChunk(size_t _id) -> Chunk<T>* {
    if (_id >= id_to_index.size()) return nullptr;

    size_t index = id_to_index[_id];
    if (index == INVALID_CHUNK_INDEX)  return nullptr;

    return &chunks[index];
}

template<typename T>
auto ChunkContainer<T>::GetChunk(size_t _id) const -> const Chunk<T>* {
    if (_id >= id_to_index.size()) return nullptr;

    size_t index = id_to_index[_id];
    if (index == INVALID_CHUNK_INDEX)  return nullptr;

    return &chunks[index];
}