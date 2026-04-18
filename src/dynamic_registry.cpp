#include "dynamic_registry.h"

template<typename T>
DynamicRegistry<T>::Chunk::Chunk(size_t _capacity) {
    chunk_buffer = operator new(sizeof(Element) * _capacity, std::align_val_t(alignof(Element)));
    //not sure how this works with dynamically sized elements like vectors and such
    //probably fine as long as destructors are called?
}

template<typename T>
DynamicRegistry<T>::Chunk::Chunk(Chunk&& _source) noexcept
    : chunk_buffer(_source.chunk_buffer)
    , count(_source.count)
    , capacity(_source.capacity)
{
    _source.chunk_buffer = nullptr;
    _source.count = 0;
    _source.capacity = 0;
}

template<typename T>
DynamicRegistry<T>::Chunk::~Chunk() {
    auto data_array = GetAll();
    for (size_t i = 0; i < count; ++i)
        data_array[i].~Element();

    if (chunk_buffer)
        operator delete(chunk_buffer, std::align_val_t(alignof(Element)));
}

template<typename T>
auto DynamicRegistry<T>::Chunk::operator=(Chunk&& _source) noexcept -> Chunk& {
    if (this == &_source)
        return *this;

    this->~Chunk();
    
    chunk_buffer = _source.chunk_buffer;
    count = _source.count;
    capacity = _source.capacity;

    _source.chunk_buffer = nullptr;
    _source.count = 0;
    _source.capacity = 0;

    return *this;
}

template<typename T>
void DynamicRegistry<T>::Chunk::TransferData(size_t _index, Chunk* _source, size_t _source_index) {
    Element& source = _source->GetAt(_source_index);
    void* destination = GetRaw(_index);

    new (destination) Element(std::move(source));
    source.~Element();
}

template<typename T>
template<typename... Args>
void DynamicRegistry<T>::Chunk::CreateAt(size_t _index, size_t _id, Args&&... _args) {
    new (GetRaw(_index)) Element(_id, std::forward<Args>(_args)...);
    count++;
}

template<typename T>
void DynamicRegistry<T>::Chunk::SwapPop(size_t _index) {
    size_t last = count-1;

    if (_index != last)
        TransferData(_index, this, last);
    
    GetAt(last).~Element();
    count--;
}

template<typename T>
auto DynamicRegistry<T>::EnsureChunk() -> Chunk& {
    for(Chunk& chunk : chunks) {
        if (chunk.count < chunk.capacity)
            return chunk;
    }

    chunks.emplace_back(capacity);
    return chunks.back();
}

template<typename T>
void DynamicRegistry<T>::InvalidateRecord(size_t _id) {
    RegistryRecord& record = records[_id];

    record.chunk = nullptr;
    record.index = SIZE_MAX;
}


template<typename T>
template<typename... Args>
auto DynamicRegistry<T>::CreateEntry(Args&&... _args) -> size_t {
    size_t id;

    if (!free_ids.empty()) {
        id = free_ids.back();
        free_ids.pop_back();
    } else {
        id = next_id++;
    }

    Chunk& chunk = EnsureChunk();
    size_t index = chunk.count;
    chunk.CreateAt(index, id, std::forward<Args>(_args)...);

    RegistryRecord record{
        &chunk,
        index
    };


    if (id >= records.size())
        records.resize(id+1);

    records[id] = record; 

    return id;
}

template<typename T>
void DynamicRegistry<T>::DestroyEntry(size_t _id) {
    RegistryRecord& record = records[_id];

    Chunk* chunk = record.chunk;
    size_t index = record.index;

    chunk->SwapPop(index);

    //if popped wasn't last
    if (index < chunk->count) {
        Element& moved = chunk->GetAt(index);

        records[moved.id] = {
            chunk,
            index
        };
    }

    InvalidateRecord(_id);
    free_ids.push_back(_id);
}

template<typename T>
auto DynamicRegistry<T>::GetEntry(size_t _id) -> T& {
    return records[_id].chunk->GetAt(records[_id].index).data;
}
