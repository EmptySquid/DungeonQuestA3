#include "Enemy.h"
#include "DungeonGame.h"	// Fine to include here - see the forward declaration in Enemy.h
#include "AIMoveRequest.h"

Enemy::Enemy(SDL_Renderer* _renderer, std::string _path, float _tileSize, DungeonGame* _game)
	: GameCharacter(_renderer, _path, _tileSize)
{
	Name = "Goblin";
	GameRef = _game;

	// Start each goblin part way through its countdown, so they don't all move at the same moment.
	// SDL_randf() gives a random number between 0 and 1.
	moveCountDown = SDL_randf() * MoveEverySeconds;
}

void Enemy::Update(double _deltaTime)
{
	// Count down by however much time has passed since the last frame
	moveCountDown -= _deltaTime;

	if (moveCountDown <= 0)
	{
		moveCountDown = MoveEverySeconds;	// Reset the countdown
		DoRandomMove();
	}
}

void Enemy::DoRandomMove()
{
	// Our very basic AI: pick a random direction and ask the game to move us.
	// The movement system will decide if it's a legal move or not.
	AIMoveRequest move;
	move.Character = this;
	move.MoveDirection = (Direction)SDL_rand(4);	// 0-3 lines up with North, East, South, West

	GameRef->AddPendingMove(move);
}
