#pragma once

#include <flecs.h>

namespace GameEngine::Core
{
	class Camera;
}

namespace GameEngine::EntitySystem::EditorECS
{
	struct CameraPtr
	{
		Core::Camera* ptr;
	};

	struct Position
	{
		float x;
		float y;
		float z;
	};

	struct Speed
	{
		float value;
	};

	//struct RenderObjectPtr
	//{
	//	GameEngine::Render::RenderObject* ptr = nullptr;

	//	GameEngine::Render::RenderObject** GetAddressOf()
	//	{
	//		return &ptr;
	//	}
	//};

	//struct GeometryPtr
	//{
	//	GameEngine::RenderCore::Geometry* ptr;
	//};

	void RegisterEditorEcsControlSystems(flecs::world& world);
}
