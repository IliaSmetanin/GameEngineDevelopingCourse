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
		enum class ObjectType {
			Jumping, Fluctuating, Movable
		};

		inline static const float kEpsilon = 1e-7f;
		inline static const float kJumpSpeed = 10.0f;
		inline static const float kGravityAcceleration = 9.81f;

		inline static const float kStartOffset = 1.0;
		inline static const float kFluctuationAcceleration = 5.0f;

		inline static const float kMoveSpeed = 10.0f;


		Render::RenderObject** GetRenderObjectRef() { return &m_RenderObject; }

		void Jump(float y_floor, float dt)
		{
			if (m_Position.y < y_floor) {
				m_Position.y = y_floor;
				m_SpeedVector.y = 0.0f;
			}

			if (std::fabs(m_Position.y - y_floor) < kEpsilon) {
				m_SpeedVector.y = 10.0f;
			}

			m_Position.y += m_SpeedVector.y * dt - 0.5f * kGravityAcceleration * dt * dt;
			m_SpeedVector.y -= kGravityAcceleration * dt;
 		}

		void ForwardBackward(float dt, Math::Vector3f equilibrium_point)
		{
			float offset = m_Position.z - equilibrium_point.z;
			float acceleration = -kFluctuationAcceleration * offset;

			m_Position.z += m_SpeedVector.z * dt + 0.5 * acceleration * dt * dt;
			m_SpeedVector.z += acceleration * dt;
		}

		void Move(Math::Vector3f dir)
		{
			m_CurrentMoveDir = m_CurrentMoveDir + dir.Normalized();
		}

		void Update(float dt)
		{
			m_Position = m_Position + m_CurrentMoveDir.Normalized() * kMoveSpeed * dt;
			m_CurrentMoveDir = Math::Vector3f::Zero();
		}

		void SetType(ObjectType type)
		{
			m_Type = type;
		}

		ObjectType GetType() const noexcept {
			return m_Type;
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
		ObjectType m_Type;

		Render::RenderObject* m_RenderObject = nullptr;

		Math::Vector3f m_Position = Math::Vector3f::Zero();
		Math::Vector3f m_SpeedVector = Math::Vector3f::Zero();
		//Math::Vector3f m_AccelVector = Math::Vector3f::Zero()

		Math::Vector3f m_CurrentMoveDir = Math::Vector3f::Zero();
	};
}