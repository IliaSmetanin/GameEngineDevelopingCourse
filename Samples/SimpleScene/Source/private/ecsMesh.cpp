#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ECS/ecsSystems.h>
#include <flecs.h>
#include <Geometry.h>
#include <RenderThread.h>
#include <RenderObject.h>

using namespace GameEngine;

void RegisterEcsMeshSystems(flecs::world& world)
{
	static const EntitySystem::ECS::RenderThreadPtr* renderThread = world.get<EntitySystem::ECS::RenderThreadPtr>();

	world.system<EntitySystem::ECS::RenderObjectPtr, const Position>()
		.each([&](EntitySystem::ECS::RenderObjectPtr& renderObject, const Position& position)
	{
		if (renderObject.ptr) [[likely]]
		{
			renderObject.ptr->SetPosition(Math::Vector3f(position.x, position.y, position.z), renderThread->ptr->GetMainFrame());
		}
	});

	world.system<EntitySystem::ECS::RenderObjectPtr, Position, DespawnTimer>()
		.each([](EntitySystem::ECS::RenderObjectPtr& renderObject, Position& position, DespawnTimer& timer)
	{
		if (timer.timeToDespawn < -0.01f)
		{
			renderObject.ptr->SetPosition(Math::Vector3f(position.x, 1000.f, position.z), renderThread->ptr->GetMainFrame());
			timer.isReadyForDestruction = true;
		}
	});
}
