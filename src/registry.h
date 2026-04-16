#pragma once

#include <deque>

#include "godot_cpp/classes/ref_counted.hpp"
#include "godot_cpp/classes/area3d.hpp"
#include "godot_cpp/classes/world3d.hpp"
#include "godot_cpp/classes/physics_server3d.hpp"
#include "godot_cpp/classes/immediate_mesh.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/standard_material3d.hpp"

#include "godot_cpp/variant/aabb.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/typed_array.hpp"


using namespace godot;


template<typename T>
class Registry {
private:
	//manual memory management
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

	struct Chunk {
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

		void TransferData(size_t _index, Chunk* _source, size_t _source_index);

		template<typename... Args>
		void CreateAt(size_t _index, size_t _id, Args&&... _args);
		void SwapPop(size_t _index);

		//loop over this like a C array using count as the limit
		//maybe return this in a cleaner way?
		auto GetAll() -> Element* { return static_cast<Element*>(chunk_buffer); }
		auto GetAt(size_t _index) -> Element& { return GetAll()[_index]; }
		auto GetRaw(size_t _index) -> void* { return static_cast<uint8_t*>(chunk_buffer) + (sizeof(Element) * _index); }
	};

	struct RegistryRecord {
		Chunk* chunk;
		size_t index;
	};

	std::deque<Chunk> chunks; //decue for stable addresses
	std::vector<RegistryRecord> records; //id = index in records

	std::vector<size_t> free_ids;
	size_t next_id = 0;
	size_t capacity;

	auto EnsureChunk() -> Chunk&;
	void InvalidateRecord(size_t _id);

public:
	template<typename... Args>
	auto CreateEntry(Args&&... _args) -> size_t;
	void DestroyEntry(size_t _id);
	auto GetEntry(size_t _id) -> T&;
	
	Registry(size_t _capacity)
		: capacity(_capacity)
	{}
};
