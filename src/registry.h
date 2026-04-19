#pragma once


#include "chunk_container.h"


template<typename T>
class Registry {
public:
	static constexpr size_t INVALID_ENTRY_ID = SIZE_MAX;

	struct Element {
		T data;
		size_t id;

		template<typename... Args>
		Element(size_t _id, Args&&... _args)
			: data(std::forward<Args>(_args)...)
			, id(_id)
		{}

		Element(Element&& _other) noexcept
			: data(std::move(_other.data))
			, id(_other.id)
		{}
	};

	struct RegistryRecord {
		size_t chunk_id;
		size_t index;
	};

private:
	ChunkContainer<Element> chunks;
	std::vector<RegistryRecord> records; //id = index in records

	std::vector<size_t> free_ids;
	size_t next_id = 0;


	void InvalidateRecord(size_t _entry_id);

public:
	auto CreateChunk(size_t _capacity) -> size_t;
	void DestroyChunk(size_t _chunk_id);
	auto GetChunk(size_t _chunk_id) const -> const Chunk<Element>*; //readonly access

	template<typename... Args>
	auto CreateEntry(size_t _chunk_id, Args&&... _args) -> size_t;
	void DestroyEntry(size_t _entry_id);
	auto GetEntry(size_t _entry_id) -> T&;
	
	void MoveEntry(size_t _entry_id, size_t _new_chunk_id);
	void SwapEntry(size_t _a, size_t _b);

	auto GetAllChunks() { return chunks.GetAllChunks(); }
};

#include "registry.inl"