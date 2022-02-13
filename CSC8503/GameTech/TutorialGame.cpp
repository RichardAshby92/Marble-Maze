#include "TutorialGame.h"
#include "../CSC8503Common/GameWorld.h"
#include "../CSC8503Common/PositionConstraint.h"
#include "../CSC8503Common/AngularConstraint.h"
#include "../CSC8503Common/SpringConstraint.h"
#include "../CSC8503Common/Triggers.h"

#include "../../Plugins/OpenGLRendering/OGLMesh.h"
#include "../../Plugins/OpenGLRendering/OGLShader.h"
#include "../../Plugins/OpenGLRendering/OGLTexture.h"
#include "../../Common/TextureLoader.h"

#include <iomanip>
#include <sstream>

using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
	renderer	= new GameTechRenderer(*world);
	physics		= new PhysicsSystem(*world);
	trigger		= new Triggers(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	time = 0;

	initLevel1 = false;
	initLevel2 = false;

	sweeperStart = nullptr;
	sweeperEnd = nullptr;
	

	Debug::SetRenderer(renderer);

	InitialiseAssets();
	
}

TutorialGame::~TutorialGame() {
	delete cubeMesh;
	delete sphereMesh;
	delete charMeshA;
	delete charMeshB;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/


void TutorialGame::InitialiseAssets() {
	auto loadFunc = [](const string& name, OGLMesh** into) {
		*into = new OGLMesh(name);
		(*into)->SetPrimitiveType(GeometryPrimitive::Triangles);
		(*into)->UploadToGPU();
	};

	loadFunc("cube.msh"		 , &cubeMesh);
	loadFunc("sphere.msh"	 , &sphereMesh);
	loadFunc("Male1.msh"	 , &charMeshA);
	loadFunc("courier.msh"	 , &charMeshB);
	loadFunc("security.msh"	 , &enemyMesh);
	loadFunc("coin.msh"		 , &bonusMesh);
	loadFunc("capsule.msh"	 , &capsuleMesh);

	basicTex	= (OGLTexture*)TextureLoader::LoadAPITexture("checkerboard.png");
	basicShader = new OGLShader("GameTechVert.glsl", "GameTechFrag.glsl");

	InitCamera();
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(1000.0f);
	world->GetMainCamera()->SetPitch(0.0f);
	world->GetMainCamera()->SetYaw(0.0f);
	world->GetMainCamera()->SetPosition(Vector3(0, 0, 250));
	lockedObject = nullptr;
}

void TutorialGame::InitMenu() {
	world->ClearAndErase();
	physics->Clear();

	sweeperStart = nullptr;
	sweeperEnd = nullptr;

	bool initLevel1 = false;
	bool initLevel2 = false;
}

void TutorialGame::InitLevel1() {
	world->ClearAndErase();
	physics->Clear();
	physics->UseGravity(true);	
	
	time = 0.0;

	GameObject* characterSphere = AddSphereToWorld({ 0,50,0 }, 10, 40);
	characterSphere->GetPhysicsObject()->SetResitution(0.8);
	characterSphere->GetPhysicsObject()->SetFriction(0.3);
	characterSphere->SetTrigger(1);
	characterSphere->SetTriggerType(1);

	AddBonusToWorld({ 30, 5, 0 });

	TwistMaze({ 0,0,0 });
	Ramp({ 125, -120, -60 });
	Piston({125, -260, -210 });
	Sweeper({125, -360, -430});
	
	GameObject* finalPlatform = AddCubeToWorld({125, -460, -430}, {200, 2, 200}, false, 0);
	finalPlatform->SetTrigger(1);
	finalPlatform->SetTriggerType(3);

	GameObject* killPlane = AddCubeToWorld({ 0,-1000,0 }, { 2000, 2, 2000 }, false, 0);
	killPlane->SetRenderObject(0);
	killPlane->SetTrigger(1);
	killPlane->SetTriggerType(4);

	bool initLevel1 = true;
	bool initLevel2 = false;
}

void TutorialGame::InitLevel2() {
	world->ClearAndErase();
	physics->Clear();
	physics->UseGravity(true);

	time = 0.0;

	Level2Maze();
	character = AddSphereToWorld({ 0,3,0 }, 0.4, 10);

	//character
	//enemies
	//powerups
	//victoryTrigger
	bool initLevel1 = false;
	bool initLevel2 = true;
}

//Test Scenes - controlled with F keys
void TutorialGame::InitTestWorld() {
	world->ClearAndErase();
	physics->Clear();

	InitMixedGridWorld(5, 5, 3.5f, 3.5f);
	InitGameExamples();
	InitDefaultFloor();
	BridgeConstraintTest();
}

void TutorialGame::InitEmptyWorld() {
	world->ClearAndErase();
	physics->Clear();
	GameObject* sphere = AddSphereToWorld({0, 20, 4}, 10, 1);
	GameObject* floor = AddCubeToWorld({ 0,0,0 }, {100, 2, 100}, true, 0);
	floor->GetTransform().SetOrientation(Quaternion::EulerAnglesToQuaternion(0, 0, -45 ));
}

void TutorialGame::InitTwoSpheres() {
	world->ClearAndErase();
	physics->Clear();

	GameObject* cube1 = AddCubeToWorld({ 20, 100, 0 }, {4, 4, 4}, true,  10);
	GameObject* cube2 = AddCubeToWorld({ -20, 100, 0 }, {4,4,4}, true, 10);
	PositionConstraint* constraint = new PositionConstraint(cube1, cube2, 40);
	world->AddConstraint(constraint);
	AngularConstraint* angularConstraint = new AngularConstraint(cube1, cube2, Quaternion({ 0.05, 0.05, 0.05 }, 1));
	world->AddConstraint(angularConstraint);
}

void TutorialGame::InitPistonTest() {
	world->ClearAndErase();
	physics->Clear();
	physics->UseGravity(false);
	
	GameObject* testfloor = AddCubeToWorld({ 0, 0 ,0 }, { 100, 2, 100 }, false, 0);
	GameObject* testcube = AddCubeToWorld({ 0, 10, 0 }, {2, 2, 2}, false, 50);
	SpringConstraint* spring = new SpringConstraint(testfloor, testcube, 10);
	world->AddConstraint(spring);
}

//Updates and Keys
void TutorialGame::UpdateGame(float dt) {
	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}

if (sweeperStart) {
	sweeperStart->Update(dt); }

if (sweeperEnd) {
	sweeperEnd->Update(dt); }	

	UpdateKeys();

	if(trigger->GetWinTrigger() == false && trigger->GetLooseTrigger() == false){
		time += dt;
		std::stringstream stream;
		stream.precision(3);
		stream << time;
		timePrint = stream.str();
	}



	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(5, 95));
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95));
	}

	if (reverseGravity) {
		Debug::Print("Gravity Reversed!", Vector2(5, 90));
	}
	else { Debug::Print("Gravity Normal", Vector2(5, 90)); }

	SelectObject();
	MoveSelectedObject();
	physics->Update(dt);

	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0,1,0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}

	world->UpdateWorld(dt);
	renderer->Update(dt);


	Debug::FlushRenderables(dt);
	renderer->Render();
	trigger->Update(dt);
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitTestWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
		lockedObject	= nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F3)) { 
		InitEmptyWorld();
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F4)) { 
		InitTwoSpheres();
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F5)) {
		InitPistonTest();
	}

	//Functionality 
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity
		physics->UseGravity(useGravity);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::H)) {
		reverseGravity = !reverseGravity; //Reverse gravity
		if(physics->GetGravity().y < 0){ physics->SetGravity({ 0, 10, 0 }); }
		else{ physics->SetGravity({ 0, -10, 0 }); }
		
	}

	//Moving Floors
	if (initLevel1) {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RIGHT)) {
			movingFloor->GetPhysicsObject()->AddTorque({ 0,0,-5000 });
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::LEFT)) {
			movingFloor->GetPhysicsObject()->AddTorque({ 0,0,5000 });
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP)) {
			movingFloor->GetPhysicsObject()->AddTorque(Vector3(-5000, 0, 0));
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN)) {
			movingFloor->GetPhysicsObject()->AddTorque(Vector3(5000, 0, 0));
		}

		//Piston
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U)) {
			piston->GetPhysicsObject()->AddForceAtPosition({ 0, 0, -400 }, { piston->GetTransform().GetPosition() });
		}
	}

	//Character Movement
	if (initLevel2) {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::RIGHT)) {
			character->GetPhysicsObject()->AddForce({50, 0, 0});
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::LEFT)) {
			character->GetPhysicsObject()->AddForce({-50, 0, 0});
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::UP)) { 
			character->GetPhysicsObject()->AddForce({0, 0, -50});
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::DOWN)) { 
			character->GetPhysicsObject()->AddForce({0, 0, 50});
		}
	}

	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();

	Vector3 charForward  = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
	Vector3 charForward2 = lockedObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);

	float force = 100.0f;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
		lockedObject->GetPhysicsObject()->AddForce(-rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
		Vector3 worldPos = selectionObject->GetTransform().GetPosition();
		lockedObject->GetPhysicsObject()->AddForce(rightAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		lockedObject->GetPhysicsObject()->AddForce(fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		lockedObject->GetPhysicsObject()->AddForce(-fwdAxis * force);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		lockedObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}

}

bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		renderer->DrawString("Press T to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));	
				selectionObject = nullptr;
				lockedObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;
				Ray* rayForward = new Ray(selectionObject->GetTransform().GetPosition(), { 1, 0, 0 });
				//if (world->Raycast(*rayForward, closestCollision, true)) { std::cout << "Object infront" << std::endl; }
				//Debug::DrawLine(selectionObject->GetTransform().GetPosition(), closestCollision.collidedAt(), { 1,0,0,1 }, 10000.0f);

				//Debug Information
				Debug::DrawAxisLines(selectionObject->GetTransform().GetMatrix(), 1.0f, 1.0f);
				Debug::Print("Object Mass: " + std::to_string(1/(selectionObject->GetPhysicsObject()->GetInverseMass())), Vector2(60, 20)); //mass
				Debug::Print("Object Name: " + (selectionObject->GetName()), Vector2(60, 25)); // Name
				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
	}
	else {
		renderer->DrawString("Press T to change to select mode!", Vector2(5, 85));
	}

	if (lockedObject) {
		renderer->DrawString("Press L to unlock object!", Vector2(5, 80));
	}

	else if (selectionObject) {
		renderer->DrawString("Press L to lock selected object object!", Vector2(5, 80));
	}

	if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
		if (selectionObject) {
			if (lockedObject == selectionObject) {
				lockedObject = nullptr;
			}
			else {
				lockedObject = selectionObject;
			}
		}

	}

	return false;
}

