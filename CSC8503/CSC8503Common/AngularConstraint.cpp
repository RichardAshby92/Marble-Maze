#include "AngularConstraint.h"
#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

void AngularConstraint::UpdateConstraint(float dt) {

	Vector3 b = objectB->GetTransform().GetOrientation().ToEuler();
	Vector3 a = objectA->GetTransform().GetOrientation().ToEuler();

	Vector3 orientationOffset = b - a;

	Vector3 equalityConstraint = orientationOffset - difference.ToEuler();

	if (orientationOffset.Length() > difference.ToEuler().Length()) {
		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();

		Vector3 constraintInertia = physA->GetInertiaTensor().GetDiagonal() + physB->GetInertiaTensor().GetDiagonal();

		if (constraintInertia.LengthSquared() > 0.0f) {
			Vector3 relativeAngularVelocity = physA->GetAngularVelocity() - physB->GetAngularVelocity();

			float J = Vector3::Dot(relativeAngularVelocity, orientationOffset.Normalised());
			Vector3 axsis = Vector3::Cross(relativeAngularVelocity, orientationOffset.Normalised());

			//Lagrange
			float biasFactor = 0.001f;
			Vector3 biasVector = { 0.01, 0.01, 0.01 };

			float biasX = -(biasFactor / dt) * equalityConstraint.x; //dot product two angles; // chnage with value passed in
			float biasY = -(biasFactor / dt) * equalityConstraint.y; // chnage with value passed in
			float biasZ = -(biasFactor / dt) * equalityConstraint.z; // chnage with value passed in

			Vector3 bias = -(biasVector / dt) * equalityConstraint;

			float lambdaX = -(J)  / constraintInertia.x;
			float lambdaY = -(J) / constraintInertia.y;
			float lambdaZ = -(J)  / constraintInertia.z;

			Vector3 lambda = -(axsis + bias) / constraintInertia;

			//Impulse
			//Vector3 aAngularImpulse = orientationOffset.Normalised()* Vector3({ lambdaX, lambdaY, lambdaZ });
			//Vector3 bAngularImpulse = -orientationOffset.Normalised()* Vector3({ lambdaX, lambdaY, lambdaZ });
			Vector3 aAngularImpulse = orientationOffset.Normalised() * lambda;
			Vector3 bAngularImpulse = - orientationOffset.Normalised() * lambda;

			physA->ApplyAngularImpulse(aAngularImpulse);
			physB->ApplyAngularImpulse(bAngularImpulse);
		}
	}
}
