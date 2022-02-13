#include "PositionConstraint.h"
#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

void PositionConstraint::UpdateConstraint(float dt) {
	Vector3 relativePos = objectA->GetTransform().GetPosition() - objectB->GetTransform().GetPosition();
	float currentDistance = relativePos.Length();
	float equalityConstraint = distance - currentDistance;

	if (abs(equalityConstraint) > 0.0f) {
		Vector3 offsetDir = relativePos.Normalised();

		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();

		Vector3 relativeVelocity = physA->GetLinearVelocity() - physB->GetLinearVelocity();

		float K = physA->GetInverseMass() + physB->GetInverseMass();

		if (K > 0.0f) {
			float velocityDot = Vector3::Dot(relativeVelocity, offsetDir);
			float biasFactor = 0.101f;
			float bias = -(biasFactor / dt) * equalityConstraint;
			float lambda = -(velocityDot + bias) / K;

			Vector3 aImpulse = offsetDir * lambda;
			Vector3 bImpulse = -offsetDir * lambda;

			physA->ApplyLinearImpulse(aImpulse);
			physB->ApplyLinearImpulse(bImpulse);
		}
	}
}

