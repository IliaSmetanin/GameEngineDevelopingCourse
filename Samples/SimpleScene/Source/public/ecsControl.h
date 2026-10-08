#pragma once

#include <flecs.h>
#include <ecsPhys.h>

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

struct BulletCreator
{
	std::function<flecs::entity(ShootSpeed)> callable;
};

struct Reload
{
	float reload_time;
};

void RegisterEcsControlSystems(flecs::world& world);

