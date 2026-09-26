#include <random>

#include <Camera.h>
#include <DefaultGeometry.h>
#include <Game.h>
#include <GameObject.h>
#include <Input/InputHandler.h>

namespace GameEngine
{
	Game::Game(
		std::function<bool()> PlatformLoopFunc
	) :
		PlatformLoop(PlatformLoopFunc)
	{
		Core::g_MainCamera = new Core::Camera();
		Core::g_MainCamera->SetPosition(Math::Vector3f(0.0f, 6.0f, -6.0f));
		Core::g_MainCamera->SetViewDir(Math::Vector3f(0.0f, -6.0f, 6.0f).Normalized());

		m_renderThread = std::make_unique<Render::RenderThread>();

		static std::mt19937 generator(std::random_device{}());
		std::uniform_int_distribution<uint32_t> distance(0, 2);

		// How many objects do we want to create
		for (int i = 0; i < 100; ++i)
		{
			m_Objects.push_back(new GameObject());

			m_Objects[i]->SetPosition(Math::Vector3f(static_cast<float>(4 * (i / 10)), static_cast<float>(4 * (i % 10)), 0.0f), m_renderThread->GetMainFrame());

			if (static_cast<GameEngine::GameObject::ObjectType>(distance(generator)) == GameEngine::GameObject::ObjectType::Jumping) {
				m_Objects[i]->SetType(GameEngine::GameObject::ObjectType::Jumping);
			}
			else if (static_cast<GameEngine::GameObject::ObjectType>(distance(generator)) == GameEngine::GameObject::ObjectType::Fluctuating)
			{
				m_Objects[i]->SetType(GameEngine::GameObject::ObjectType::Fluctuating);
				Math::Vector3f pos = m_Objects[i]->GetPosition();
				pos.z += 5.0f;
				m_Objects[i]->SetPosition(pos, m_renderThread->GetMainFrame());
			}
			else if (static_cast<GameEngine::GameObject::ObjectType>(distance(generator)) == GameEngine::GameObject::ObjectType::Movable)
			{
				m_Objects[i]->SetType(GameEngine::GameObject::ObjectType::Movable);
			}

			Render::RenderObject** renderObject = m_Objects.back()->GetRenderObjectRef();
			m_renderThread->EnqueueCommand(Render::ERC::CreateRenderObject, RenderCore::DefaultGeometry::Cube(), renderObject);
		}

		Core::g_InputHandler->RegisterCallback("GoForward", []() { Core::g_MainCamera->Move(Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoBack", []() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetViewDir()); });
		Core::g_InputHandler->RegisterCallback("GoRight", []() { Core::g_MainCamera->Move(Core::g_MainCamera->GetRightDir()); });
		Core::g_InputHandler->RegisterCallback("GoLeft", []() { Core::g_MainCamera->Move(-Core::g_MainCamera->GetRightDir()); });
		Core::g_InputHandler->RegisterCallback("MoveObjectRight", [this]() { for (size_t i = 0; i < this->m_Objects.size(); ++i) {
			if (m_Objects[i]->GetType() == GameEngine::GameObject::ObjectType::Movable) {
				m_Objects[i]->Move(Math::Vector3f(1.0f, 0.0f, 0.0f));
			}
			}; });
		Core::g_InputHandler->RegisterCallback("MoveObjectLeft", [this]() { for (size_t i = 0; i < this->m_Objects.size(); ++i) {
			if (m_Objects[i]->GetType() == GameEngine::GameObject::ObjectType::Movable) {
				m_Objects[i]->Move(Math::Vector3f(-1.0f, 0.0f, 0.0f));
			}
		}; });
	}

	void Game::Run()
	{
		assert(PlatformLoop != nullptr);

		m_GameTimer.Reset();

		bool quit = false;
		while (!quit)
		{
			m_GameTimer.Tick();
			float dt = m_GameTimer.GetDeltaTime();

			Core::g_MainWindowsApplication->Update();
			Core::g_InputHandler->Update();
			Core::g_MainCamera->Update(dt);

			Update(dt);

			m_renderThread->OnEndFrame();

			// The most common idea for such a loop is that it returns false when quit is required, or true otherwise
			quit = !PlatformLoop();
		}
	}

	void Game::Update(float dt)
	{
		for (int i = 0; i < m_Objects.size(); ++i)
		{
			//Math::Vector3f pos = m_Objects[i]->GetPosition();

			// Showcase
			if (m_Objects[i]->GetType() == GameEngine::GameObject::ObjectType::Jumping)
			{
				m_Objects[i]->Jump(static_cast<float>(4 * (i % 10)), dt);
			}
			else if (m_Objects[i]->GetType() == GameEngine::GameObject::ObjectType::Fluctuating)
			{
				static Math::Vector3f equilibrium_point(static_cast<float>(4 * (i / 10)), static_cast<float>(4 * (i % 10)), 0.0f);
				m_Objects[i]->ForwardBackward(dt, equilibrium_point);
			}
			else if (m_Objects[i]->GetType() == GameEngine::GameObject::ObjectType::Movable)
			{
				//pos.x += 0.5f * dt;
				//pos.y -= 0.5f * dt;
				m_Objects[i]->Update(dt);
			}
			m_Objects[i]->SetPosition(m_Objects[i]->GetPosition(), m_renderThread->GetMainFrame());
			//m_Objects[i]->SetPosition(pos, m_renderThread->GetMainFrame());
		}
	}
}