#include <Camera.h>
#include <DefaultGeometry.h>
#include <ecsControl.h>
#include <ecsMesh.h>
#include <ecsPhys.h>
#include <GameFramework/GameFramework.h>
#include <Input/Controller.h>
#include <RenderObject.h>

using namespace GameEngine;

void GameFramework::Init()
{
	RegisterEcsMeshSystems(m_World);
	RegisterEcsControlSystems(m_World);
	RegisterEcsPhysSystems(m_World);

	flecs::entity cubeControl = m_World.entity()
		.set(Position{ Math::Vector3f(-2.f, 0.f, 0.f) })
		.set(Velocity{ Math::Vector3f(0.f, 0.f, 0.f) })
		.set(Speed{ 10.f })
		.set(FrictionAmount{ 0.9f })
		.set(JumpSpeed{ 10.f })
		.set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
		.set(BouncePlane{ Math::Vector4f(0.f, 1.f, 0.f, 5.f) })
		.set(Bounciness{ 0.3f })
		.set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(RenderObjectPtr{ new Render::RenderObject() })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });

	flecs::entity cubeMoving = m_World.entity()
		.set(Position{ Math::Vector3f(2.f, 0.f, 0.f) })
		.set(Velocity{ Math::Vector3f(0.f, 3.f, 0.f) })
		.set(Gravity{ Math::Vector3f(0.f, -9.8065f, 0.f) })
		.set(BouncePlane{ Math::Vector4f(0.f, 1.f, 0.f, 5.f) })
		.set(Bounciness{ 1.f })
		.set(GeometryPtr{ RenderCore::DefaultGeometry::Cube() })
		.set(RenderObjectPtr{ new Render::RenderObject() });

	flecs::entity camera = m_World.entity()
		.set(Position{ Math::Vector3f(0.0f, 12.0f, -10.0f) })
		.set(Speed{ 10.f })
		.set(CameraPtr{ Core::g_MainCamera })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });

	flecs::entity gun = m_World.entity("Gun")
		.set(ShootPosition{ Core::g_MainCamera->GetPosition() })
		.set(ShootSpeed{ 50.f })
		.set(ShootState{ })
		.set(ControllerPtr{ new Core::Controller(Core::g_FileSystem->GetConfigPath("Input_default.ini")) });
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