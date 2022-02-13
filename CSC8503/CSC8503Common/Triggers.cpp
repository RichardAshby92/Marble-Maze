#include "Triggers.h"
#include "CollisionDetection.h"

using namespace NCL;
using namespace CSC8503;

void Triggers::Update(float dt) {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	gameWorld.GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		if ((*i)->GetTrigger() == false) {
			continue;
		}
		for (auto j = i + 1; j != last; ++j) {
			if ((*j)->GetPhysicsObject() == nullptr) {
				continue;
			}
			CollisionDetection::CollisionInfo info;
			if (CollisionDetection::ObjectIntersection(*i, *j, info)) {
				if (info.a->GetTrigger() && info.b->GetTrigger()) {
					ResolveTriggers(info.a, info.b);
					return;
				}
			}
		}
	}
}

void Triggers::ResolveTriggers (GameObject* a, GameObject* b) {
	if (a->GetTriggerType() == 1 && b->GetTriggerType() == 2) {
		std::cout << "score increased \n";
		score++;		
		gameWorld.RemoveGameObject(b);
		return;
	}

	if (a->GetTriggerType() == 2 && b->GetTriggerType() == 1) {
		std::cout << "score increased \n";
		score++;
		gameWorld.RemoveGameObject(a);
		return;
	}

	if (a->GetTriggerType() == 1 && b->GetTriggerType() == 3) {
		std::cout << "Victory";
		winTrigger = true;
		return;
	}

	if (a->GetTriggerType() == 3 && b->GetTriggerType() == 1) {
		std::cout << "Victory";
		winTrigger = true;
		return;
	}

	if (a->GetTriggerType() == 1 && b->GetTriggerType() == 4) {
		std::cout << "Lost";
		looseTrigger = true;
		return;
	}

	if (a->GetTriggerType() == 4 && b->GetTriggerType() == 1) {
		std::cout << "Lost";
		looseTrigger = true;
		return;
	}
}