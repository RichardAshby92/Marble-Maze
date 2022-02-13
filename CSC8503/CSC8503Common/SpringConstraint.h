#pragma once
#pragma once
#include "Constraint.h"
#include "../CSC8503Common/Transform.h"

namespace NCL {
	namespace CSC8503 {
		class GameObject;

		class SpringConstraint : public Constraint {
		public:
			SpringConstraint(GameObject* a, GameObject* b, float relaxedLength) {
				objectA = a;
				objectB = b;
				difference = relaxedLength;
			}
			~SpringConstraint() {}

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			GameObject* objectB;
			float difference;
		};
	}
}