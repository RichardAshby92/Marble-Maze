#include "../../Common/Window.h"

#include "../CSC8503Common/StateMachine.h"
#include "../CSC8503Common/StateTransition.h"
#include "../CSC8503Common/State.h"
#include "../CSC8503Common/PushdownMachine.h"
#include "../CSC8503Common/PushdownState.h"

#include "../CSC8503Common/NavigationGrid.h"

#include "TutorialGame.h"

using namespace NCL;
using namespace CSC8503;

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/

vector<Vector3> testNodes;
void TestPath() {
	NavigationGrid grid("TerrorMaze.txt");

	NavigationPath outPath;

	Vector3 startPos(80, 0, 10);
	Vector3 endPos(80, 0, 80);

	bool found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void DisplayPathFinding() {
	for (int i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}

class Loose : public PushdownState {
public:
	Loose(TutorialGame* g) { game = g; };
protected:
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
			return PushdownResult::Pop;
		}

		Debug::Print("Defeat, Press R to restart the Level", Vector2(30, 35));
		Debug::Print("Time Elasped: " + std::to_string(game->time), Vector2(30, 40));
		Debug::Print("Final Score: " + std::to_string(game->GetTriggers()->GetScore()), Vector2(30, 45));
		game->UpdateGame(dt);

		return PushdownResult::NoChange;
	};

	void OnAwake() override {
		game->InitMenu();
	}
	TutorialGame* game;
};

class Win : public PushdownState {
public:
	Win(TutorialGame* g) { game = g; };
protected:
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::R)) {
			return PushdownResult::Pop;
		}

		Debug::Print("Victory, Press R to restart the Level", Vector2(30, 35));
		Debug::Print("Time Elasped: " + std::to_string(game->time), Vector2(30, 40));
		Debug::Print("Final Score: " + std::to_string(game->GetTriggers()->GetScore()), Vector2(30, 45));
		game->UpdateGame(dt);

		return PushdownResult::NoChange;
	};

	void OnAwake() override {
		game->InitMenu();
	}
	TutorialGame* game;
};

class Level1 : public PushdownState {
public:
	Level1(TutorialGame* g) { game = g; };
protected:
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (game->GetTriggers()->GetWinTrigger()) {
			*newState = new Win(game);
			return PushdownResult::Push;
		}

		if (game->GetTriggers()->GetLooseTrigger()) {
			*newState = new Loose(game);
			return PushdownResult::Push;
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::M)) {
			return PushdownResult::Pop;
		}

		game->UpdateGame(dt);

		Debug::Print("Time: " + game->timePrint, Vector2(75, 5));
		Debug::Print("Score: " + std::to_string(game->GetTriggers()->GetScore()), Vector2(50, 5));

		return PushdownResult::NoChange;
	};

	void OnAwake() override {
		game->InitLevel1();
		game->initLevel2 = false;
		game->initLevel1 = true;
	}

	TutorialGame* game;
};

class Level2 : public PushdownState {

public:
	Level2(TutorialGame* g) { game = g; };
protected:
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::M)) {
			return PushdownResult::Pop;
		}

		Debug::Print("Time: " + game->timePrint, Vector2(75, 5));
		Debug::Print("Score: " + std::to_string(game->GetTriggers()->GetScore()), Vector2(50, 5));

		game->UpdateGame(dt);
		return PushdownResult::NoChange;
	};

	void OnAwake() override {
		game->InitLevel2();
		game->initLevel2 = true;
		game->initLevel1 = false;
	}

	TutorialGame* game;
};

class Menu : public PushdownState {
public:
	Menu(TutorialGame* g) { game = g; };
protected:
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM1)) {
			*newState = new Level1(game);
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NUM2)) {
			*newState = new Level2(game);
			return PushdownResult::Push;
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::ESCAPE)) {
			return PushdownResult::Pop;
		}

		Debug::Print("Press 1 to Load Level 1: Marble Descent", Vector2(10, 25));
		Debug::Print("Press 2 to load Level 2: Maze of 'Terror'", Vector2(10, 60));

		game->UpdateGame(dt);

		return PushdownResult::NoChange;
	};
	void OnAwake() override {
		game->InitMenu();
	}

	TutorialGame* game;
};

int main() {
	Window*w = Window::CreateGameWindow("CSC8503 Game technology!", 1920, 1080, true);

	if (!w->HasInitialised()) {
		return -1;
	}	
	srand(time(0));
	w->ShowOSPointer(false);
	w->LockMouseToWindow(true);

	TutorialGame* g = new TutorialGame();
	PushdownMachine machine(new Menu(g));
	w->GetTimer()->GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyboardKeys::ESCAPE)) {
		float dt = w->GetTimer()->GetTimeDeltaSeconds();
		if (dt > 0.1f) {
			std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::PRIOR)) {
			w->ShowConsole(true);
		}
		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::NEXT)) {
			w->ShowConsole(false);
		}

		if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::T)) {
			w->SetWindowPosition(0, 0);
		}
		if (!machine.Update(dt)) {
			return 0;
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		//g->UpdateGame(dt);
		//TestPushDownAutomata(w);
	}
	Window::DestroyGameWindow();
}