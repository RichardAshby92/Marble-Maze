#include "FixedConstraint.h"
#include "GameObject.h"

using namespace NCL;
using namespace CSC8503;

void FixedConstraint::UpdateConstraint(float dt) {
	Vector3 x1 = objectA->GetTransform().GetPosition();
	Vector3 x2 = objectB->GetTransform().GetPosition();

	Vector3 translastionConstraint = x2 + distanceB + distanceB - x1 - distanceA;
	Vector3 rotationConstraint = objectB->GetTransform().GetOrientation().ToEuler() - 
		objectA->GetTransform().GetOrientation().ToEuler();

	if (abs(translastionConstraint.LengthSquared() > 0.0f)) {
		//Jtrans
		float elementsR1[9] = { 0.0f, -distanceA.z, distanceA.y, distanceA.z, 0.0f, -distanceA.x, -distanceA.y, distanceA.x, 0.0f };
		float elementsR2[9] = { 0.0f, -distanceB.z, distanceB.y, distanceB.z, 0.0f, -distanceB.x, -distanceB.y, distanceB.x, 0.0f };
		Matrix3 E1;
		Matrix3 distanceMatrixA = elementsR1;
		Matrix3 E2;
		Matrix3 distanceMatrixB = elementsR2;
		
		//use cross product of Vector3s
		//Why not use translation constraint
		Vector3 Ctrans1 = Vector3::Cross(E1.ToEuler(), objectA->GetPhysicsObject()->GetLinearVelocity());
		Vector3 Ctrans2 = Vector3::Cross(distanceMatrixA.ToEuler(), objectA->GetPhysicsObject()->GetAngularVelocity());
		Vector3 Ctrans3 = Vector3::Cross(E2.ToEuler(), objectB->GetPhysicsObject()->GetLinearVelocity());
		Vector3 Ctrans4 = Vector3::Cross(distanceMatrixB.ToEuler(), objectB->GetPhysicsObject()->GetAngularVelocity());

		float biasFactor = 0.01;
		//float bias = Vector3::Dot((biasFactor / dt), translastionConstraint); //Should be a vector

		//K = 

		//Ctrans + bias
		//Find K
		//Lambda = Times K by ctrans + bias
		//Impulse = lambda * direction (normalised differences in positions)  

		//Matrix4 Jtrans1 = { -E1, distanceA, E1, -distanceB };
		//Relative Velocities
		//Ctrans JTrans * relative Velocities
	}

	if (abs(rotationConstraint.LengthSquared() > 0.0f)) {
		//Jrot
		//Relative Velocities
		//Crot
	}
}