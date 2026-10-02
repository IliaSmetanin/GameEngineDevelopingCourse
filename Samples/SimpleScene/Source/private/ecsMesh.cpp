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

	world.system<const GeometryPtr, RenderObjectPtr>()
		.each([&](flecs::entity e, const GeometryPtr& geometry, RenderObjectPtr& renderObject)
	{
		renderThread->ptr->EnqueueCommand(Render::ERC::CreateRenderObject, geometry.ptr, renderObject.ptr);
		e.remove<GeometryPtr>();
	});

	world.system<RenderObjectPtr, const Position>()
		.each([&](RenderObjectPtr& renderObject, const Position& position)
	{
		renderObject.ptr->SetPosition(position.value, renderThread->ptr->GetMainFrame());
	});

	world.system<RenderObjectPtr, Position, Despawn>()
		.each([&](flecs::entity e, RenderObjectPtr& renderObject, Position& pos, Despawn& despawn)
	{
		if (despawn.triggered) {
			float dt = world.delta_time();
			despawn.timeToDespawn -= dt;
		}

		if (despawn.timeToDespawn < 0.01f) {
			pos.value.y = 1000.f;
			renderObject.ptr->SetPosition(pos.value, renderThread->ptr->GetMainFrame());
		}

		if (despawn.timeToDespawn < -0.f) {
			e.destruct();
		}
	});
}


