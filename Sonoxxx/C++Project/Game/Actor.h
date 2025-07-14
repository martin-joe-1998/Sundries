#pragma once
#include <vector>
#include <memory>

#include "Utility/Math.h"

namespace sl
{
	class Game;
	class SceneBase;
	class Component;
	
	class SpriteComponent; // NowTodo: kesu

	class Actor : public std::enable_shared_from_this<Actor> // shared_ptr管理用
	{

	public:
		enum State
		{
			Active,
			Paused,
			Dead,
		};

		Actor(Game* game);
		virtual ~Actor();

		virtual void Initialize();

		void Update(float deltaTime);
		void UpdateComponents(float deltaTime);
		void UpdateActor(float deltaTime);


		// void ProcessInput()
		// virtual void ActorInput();

		void SetPosition(const Vector3& pos);
		void SetRotation(const Vector3& rot);
		void SetScale(const Vector3& scl);

		const Vector3& GetPosition() const;
		const Vector3& GetRotation() const;
		const Vector3& GetScale() const;

		// void CalcWorldTransform();
		// const Matrix4& GetWorldTransform() const { return mWorldTransform; }

		State GetState() const { return mState; }
		void SetState(State state) { mState = state; }

		Game* GetGame() const { return mGame; }



		void AddComponent(Component* component);
		void RemoveComponent(Component* component);
		//const std::vector<Component*>& GetComponents() const { return mComponents; }
		
		//// コンポーネントを取得
		//template <typename T>
		//T* GetComponent()
		//{
		//	for (auto& comp : mComponents)
		//	{
		//		if (T* casted = dynamic_cast<T*>(comp))
		//		{
		//			return casted;
		//		}
		//	}
		//	return nullptr; // 見つからなかった場合はnullptrを返す
		//}

	protected:
		
	private:
		Game* mGame;
		State mState;

		Vector3 mPosition;
		Vector3 mRotation;
		Vector3 mScale;
		// Matrix4 mWorldTransform;

		std::vector<Component*> mComponents;


		std::shared_ptr<SpriteComponent> mSprite;
	};
}

