#include "registry.h"

template<typename T>
template<typename... Args>
auto Registry<T>::CreateEntry(size_t _chunk_id, Args&&... _args) -> size_t {
    size_t id;

    if (!free_ids.empty()) {
        id = free_ids.back();
        free_ids.pop_back();
    } else {
        id = next_id++;
    }

    Chunk<Element>* chunk = chunks.GetChunk(_chunk_id);
    if (!chunk) return INVALID_ENTRY_ID;
    
    chunk->Push(id, std::forward<Args>(_args)...);

    if (id >= records.size())
        records.resize(id+1);

    records[id] = RegistryRecord{
        _chunk_id,
        chunk->count-1
    };

    return id;
}

template<typename T>
void Registry<T>::DestroyEntry(size_t _entry_id) {
    if (_entry_id >= records.size()) return;
    RegistryRecord& record = records[_entry_id];

    Chunk<Element>* chunk = chunks.GetChunk(record.chunk_id);
    if (!chunk) return;
    size_t index = record.index;

    chunk->SwapPop(index);

    //if swap happened
    if (index < chunk->count) {
        Element& swapped = chunk->GetAt(index);
        records[swapped.id].index = index;
    }

    InvalidateRecord(_entry_id);
    free_ids.push_back(_entry_id);
}

template<typename T>
auto Registry<T>::GetEntry(size_t _entry_id) -> T& {
    RegistryRecord& record = records[_entry_id];

    Chunk<Element>* chunk = chunks.GetChunk(record.chunk_id);
    if (!chunk) return;

    return chunk->GetAt(record.index).data;
}


template<typename T>
auto Registry<T>::CreateChunk(size_t _capacity) -> size_t {
    return chunks.CreateChunk(_capacity);
}

template<typename T>
void Registry<T>::DestroyChunk(size_t _chunk_id) {
    Chunk<Element>* chunk = chunks.GetChunk(_chunk_id);
    if(!chunk) return;

    auto elements = chunk->GetAll();

    for (int i = 0; i < chunk->count; ++i) {
        InvalidateRecord(elements[i].id);
        free_ids.push_back(elements[i].id);
    }

    chunks.DestroyChunk(_chunk_id);
}

template<typename T>
auto Registry<T>::GetChunk(size_t _chunk_id) const -> const Chunk<Element>* {
    return chunks.GetChunk(_chunk_id);
}


template<typename T>
void Registry<T>::MoveEntry(size_t _entry_id, size_t _new_chunk_id) {
    RegistryRecord& record = records[_entry_id];
    if (record.chunk_id == _new_chunk_id) return;

    Chunk<Element>* src = chunks.GetChunk(record.chunk_id);
    Chunk<Element>* dst = chunks.GetChunk(_new_chunk_id);

    Element& data = src->GetAt(record.index);

    dst->Push(std::move(data));
    size_t new_index = dst->count-1;

    src->SwapPop(record.index);

    if (record.index < src->count) {
        Element& swapped = src->GetAt(record.index);
        records[swapped.id].index = record.index;
    }

    record.chunk_id = _new_chunk_id;
    record.index = new_index;
}


template<typename T>
void Registry<T>::SwapEntry(size_t _a, size_t _b) {
    if (_a == _b) return;

    RegistryRecord& rec_a = records[_a];
    RegistryRecord& rec_b = records[_b];

    Chunk<Element>& chunk_a = chunks.GetChunk(rec_a.chunk_id);
    Chunk<Element>& chunk_b = chunks.GetChunk(rec_b.chunk_id);

    Element& element_a = chunk_a.GetAt(rec_a.index);
    Element& element_b = chunk_b.GetAt(rec_b.index);

    std::swap(element_a, element_b);
    element_a.id = _a;
    element_b.id = _b;
    std::swap(rec_a, rec_b);
}


template<typename T>
void Registry<T>::InvalidateRecord(size_t _entry_id) {
    if (_entry_id >= records.size()) return;
    RegistryRecord& record = records[_entry_id];

    record.chunk_id = SIZE_MAX;
    record.index = SIZE_MAX;
}


