#pragma once
#include "Enums.h"
#include "GameCharacter.h"

// Another 'data class'. When a goblin wants to move it fills one of these in
// and hands it to DungeonGame, which runs it through the movement system (TryMove).
class AIMoveRequest
{
public:
	GameCharacter* Character = nullptr;	// Who wants to move
	Direction MoveDirection = North;	// Which way they want to go
};
