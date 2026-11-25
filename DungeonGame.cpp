#include "DungeonGame.h"

DungeonGame::DungeonGame()
{
}

DungeonGame::~DungeonGame()
{
	// Deleting runs each character's destructor, which frees its texture
	delete Hero;
	DeleteEnemies();

	// Free the textures DungeonGame loaded itself
	for (int i = 0; i < TileTextureAmount; i++)
	{
		SDL_DestroyTexture(TileTextures[i]);
	}
	SDL_DestroyTexture(SwordTexture);
	SDL_DestroyTexture(PotionTexture);
}

void DungeonGame::Awake(SDL_Renderer* _renderer)
{
	std::cout << "You have entered the dungeon\n";
	Renderer = _renderer;

	//Initialize the hero
	this->Hero = new Player(Renderer, HeroSpritePath, TileSize);

	// Loads Carpet Textures
	for (int i = 0; i < TileTextureAmount; i++)
	{
		TileSurfaces = SDL_LoadBMP(TileTexturePaths[i].c_str());						// Carpet Surface
		TileTextures[i] = SDL_CreateTextureFromSurface(Renderer, TileSurfaces);		// Carpet Texture
		SDL_SetTextureScaleMode(TileTextures[i], SDL_SCALEMODE_NEAREST);			// Scales Texture
		SDL_DestroySurface(TileSurfaces);											// Release memory by deleting the surface
	}

	// Loads Pickup Textures
	SwordTexture = IMG_LoadTexture(Renderer, SwordPickupPath.c_str());
	SDL_SetTextureScaleMode(SwordTexture, SDL_SCALEMODE_NEAREST);
	PotionTexture = IMG_LoadTexture(Renderer, PotionPickupPath.c_str());
	SDL_SetTextureScaleMode(PotionTexture, SDL_SCALEMODE_NEAREST);

	// The tiles never move in memory, so their neighbours only need setting once
	SetTileNeighbours();

	StartNewGame();
}

void DungeonGame::StartNewGame()
{
	// Reset the hero back to how they started
	Hero->Health = Hero->MaxHealth;
	Hero->IsAlive = true;
	Hero->HasSword = false;
	Hero->AttackPower = HeroBaseAttackPower;
	Hero->ChangeTexture(Renderer, HeroSpritePath);

	//Build the dungeon and load the starting room
	CurrentRoomX = NumRoomsX / 2;
	CurrentRoomY = NumRoomsY / 2;
	RandomizeDungeon();
	LoadRoom(CurrentRoomX, CurrentRoomY);

	//Place the hero, then fill the room with goblins and pickups
	Hero->MoveToTile(GetRandomFloorTile());
	SpawnEnemies();
	SpawnPickups();
}

void DungeonGame::Update(double _deltaTime)
{
	// 1. Each goblin counts down, and asks for a move when its timer runs out
	for (Enemy* enemy : Enemies)
	{
		if (enemy->IsAlive)
		{
			enemy->Update(_deltaTime);
		}
	}

	// 2. Run all of the goblins' move requests through the movement system
	ProcessPendingMoves();

	// 3. If the hero died this frame, start a fresh dungeon
	if (!Hero->IsAlive)
	{
		std::cout << "\n*** GAME OVER - a new dungeon awaits... ***\n\n";
		StartNewGame();
	}

	// 4. Draw everything
	Render();
}

void DungeonGame::Render()
{
	//Draw the tiles, and any pickup lying on them
	for (int x = 0; x < RoomSize; x++)
	{
		for (int y = 0; y < RoomSize; y++)
		{
			Tile* tile = &Tiles[x][y];

			// Walls have no texture yet, so skip them
			if (tile->Texture != nullptr)
			{
				SDL_RenderTexture(Renderer, tile->Texture, NULL, &tile->Rect);
			}

			// Pickups are drawn over the top of the tile, using the same Rect
			if (tile->TilePickup != NoPickup)
			{
				SDL_RenderTexture(Renderer, GetPickupTexture(tile->TilePickup), NULL, &tile->Rect);
			}
		}
	}

	//Draw the goblins that are still alive
	for (Enemy* enemy : Enemies)
	{
		if (enemy->IsAlive)
		{
			SDL_RenderTexture(Renderer, enemy->Texture, NULL, &enemy->Rect);
		}
	}

	//Draw the hero
	SDL_RenderTextureRotated(Renderer, Hero->Texture, NULL, &Hero->Rect, NULL, NULL, Hero->GetFlipMode());

	//Draw the hero's health as a row of small potions in the top-left corner
	for (int i = 0; i < Hero->Health; i++)
	{
		SDL_FRect healthRect = { 5 + i * 35.0f, 5, 30, 30 };
		SDL_RenderTexture(Renderer, PotionTexture, NULL, &healthRect);
	}
}

