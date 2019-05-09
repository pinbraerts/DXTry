#ifndef DXTRY_OBJECT_HPP
#define DXTRY_OBJECT_HPP

#include "includes.hpp"
#include "Error.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

struct ObjectData {
	std::shared_ptr<Mesh> mesh;
	std::shared_ptr<Material> material;
	Matrix model;
};

struct Object: IObject, ObjectData {
	ComPtr<ID3D11Buffer> constant_buffer;

	Object() = default;
	Object(ObjectData&& data);
	Object(Engine& engine, ObjectData&& data);

	void set(ObjectData&& data);

	void init(Engine& engine) override;
	void render(Engine& engine) override;
	void update(Engine& engine) override;
};

#endif // !DXTRY_OBJECT_HPP
