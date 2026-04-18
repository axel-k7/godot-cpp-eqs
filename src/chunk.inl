#include "chunk.h"

template<typename T>
Chunk<T>::Chunk(size_t _capacity) 
    : chunk_buffer(operator new(sizeof(T) * _capacity, std::align_val_t(alignof(T))))
    , capacity(_capacity)
{
    //not sure how this works with dynamically sized Ts like vectors and such
    //probably fine as long as destructors are called?
}

template<typename T>
Chunk<T>::Chunk(Chunk&& _source) noexcept
    : chunk_buffer(_source.chunk_buffer)
    , count(_source.count)
    , capacity(_source.capacity)
{
    _source.chunk_buffer = nullptr;
    _source.count = 0;
    _source.capacity = 0;
}

template<typename T>
Chunk<T>::~Chunk() {
    Clear();
    if (chunk_buffer)
        operator delete(chunk_buffer, std::align_val_t(alignof(T)));
}

template<typename T>
auto Chunk<T>::operator=(Chunk&& _source) noexcept -> Chunk& {
    if (this == &_source)
        return *this;

    Clear();
    if (chunk_buffer)
        operator delete(chunk_buffer, std::align_val_t(alignof(T)));
    
    chunk_buffer = _source.chunk_buffer;
    count = _source.count;
    capacity = _source.capacity;

    _source.chunk_buffer = nullptr;
    _source.count = 0;
    _source.capacity = 0;

    return *this;
}


template<typename T>
template<typename... Args>
auto Chunk<T>::Push(Args&&... _args) -> T& {
    if (count >= capacity)
        throw std::bad_alloc();

    void* last = GetRaw(count);
    T* item = new (last) T(std::forward<Args>(_args)...);
    count++;

    return *item;
}


template<typename T>
void Chunk<T>::PopBack() {
    if (count == 0) return;

    count--;
    GetAt(count).~T();
}


template<typename T>
void Chunk<T>::Clear() {
    for (size_t i = 0; i < count; ++i)
        GetAt(i).~T();

    count = 0;
}


template<typename T>
void Chunk<T>::SwapPop(size_t _index) {
    if (_index >= count) return;

    size_t last = count-1;

    if (_index != last) {
        T* data = GetAll();
        T& dst = data[_index];
        T& src = data[last];
        
        dst.~T();
        new (&dst) T(std::move(src));
    }
    
    PopBack();
}

template<typename T>
auto Chunk<T>::MoveFrom(Chunk& _source, size_t _source_index) -> T& {
    //breaks if source = this
    
    T& source_data = _source.GetAt(_source_index);
    T* new_data = Push(std::move(source_data));

    _source.SwapPop(_source_index);

    return *new_data;
}