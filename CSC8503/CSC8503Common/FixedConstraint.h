#pragma once
#include "Constraint.h"
#include "Transform.h"

namespace NCL {
	namespace CSC8503 {
		class GameObject;

		class FixedConstraint : public Constraint {
		public:
			FixedConstraint(GameObject* a, GameObject* b, Vector3 r1, Vector3 r2) {
				objectA = a;
				objectB = b;
				distanceA = r1;
				distanceB = r2;
			}
			~FixedConstraint() {}

			void UpdateConstraint(float dt) override;

		protected:
			GameObject* objectA;
			GameObject* objectB;
			Vector3 distanceA;
			Vector3 distanceB;
		};
	}
}
