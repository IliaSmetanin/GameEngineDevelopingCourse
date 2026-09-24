#pragma once

#include <RenderObject.h>
#include <RenderThread.h>
#include <Vector.h>

namespace GameEngine
{
	class GameObject final
	{
	public:
		GameObject() = default;

	public:
		inline static const float kJumpSpeed = 10.0f;
		inline static const float kGravityAcceleration = 9.81f;
		inline static const float kEpsilon = 1e-7;

		Render::RenderObject** GetRenderObjectRef() { return &m_RenderObject; }

		void Jump(float dt)
		{
			if (m_Position.y < 0.0f) {
				m_Position.y = m_SpeedVector.y = 0.0f;
			}

			if (std::fabs(m_Position.y - 0.0f) < kEpsilon) {
				m_SpeedVector.y = 10.0f;
			}

			m_Position.y += m_SpeedVector.y * dt - 0.5f * kGravityAcceleration * dt * dt;
			m_SpeedVector.y -= kGravityAcceleration * dt;
 		}

		void SetPosition(Math::Vector3f position, size_t frame)
		{
			m_Position = position;

			if (m_RenderObject) [[likely]]
			{
				m_RenderObject->SetPosition(position, frame);
			}
		}

		Math::Vector3f GetPosition()
		{
			return m_Position;
		}

	protected:
		Render::RenderObject* m_RenderObject = nullptr;

		Math::Vector3f m_Position = Math::Vector3f::Zero();
		Math::Vector3f m_SpeedVector = Math::Vector3f::Zero();
		//Math::Vector3f m_AccelVector = Math::Vector3f::Zero()
	};
}