void TutorialGame::MoveSelectedObject() {
	renderer->DrawString("Click Force" + std::to_string(forceMagnitude), Vector2(10, 20));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) { return; }

	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());
		RayCollision closestCollision;

		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

//Prefabs
void TutorialGame::TwistMaze(Vector3 position) {

	GameObject* point = AddSphereToWorld({ 0, 100, 0 }, 1, 0);
	point->SetRenderObject(0);

	movingFloor = AddCubeToWorld({ position }, { 100, 2, 100 }, true, 1);
	PositionConstraint* cable = new PositionConstraint(point, movingFloor, 101);
	world->AddConstraint(cable);

	for (int i = 1; i < 11; ++i) {
		float x = (3 * i);
		float z = (5 * i);
		Vector3 bonusPosition = { x, 5, z };
		AddBonusToWorld(bonusPosition);
	}
}
void TutorialGame::Ramp(Vector3 Position) {

	GameObject* floor = AddCubeToWorld(Position, { 15, 2, 200 }, true, 0);
	floor->GetTransform().SetOrientation(Quaternion({ -0.25, 0, 0 }, 1));
	GameObject* leftWall = AddCubeToWorld({ Position.x - 15, Position.y + 10, Position.z }, {2, 10, 200}, true, 0);
	leftWall->GetTransform().SetOrientation(Quaternion({ -0.25, 0, 0 }, 1));
	GameObject* rightWall = AddCubeToWorld({Position.x + 15, Position.y + 10, Position.z}, { 2, 10, 200 }, true, 0);
	rightWall->GetTransform().SetOrientation(Quaternion({ -0.25, 0, 0 }, 1));

	float y = -10;
	float z = -100;
}

