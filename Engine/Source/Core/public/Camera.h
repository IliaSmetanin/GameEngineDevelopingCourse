#pragma once

#include <Core/export.h>
#include <Matrix.h>
#include <Vector.h>

namespace GameEngine
{
	namespace Core
	{
		class CORE_API Camera final
		{
		public:
			Camera() = default;

		public:
			Math::Matrix4x4f GetViewMatrix();
			Math::Vector3f GetPosition() const { return m_Position; }
			void SetPosition(Math::Vector3f position) { m_Position = position; }
			Math::Vector3f GetViewDir() const { return m_ViewDir; }
			void SetViewDir(Math::Vector3f viewDir) { m_ViewDir = viewDir; }
			void Rotate(float yaw, float pitch);
			void Move(Math::Vector3f translation) { m_Position += translation; }
			void Reset() noexcept;
			Math::Vector3f GetForwardDir() const { return m_ViewDir * kCameraMoveSpeed;
			}
			Math::Vector3f GetRightDir() const { return Math::Vector3f(m_ViewDir.z, 0.0, -(m_ViewDir.x)) * kCameraMoveSpeed; }

		private:
			inline static const float kCameraMoveSpeed = 2.0;

			Math::Vector3f m_Position;
			Math::Vector3f m_ViewDir;
		};

		extern CORE_API Camera* g_MainCamera;
	}
}