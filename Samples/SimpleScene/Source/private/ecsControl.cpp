#include <Camera.h>
#include <ecsControl.h>
#include <ECS/ecsSystems.h>
#include <ecsPhys.h>
#include <flecs.h>
#include <Input/Controller.h>
#include <Input/InputHandler.h>
#include <Vector.h>

using namespace GameEngine;

void RegisterEcsControlSystems(flecs::world& world)
{
	world.system<Position, CameraPtr, const Speed, const ControllerPtr>()
		.each([&](Position& position, CameraPtr& camera, const Speed& speed, const ControllerPtr& controller)
	{
		Math::Vector3f currentMoveDir = Math::Vector3f::Zero();
		if (controller.ptr->IsPressed("GoLeft"))
		{
			currentMoveDir = currentMoveDir - camera.ptr->GetRightDir();
		}
		if (controller.ptr->IsPressed("GoRight"))
		{
			currentMoveDir = currentMoveDir + camera.ptr->GetRightDir();
		}
		if (controller.ptr->IsPressed("GoBack"))
		{
			currentMoveDir = currentMoveDir - camera.ptr->GetViewDir();
		}
		if (controller.ptr->IsPressed("GoForward"))
		{
			currentMoveDir = currentMoveDir + camera.ptr->GetViewDir();
		}
		position.value = position.value + currentMoveDir.Normalized() * speed * world.delta_time();
		camera.ptr->SetPosition(position.value);
	});

	world.system<const Position, Velocity, const ControllerPtr, const BouncePlane, const JumpSpeed>()
		.each([](const Position& pos, Velocity& vel, const ControllerPtr& controller, const BouncePlane& plane, const JumpSpeed& jump)
	{
		constexpr float planeEpsilon = 0.1f;
		if (plane.value.x * pos.value.x + plane.value.y * pos.value.y + plane.value.z * pos.value.z < plane.value.w + planeEpsilon)
		{
			if (controller.ptr->IsPressed("Jump"))
			{
				vel.value.y = jump.value;
			}
		}
	});

	world.system<ShootSpeed, Bandolier, const ControllerPtr, const BulletCreator>()
		.each([&](flecs::entity e, ShootSpeed shootSpeed, Bandolier& bandolier,
				  const ControllerPtr& controller, const BulletCreator& bulletCreator)
	{
		float dt = world.delta_time();
		bandolier.reloadTime -= dt;

		if (controller.ptr->IsPressed("Shoot") && bandolier.reloadTime < 0.0f && bandolier.shells > 0)
		{
			bulletCreator.callable(shootSpeed);
			//e.disable<BulletCreator>();
			bandolier.reloadTime = .2f;
			--bandolier.shells;
		}

		if (bandolier.shells == 0) {
			bandolier.reloadTime = 2.f;
			bandolier.shells = 6;
		}
	});
}

