#pragma once
#include "GameObject.h"
#include "../CSC8503Common/GameWorld.h"

namespace NCL {
	namespace CSC8503 {
		class Triggers {
		public:
			Triggers(GameWorld& g) : gameWorld(g) {
				score = 0;
				winTrigger = false;
				looseTrigger = false;
			}
			~Triggers() {};

			void Update(float dt);
			void ResolveTriggers(GameObject* a, GameObject* b);

			int GetScore() { return score; }
			void SetScore(int newScore) { score = newScore; }

			bool GetWinTrigger() { return winTrigger; }
			void SetWinTrigger(int resetWinTrigger) { winTrigger = resetWinTrigger; }

			bool GetLooseTrigger() { return looseTrigger; }
			void SetLooseTrigger(int resetLooseTrigger) { looseTrigger = resetLooseTrigger; }

		protected:
			GameWorld& gameWorld;

			int score;
			bool winTrigger;
			bool looseTrigger;
		};
	}
}