void DungeonGame::Input(Direction _dir)
{
	// Ignore input while the hero is dead (the game restarts in Update)
	if (!Hero->IsAlive)
	{
		return;
	}

	// Ask the movement system what would happen, then act on the result
	MoveContext context = TryMove(Hero, _dir);

	switch (context.Result)
	{
		case MoveOK:
			MoveCharacter(Hero, context.Destination);
			break;
		case Blocked:
			// A wall is in the way - do nothing
			break;
		case Combat:
			HandleCombat(Hero, context.Occupant);
			break;
		case LoadNewRoom:
			EnterNewRoom(_dir);
			break;
	}
}

void DungeonGame::RandomizeDungeon()
{
	// Give every room in the dungeon a random room file
	for (int x = 0; x < NumRoomsX; x++)
	{
		for (int y = 0; y < NumRoomsY; y++)
		{
			DungeonLayout[x][y] = SDL_rand(RoomFileAmount);
		}
	}
}

void DungeonGame::LoadRoom(const char* filename)
{
	SDL_Surface* surf = SDL_LoadBMP(filename);
	if (surf == nullptr)
	{
		std::cout << "Failed to load room: " << filename << "\n";
		return;
	}
	const SDL_PixelFormatDetails* pixelDetails = SDL_GetPixelFormatDetails(surf->format);
	const Uint8 bpp = SDL_BYTESPERPIXEL(surf->format);
	SDL_Color col;
	for (int y = 0; y < surf->h; y++) {
		for (int x = 0; x < surf->w; x++) {
			Uint8* pixel = static_cast<Uint8*>(surf->pixels) + y * surf->pitch + x * bpp;
			SDL_GetRGB(*reinterpret_cast<Uint32*>(pixel), pixelDetails, NULL, &col.r, &col.g,
				&col.b);
			// col now contains pixel colour at position x,y!
			this->Tiles[x][y].Configure(col, x * TileSize, y * TileSize, TileSize, TileTextures, TileTextureAmount);
			this->Tiles[x][y].SetCoordinates(x, y);
		}
	}
	SDL_DestroySurface(surf);	// Release memory by deleting the surface
	std::cout << "Room Loaded: " << filename << "\n";
}

void DungeonGame::LoadRoom(int x, int y)
{
	// Overload - look up which room file belongs at x,y, then load it as normal
	int index = DungeonLayout[x][y];
	const char* fileName = RoomFiles[index];
	LoadRoom(fileName);
}

void DungeonGame::SetTileNeighbours()
{
	for (int x = 0; x < RoomSize; x++)
	{
		for (int y = 0; y < RoomSize; y++)
		{
			// North is y - 1, East is x + 1, South is y + 1, West is x - 1
			// GetTile returns nullptr for tiles off the edge of the grid
			Tiles[x][y].SetNeighbours(GetTile(x, y - 1), GetTile(x + 1, y), GetTile(x, y + 1), GetTile(x - 1, y));
		}
	}
}

Tile* DungeonGame::GetTile(int x, int y)
{
	// Outside the grid - there is no tile here
	if (x < 0 || x >= RoomSize || y < 0 || y >= RoomSize)
	{
		return nullptr;
	}

	return &Tiles[x][y];
}

Tile* DungeonGame::GetRandomFloorTile()
{
	// Keep picking random tiles until we find a floor tile.
	// Skips the outer edge so nothing gets placed in a doorway.
	while (true)
	{
		Tile* tile = GetTile(SDL_rand(RoomSize - 1), SDL_rand(RoomSize - 1));
		if (tile->Walkable)
		{
			return tile;
		}
	}
}

Tile* DungeonGame::GetEmptyFloorTile()
{
	// Keep picking random floor tiles until we find one with nobody on it and no pickup
	while (true)
	{
		Tile* tile = GetRandomFloorTile();
		if (GetCharacterOnTile(tile) == nullptr && tile->TilePickup == NoPickup)
		{
			return tile;
		}
	}
}

