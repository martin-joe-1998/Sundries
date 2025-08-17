#pragma once
#include <vector>
#include <memory>
#include <string>

#include "Utility/Math.h"

namespace sl
{
	class Game;
	class SceneBase;
	class Component;
	
	class Actor : public std::enable_shared_from_this<Actor> // shared_pträ«óùóp
	{

	public:
		enum State
		{
			EActive,
			EPaused,
			EDead,
		};

		Actor(Game* game);
		virtual ~Actor();

		virtual void Initialize();

		void Update(float deltaTime);
		void UpdateComponents(float deltaTime);
		virtual void UpdateActor(float deltaTime);


		// void ProcessInput()
		// virtual void ActorInput();

		void SetPosition(const Vector3& pos);
		void SetRotation(const Vector3& rot);
		void SetScale(const Vector3& scl);

		const Vector3& GetPosition() const;
		const Vector3& GetRotation() const;
		const Vector3& GetScale() const;

		Matrix4 GetWorldTransform() const;

		State GetState() const { return mState; }
		void SetState(State state) { mState = state; }

		Game* GetGame() const { return mGame; }

		// Blend State
		const std::string& GetRenderType() const { return mRenderType; }
		void SetRenderType(const char* rt) { mRenderType = rt; }

		void AddComponent(Component* component);
		void RemoveComponent(Component* component);

		template <typename T>
		T* GetComponent() const
		{
			for (auto* comp : mComponents)
			{
				if (auto* casted = dynamic_cast<T*>(comp)) // Todo: typeid
				{
					return casted;
				}
			}
			return nullptr;
		}

	protected:
		
	private:
		Game* mGame;
		State mState;

		Vector3 mPosition;
		Vector3 mRotation;
		Vector3 mScale;

		std::vector<Component*> mComponents;

		// Blend State
		std::string mRenderType = "Null";
	};
}

