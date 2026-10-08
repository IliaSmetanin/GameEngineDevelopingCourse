// This define is essential for the scripts to expose the ECS components to lua syntax
// There is a task to rework this behavior
#define GAME_FRAMEWORK

#include <Camera.h>
#include <DefaultGeometry.h>
#include <ecsControl.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ECS/ecsSystems.h>
#include <GameFramework/GameFramework.h>
#include <Input/Controller.h>
#include <RenderObject.h>
#include <flecs.h>

using namespace GameEngine;

void GameFramework::Init()
{
	RegisterComponents();
	RegisterSystems();

	flecs::entity cubeControl = m_World.entity()
		.set(Position{ -2.f, 0.f, 0.f })
		.set(Velocity{ 0.f, 0.f, 0.f })
		.set(Speed{ 10.f })
		.set(FrictionAmount{ 0.9f })
		.set(JumpSpeed{ 10.f })
		.set(Gravity{ 0.f, -9.8065f, 0.f })
		.set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
		.set(Bounciness{ 0.3f })
		.set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });

	flecs::entity cubeMoving = m_World.entity()
		.set(Position{ 2.f, 0.f, 0.f })
		.set(Velocity{ 0.f, 3.f, 0.f })
		.set(Gravity{ 0.f, -9.8065f, 0.f })
		.set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
		.set(Bounciness{ 1.f })
		.set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() });

	flecs::entity camera = m_World.entity()
		.set(Position{ 0.0f, 12.0f, -10.0f })
		.set(Speed{ 10.f })
		.set(CameraPtr{ Core::g_MainCamera })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });

	flecs::entity cubeMovingLeft = m_World.entity()
		.set(Position{ 3.f, 5.f, -5.f })
		.set(Velocity{ -1.f, 0.f, 0.f })
		.set(DespawnTimer{ false, false, 3.f })
		.set(CollisionTraits{ false, 1.f, -0.f })
		.set(MovingOnX{ true })
		.set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() });


	flecs::entity cubeMovingRight = m_World.entity()
		.set(Position{ -3.f, 5.f, -5.f })
		.set(Velocity{ 1.f, 0.f, 0.f })
		.set(DespawnTimer{ false, false , 3.f })
		.set(CollisionTraits{ false, 1.f, -0.f })
		.set(MovingOnX{ true })
		.set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() });

	flecs::entity gun = m_World.entity("Gun")
		.set(ShootPosition{ Core::g_MainCamera->GetPosition().x, Core::g_MainCamera->GetPosition().y, Core::g_MainCamera->GetPosition().z })
		.set(ShootSpeed{ 50.f })
		.set(Reload{ -0.f })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) })
		.set(BulletCreator{ [&](ShootSpeed shoot_speed)
			{
				return m_World.entity()
					.set(Position{ Core::g_MainCamera->GetPosition().x, Core::g_MainCamera->GetPosition().y, Core::g_MainCamera->GetPosition().z })
					.set(Velocity{ Core::g_MainCamera->GetViewDir().Normalized().x * shoot_speed.value,
								   Core::g_MainCamera->GetViewDir().Normalized().y * shoot_speed.value,
								   Core::g_MainCamera->GetViewDir().Normalized().z * shoot_speed.value })
					.set(Gravity{ 0.f, -9.8065f, 0.f })
					.set(BouncePlane{ 0.f, 1.f, 0.f, 5.f })
					.set(Bounciness{ .3f })
					//.set(Despawn{ false, 5.f })
					.set(CollisionTraits{ 0, .15f, -0.f })
					.set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Bullet() })
					.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() });
					//.set(Bullet{ true });
			}
		});

	flecs::entity cubeTarget = m_World.entity()
		.set(Position{ 0.f, 15.f, 0.f })
		.set(Velocity{ 0.f, 0.f, 0.f })
		.set(CollisionTraits{ false, 1.f, -0.f })
		.set(Target{ true })
		.set(EntitySystem::ECS::GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(EntitySystem::ECS::RenderObjectPtr{ new Render::RenderObject() });
}

void GameFramework::RegisterComponents()
{
	// Exposing these components for the lua system
	ECS_META_COMPONENT(m_World, Position);
	ECS_META_COMPONENT(m_World, Velocity);
	ECS_META_COMPONENT(m_World, Gravity);
	ECS_META_COMPONENT(m_World, BouncePlane);
	ECS_META_COMPONENT(m_World, Bounciness);
	ECS_META_COMPONENT(m_World, ShiverAmount);
	ECS_META_COMPONENT(m_World, FrictionAmount);
	ECS_META_COMPONENT(m_World, Speed);
	ECS_META_COMPONENT(m_World, DespawnTimer);
	ECS_META_COMPONENT(m_World, CollisionTraits);
	ECS_META_COMPONENT(m_World, MovingOnX);
	ECS_META_COMPONENT(m_World, ShootPosition);
	ECS_META_COMPONENT(m_World, ShootSpeed);
	ECS_META_COMPONENT(m_World, Target);
}

void GameFramework::RegisterSystems()
{
	RegisterEcsMeshSystems(m_World);
	RegisterEcsControlSystems(m_World);
}

void GameFramework::Update(float dt)
{

}