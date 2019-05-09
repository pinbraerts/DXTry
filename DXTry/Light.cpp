#include "Light.hpp"
#include "Engine.hpp"

void Light::set(ObjectData&& data, LightData l) {
	Object::set(std::move(data));
	LightData::operator=(l);
}

void Light::premultiply(const Matrix& world) {
	Matrix tr = world.Transpose();

	float w = position.w;
	position.w = 1;
	position = Vector4::Transform(position, tr);
	position.w = w;

	w = eye.w;
	eye.w = 1;
	eye = Vector4::Transform(eye, tr);
	eye.w = w;

	w = direction.w;
	direction.w = 1;
	direction = Vector4::Transform(direction, tr);
	direction.w = w;
}

void Light::init(Engine& engine) {
	Object::init(engine);
	constant_light = engine.create_buffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(LightData));
}

void Light::update(Engine& engine) {
	Object::update(engine);
	engine.context->UpdateSubresource(
		constant_light.Get(),
		0,
		nullptr,
		static_cast<LightData*>(this),
		0,
		0
	);
}

void Light::render(Engine& engine) {
	ID3D11Buffer* pscbs[] {
		constant_light.Get()
	};
	engine.context->VSSetConstantBuffers(1, (UINT)std::size(pscbs), pscbs);
	engine.context->PSSetConstantBuffers(1, (UINT)std::size(pscbs), pscbs);

	if(position.w < 0.5f)
		Object::render(engine);
}