void TutorialGame::Piston(Vector3 Position) {
	GameObject* backStop = AddCubeToWorld({Position}, {20, 20, 2}, false, 0);	
	GameObject* ramp = AddCubeToWorld({Position.x, Position.y-22, Position.z - 48}, {20, 2, 50}, false, 0);
	piston = AddCubeToWorld({ Position.x, Position.y, Position.z-20}, { 15, 15, 15 }, false, 10);
	piston->GetPhysicsObject()->SetFriction(0);
	GameObject* rampLeftWall = AddCubeToWorld({Position.x-22, Position.y - 13, Position.z - 48}, {2, 10, 50}, false, 0);
	GameObject* rampRightWall = AddCubeToWorld({ Position.x + 22, Position.y - 13, Position.z - 48}, {2, 10, 50}, false, 0);

	SpringConstraint* spring = new SpringConstraint(piston, backStop, 20);
	world->AddConstraint(spring);
}

void TutorialGame::Sweeper(Vector3 Position) {
	GameObject* platform = AddCubeToWorld({ Position}, { 100, 2, 100 }, false, 0);
	GameObject* guideRailsRight = AddCubeToWorld({ Position.x, Position.y + 10, Position.z- 80 }, {100, 5, 2}, false, 0);
	GameObject* guideRailsLeft = AddCubeToWorld({ Position.x, Position.y + 10, Position.z + 80 }, { 100, 5, 2 }, false, 0);

	sweeperStart = AddStateObjectToWorld({Position.x + 90, Position.y + 15, Position.z - 90}, { 10, 10, 10 }, 0.1);
	sweeperStart->GetPhysicsObject()->SetFriction(0);
	sweeperEnd = AddStateObjectToWorld({ Position.x + 90, Position.y + 15, Position.z + 90 }, { 10, 10, 10 }, 0.1);
	sweeperEnd->GetPhysicsObject()->SetFriction(0);

	GameObject* previous = sweeperStart;

	for (int i = 1; i < 6; ++i) {
		int x = 30;
		Vector3 placement = { Position.x + 100, Position.y + 15, Position.z - (100 - x * i) };
		GameObject* block = AddCubeToWorld({ placement }, { 10, 10, 10 }, false, 10.0);
		PositionConstraint* constraint = new PositionConstraint(previous, block, x);
		AngularConstraint* angularConstraint = new AngularConstraint(previous, block, Quaternion::EulerAnglesToQuaternion(1, 1, 1));
		world->AddConstraint(angularConstraint);
		world->AddConstraint(constraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, sweeperEnd, 30);
	AngularConstraint* angularConstraint = new AngularConstraint(previous, sweeperEnd, Quaternion::EulerAnglesToQuaternion(1, 1, 1));
	world->AddConstraint(constraint);
	world->AddConstraint(angularConstraint);
}


void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);

	float invCubeMass = 5;
	int numLinks = 10;
	float maxDistance = 30;
	float cubeDistance = 20;

	Vector3 startPos = Vector3(100, 100, 100);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, false, 0);
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks+2)*cubeDistance, 0, 0), cubeSize, false, 0);

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, false, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		//AngularConstraint* angularCoonstraint = new AngularConstraint(previous, block);
		world->AddConstraint(constraint);
		//world->AddConstraint(angularCoonstraint);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	//AngularConstraint* angularCoonstraint = new AngularConstraint(previous, end);
	world->AddConstraint(constraint);
	//world->AddConstraint(angularCoonstraint);
}

