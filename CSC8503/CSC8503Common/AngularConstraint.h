#pragma once
#include "Constraint.h"
#include "../CSC8503Common/Transform.h"


namespace NCL {
	namespace CSC8503 {
		class GameObject;

		class AngularConstraint : public Constraint {
		public:
			AngularConstraint(GameObject* a, GameObject* b, Quaternion diff) {
				objectA = a;
				objectB = b;
				difference = diff;
			}
			~AngularConstraint() {}

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			GameObject* objectB;
			Quaternion difference;
		};
	}
}