#include "SpringConstraint.h"
#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

void SpringConstraint::UpdateConstraint(float dt) {

	float positionOffset = objectB->GetTransform().GetPosition().z
		- objectA->GetTransform().GetPosition().z;
	
	float equalityConstraint = positionOffset - difference;

	if (abs(equalityConstraint) > 0.0) {
		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();
		float constraintMass = physA->GetInverseMass() + physB->GetInverseMass();

		if (constraintMass > 0.0) {
			
			float springConstant = 0.1;

			Vector3 ForceA = {0,0, springConstant * equalityConstraint };
			Vector3 ForceB = {0, 0, -(springConstant * equalityConstraint) };

			physA->ApplyLinearImpulse(ForceA * dt);
			physB->ApplyLinearImpulse(ForceB * dt);

		}
	}
}