void DungeonGame::EnterNewRoom(Direction _dir)
{
	// Work out which room we're heading into, and which tile we arrive on.
	// We come in on the opposite side of the new room, eg. leave East and arrive on the West edge.
	// This assumes all the doorways line up between rooms!
	int roomX = CurrentRoomX;
	int roomY = CurrentRoomY;
	int tileX = Hero->CurrentTile->CoordinateX;
	int tileY = Hero->CurrentTile->CoordinateY;

	switch (_dir)
	{
		case North: roomY--; tileY = RoomSize - 1; break;
		case East:  roomX++; tileX = 0;            break;
		case South: roomY++; tileY = 0;            break;
		case West:  roomX--; tileX = RoomSize - 1; break;
	}

	// Edge of the dungeon - there's no room to go to
	if (roomX < 0 || roomX >= NumRoomsX || roomY < 0 || roomY >= NumRoomsY)
	{
		std::cout << "That's the edge of the dungeon!\n";
		return;
	}

	CurrentRoomX = roomX;
	CurrentRoomY = roomY;
	LoadRoom(CurrentRoomX, CurrentRoomY);
	std::cout << "Entered room " << CurrentRoomX << ", " << CurrentRoomY << "\n";

	Hero->MoveToTile(GetTile(tileX, tileY));
	SpawnEnemies();
	SpawnPickups();
}

void DungeonGame::SpawnEnemies()
{
	// Get rid of the goblins from the last room
	DeleteEnemies();

	// Spawn between 1 and MaxGoblinsPerRoom goblins on empty floor tiles
	int goblinCount = 1 + SDL_rand(MaxGoblinsPerRoom);
	for (int i = 0; i < goblinCount; i++)
	{
		Enemy* goblin = new Enemy(Renderer, GoblinSpritePath, TileSize, this);
		goblin->MoveToTile(GetEmptyFloorTile());
		Enemies.push_back(goblin);
	}
	std::cout << goblinCount << " goblin(s) lurk in this room!\n";
}

void DungeonGame::DeleteEnemies()
{
	// Delete every goblin (frees their textures), then empty the list.
	// Any moves they asked for are now useless too.
	for (Enemy* enemy : Enemies)
	{
		delete enemy;
	}
	Enemies.clear();
	PendingMoves.clear();
}

GameCharacter* DungeonGame::GetCharacterOnTile(Tile* _tile)
{
	// Returns whoever is standing on this tile, or nullptr if it's empty
	if (Hero->CurrentTile == _tile)
	{
		return Hero;
	}

	for (Enemy* enemy : Enemies)
	{
		// Dead goblins don't count
		if (enemy->IsAlive && enemy->CurrentTile == _tile)
		{
			return enemy;
		}
	}

	return nullptr;
}

MoveContext DungeonGame::TryMove(GameCharacter* _character, Direction _dir)
{
	MoveContext context;
	context.Destination = _character->CurrentTile->GetNeighbour(_dir);

	if (_dir == Direction::East)
	{
		_character->flip = SDL_FLIP_HORIZONTAL;
	}

	else if (_dir == Direction::West)
	{
		_character->flip = SDL_FLIP_NONE;
	}

	// No neighbour - we're walking out of the room
	if (context.Destination == nullptr)
	{
		context.Result = LoadNewRoom;
		return context;
	}

	// Walls block movement
	if (!context.Destination->Walkable)
	{
		context.Result = Blocked;
		return context;
	}

	// Someone is already standing there
	GameCharacter* occupant = GetCharacterOnTile(context.Destination);
	if (occupant != nullptr)
	{
		context.Occupant = occupant;

		// The hero and a goblin bumping into each other means a fight.
		// Goblins don't fight each other - they just get in each other's way.
		if (_character == Hero || occupant == Hero)
		{
			context.Result = Combat;
		}
		else
		{
			context.Result = Blocked;
		}
		return context;
	}

	// Nothing in the way
	context.Result = MoveOK;
	return context;
}

void DungeonGame::MoveCharacter(GameCharacter* _character, Tile* _tile)
{
	_character->MoveToTile(_tile);

	// Only the hero can pick things up
	if (_character == Hero)
	{
		CollectPickup(_tile);
	}
}

