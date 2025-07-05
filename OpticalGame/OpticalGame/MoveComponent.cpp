#include "MoveComponent.h"
#include "Actor.h"

MoveComponent::MoveComponent(Actor* owner, int updateOrder)
	: Component(owner, updateOrder)
{
}

MoveComponent::~MoveComponent()
{

}

void MoveComponent::Update(float deltaTime)
{

}

void MoveComponent::ProcessInput(float deltaTime)
{

}