void TutorialGame::Level2Maze() {
	AddCubeToWorld({ 0, 0, 0 }, { 10, 1, 10 }, false, 0);

	//Vertical Maze Boundaries
	AddCubeToWorld( {-9,3, 0}, { 1, 2, 10 }, false, 0);
	AddCubeToWorld( {9, 3, 0}, { 1, 2, 10 }, false, 0);

	//Horizontal Maze Boundaries
	AddCubeToWorld({0, 3, 9}, {8, 2, 1}, false, 0);
	AddCubeToWorld({0, 3, -9}, {8, 2, 1}, false, 0);

	//Maze Walls
	AddCubeToWorld({ 0, 3, 4 }, { 6, 2, 1 }, false, 0);
	AddCubeToWorld({ 5, 3, -1 }, { 1, 2, 4 }, false, 0);

	AddCubeToWorld({ 2, 3, -1 }, { 1, 2, 2 }, false, 0);
	AddCubeToWorld({ -2, 3, -1 }, { 1, 2, 2 }, false, 0);

	AddCubeToWorld({ 0, 3, -2 }, { 1, 2, 1 }, false, 0);

	AddCubeToWorld({ -7, 3, -2 }, { 1, 2, 4 }, false, 0);
	
}

GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("floor");

	Vector3 floorSize	= Vector3(100, 2, 100);
	AABBVolume* volume	= new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	floor->SetLayer(0);

	world->AddGameObject(floor);

	return floor;
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims, false);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols + 1; ++x) {
		for (int z = 1; z < numRows + 1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.85f, 0.3f) * meshSize);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	if (rand() % 2) {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshA, nullptr, basicShader));
	}
	else {
		character->SetRenderObject(new RenderObject(&character->GetTransform(), charMeshB, nullptr, basicShader));
	}
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	//lockedObject = character;

	return character;
}

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize = 3.0f;
	float inverseMass = 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) { //Change!
	GameObject* bonus = new GameObject("Sphere");

	Vector3 sphereSize = Vector3(1, 1, 1);
	SphereVolume* volume = new SphereVolume(1);
	bonus->SetBoundingVolume((CollisionVolume*)volume);

	bonus->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	bonus->SetRenderObject(new RenderObject(&bonus->GetTransform(), sphereMesh, basicTex, basicShader));
	bonus->GetRenderObject()->SetColour({ 1,0,0,1 });
	bonus->SetPhysicsObject(new PhysicsObject(&bonus->GetTransform(), bonus->GetBoundingVolume()));

	bonus->GetPhysicsObject()->SetInverseMass(0);
	bonus->GetPhysicsObject()->InitSphereInertia();
	bonus->GetPhysicsObject()->SetResitution(1);

	bonus->SetTrigger(1);
	bonus->SetTriggerType(2);

	world->AddGameObject(bonus);

	return bonus;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position, Vector3 dimensions, float inverseMass) {
	StateGameObject* block = new StateGameObject();

	AABBVolume* volume = new AABBVolume(dimensions);

	block->SetBoundingVolume((CollisionVolume*)volume);

	block->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	block->SetRenderObject(new RenderObject(&block->GetTransform(), cubeMesh, basicTex, basicShader));
	block->SetPhysicsObject(new PhysicsObject(&block->GetTransform(), block->GetBoundingVolume()));

	block->GetPhysicsObject()->SetInverseMass(inverseMass);
	block->GetPhysicsObject()->InitCubeInertia();
	block->GetPhysicsObject()->SetResitution(0.6);

	world->AddGameObject(block);

	return block;
}

// Adds basic Primatives

GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject("Sphere");

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();
	sphere->GetPhysicsObject()->SetResitution(0.6);

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass) {
	GameObject* capsule = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform()
		.SetScale(Vector3(radius* 2, halfHeight, radius * 2))
		.SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(capsule);

	return capsule;

}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions,  bool rotate, float inverseMass) {
	GameObject* cube = new GameObject("cube");

	AABBVolume* volume = new AABBVolume(dimensions);
	OBBVolume* volumeRot = new OBBVolume(dimensions);

	if (rotate) {cube->SetBoundingVolume((CollisionVolume*)volumeRot);}
	else{ cube->SetBoundingVolume((CollisionVolume*)volume); }

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetPhysicsObject()->SetResitution(0.6);

	world->AddGameObject(cube);

	return cube;
}



