#pragma once

#include <flecs.h>
#include <Vector.h>

namespace GameEngine::Core
{
	class Camera;
	class Controller;
}

struct ControllerPtr
{
	GameEngine::Core::Controller* ptr;
};

struct JumpSpeed
{
	float value;
};

struct CameraPtr
{
	GameEngine::Core::Camera* ptr;
};

struct ShootPosition
{
	GameEngine::Math::Vector3f value;
};

struct ShootSpeed
{
	float value;

	explicit operator float() {
		return value;
	}
};

struct BulletCreator
{
	std::function<flecs::entity(ShootSpeed)> callable;
};

struct Bandolier {
	size_t shells;
	float reloadTime;
};

struct FirstTarget {
	flecs::entity target;
};

struct SecondTarget {
	flecs::entity target;
};

void RegisterEcsControlSystems(flecs::world& world);

