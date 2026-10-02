#include <Camera.h>
#include <DefaultGeometry.h>
#include <ecsControl.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <ECS/ecsSystems.h>
#include <GameFramework/GameFramework.h>
#include <Input/Controller.h>
#include <RenderObject.h>
#include <GameWorld.h>

using namespace GameEngine;

void GameFramework::Init()
{
	RegisterComponentsReflection();
	RegisterSystems();

	World::GameWorld::GetInstance()->LoadLevel(
		m_World,
		Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string()
	);

	flecs::entity camera = m_World.entity()
		.set(Position{ 0.0f, 12.0f, -10.0f })
		.set(Speed{ 10.f })
		.set(CameraPtr{ Core::g_MainCamera })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });

	flecs::entity gun = m_World.entity("Gun")
		.set(ShootPosition{ Core::g_MainCamera->GetPosition() })
		.set(ShootSpeed{ 50.f })
		.set(ShootState{ })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });
}

void GameFramework::RegisterComponentsReflection()
{
	m_World.component<Position>()
		.member<float>("x")
		.member<float>("y")
		.member<float>("z");

	m_World.component<Velocity>()
		.member<float>("x")
		.member<float>("y")
		.member<float>("z");

	m_World.component<GeometryPtr>()
		.member<uint64_t>("ptr");

	m_World.component<ControllerPtr>()
		.member<uint64_t>("ptr");

	m_World.component<Gravity>()
		.member<float>("x")
		.member<float>("y")
		.member<float>("z");

	m_World.component<BouncePlane>()
		.member<float>("x")
		.member<float>("y")
		.member<float>("z")
		.member<float>("w");

	m_World.component<Bounciness>()
		.member<float>("value");

	m_World.component<ShiverAmount>()
		.member<float>("value");

	m_World.component<FrictionAmount>()
		.member<float>("value");

	m_World.component<Speed>()
		.member<float>("value");

	m_World.component<JumpSpeed>()
		.member<float>("value");
}

void GameFramework::RegisterSystems()
{
	RegisterEcsMeshSystems(m_World);
	RegisterEcsControlSystems(m_World);
}

void GameFramework::Update(float dt)
{
	//flecs::entity gun = m_World.lookup("Gun");
	//gun.set(BulletCreator{ [&](ShootSpeed shoot_speed)
	//	{
	//		return m_World.entity()
	//			.set(Position{ Core::g_MainCamera->GetPosition() })
	//			.set(Velocity{ Core::g_MainCamera->GetViewDir().Normalized() * static_cast<float>(shoot_speed) })
	//			.set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
	//			.set(GeometryPtr{ RenderCore::DefaultGeometry::Bullet() })
	//			.set(RenderObjectPtr{ new Render::RenderObject() })
	//			.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });
	//	}
	//});
	flecs::entity gun = m_World.lookup("Gun");
	if (gun.get<ShootState>()->fired) {
		m_World.lookup("bullet")
			.set(Position{ Core::g_MainCamera->GetPosition() })
			.set(Velocity{ Core::g_MainCamera->GetViewDir().Normalized() * static_cast<float>(gun.get<ShootSpeed>()->value) })
			.set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
			.set(GeometryPtr{ RenderCore::DefaultGeometry::Bullet() })
			.set(RenderObjectPtr{ new Render::RenderObject() })
			.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });
		gun.get_mut<ShootState>()->fired = false;
	}
}