void DungeonGame::AddPendingMove(AIMoveRequest _move)
{
	// Called by the goblins - we'll deal with it in ProcessPendingMoves
	PendingMoves.push_back(_move);
}

void DungeonGame::ProcessPendingMoves()
{
	for (AIMoveRequest move : PendingMoves)
	{
		// If the hero died earlier this frame, stop - the game is about to restart
		if (!Hero->IsAlive)
		{
			break;
		}

		// This goblin might have been killed earlier this frame
		if (!move.Character->IsAlive)
		{
			continue;
		}

		MoveContext context = TryMove(move.Character, move.MoveDirection);

		switch (context.Result)
		{
			case MoveOK:
				MoveCharacter(move.Character, context.Destination);
				break;
			case Blocked:
				// A wall or another goblin is in the way - stay put
				break;
			case Combat:
				// The goblin walked into the hero!
				HandleCombat(move.Character, context.Occupant);
				break;
			case LoadNewRoom:
				// Goblins stay in their own room
				break;
		}
	}

	// All requests have been dealt with
	PendingMoves.clear();
}

CombatResult DungeonGame::Fight(GameCharacter& _attacker, GameCharacter& _defender)
{
	CombatResult result = Undefined;

	// Combat design: both fighters roll a six-sided dice and add their AttackPower.
	// The highest total wins, the same total is a draw.
	// Goblins have 0 AttackPower, the hero has 1, or 3 with a sword.
	int attackerRoll = 1 + SDL_rand(6) + _attacker.AttackPower;
	int defenderRoll = 1 + SDL_rand(6) + _defender.AttackPower;

	std::cout << _attacker.Name << " attacks " << _defender.Name << "! ";
	std::cout << _attacker.Name << " rolls " << attackerRoll << ", " << _defender.Name << " rolls " << defenderRoll << "\n";

	if (attackerRoll > defenderRoll)
	{
		result = Win;
	}
	else if (attackerRoll < defenderRoll)
	{
		result = Loss;
	}
	else
	{
		result = Draw;
	}

	return result;
}

void DungeonGame::HandleCombat(GameCharacter* _attacker, GameCharacter* _defender)
{
	CombatResult result = Fight(*_attacker, *_defender);

	switch (result)
	{
		case Win:
			// The defender takes a hit. If that kills them, the attacker moves into their tile.
			_defender->TakeDamage(1);
			if (!_defender->IsAlive)
			{
				MoveCharacter(_attacker, _defender->CurrentTile);
			}
			break;
		case Draw:
			// Nobody gets hurt, the attacker stays where they are
			std::cout << "It's a draw!\n";
			break;
		case Loss:
			// The attacker takes a hit and stays where they are
			_attacker->TakeDamage(1);
			break;
		case Undefined:
			break;
	}
}

void DungeonGame::SpawnPickups()
{
	// A chance of a potion in every room
	if (SDL_rand(100) < PotionChance)
	{
		GetEmptyFloorTile()->TilePickup = Potion;
	}

	// A chance of a sword, but only if the hero doesn't have one yet
	if (!Hero->HasSword && SDL_rand(100) < SwordChance)
	{
		GetEmptyFloorTile()->TilePickup = Sword;
	}
}

void DungeonGame::CollectPickup(Tile* _tile)
{
	switch (_tile->TilePickup)
	{
		case Sword:
			// The sword boosts the hero's attack power, and swaps to the armed sprite
			Hero->HasSword = true;
			Hero->AttackPower = SwordAttackPower;
			Hero->ChangeTexture(Renderer, HeroSwordSpritePath);
			std::cout << "You picked up a sword! Attack power is now " << Hero->AttackPower << "\n";
			break;
		case Potion:
			// The potion restores some health
			std::cout << "You drank a potion! ";
			Hero->Heal(PotionHealAmount);
			break;
		case NoPickup:
			// Nothing here
			return;
	}

	// The pickup has been used, so remove it from the tile
	_tile->TilePickup = NoPickup;
}

SDL_Texture* DungeonGame::GetPickupTexture(PickupType _pickup)
{
	if (_pickup == Sword)
	{
		return SwordTexture;
	}
	if (_pickup == Potion)
	{
		return PotionTexture;
	}
	return nullptr;
}
