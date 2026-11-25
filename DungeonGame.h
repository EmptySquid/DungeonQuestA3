#pragma once
#include <iostream>
#include <list>
#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include "GameCharacter.h"
#include "Player.h"
#include "Enemy.h"
#include "Tile.h"
#include "Enums.h"
#include "MoveContext.h"
#include "AIMoveRequest.h"

// Sprites
const static std::string HeroSpritePath = "Sprites/Hero_no_sword.png";
const static std::string HeroSwordSpritePath = "Sprites/Hero_sword.png";
const static std::string GoblinSpritePath = "Sprites/Enemy_orc_blue.png";
const static std::string SwordPickupPath = "Sprites/Pickup_Sword.png";
const static std::string PotionPickupPath = "Sprites/Pickup_Health.png";

const static char* RoomFiles[]{ "Data/Rooms/Room01.bmp", "Data/Rooms/Room02.bmp", "Data/Rooms/Room03.bmp", "Data/Rooms/Room04.bmp", "Data/Rooms/Room05.bmp" };
const int RoomFileAmount = sizeof(RoomFiles) / sizeof(RoomFiles[0]);	// Gets Room file array length
const static std::string TileTexturePaths[]{ "Sprites/Tile_carpet_base.bmp", "Sprites/Tile_carpet_blood_1.bmp", "Sprites/Tile_carpet_bones.bmp" };
const int TileTextureAmount = sizeof(TileTexturePaths) / sizeof(TileTexturePaths[0]);	// Gets Carpet texture array length

const static int RoomSize = 10;

// The dungeon is a grid of rooms
const static int NumRoomsX = 10;
const static int NumRoomsY = 10;

// Gameplay settings - tweak these to balance the game
const static int MaxGoblinsPerRoom = 3;
const static int HeroBaseAttackPower = 1;
const static int SwordAttackPower = 3;		// The hero's attack power once they find a sword
const static int PotionHealAmount = 2;
const static int PotionChance = 50;			// % chance of a potion spawning in each room
const static int SwordChance = 25;			// % chance of a sword spawning in each room


class DungeonGame
{
public:
	//Variables
	float TileSize = 100;
	SDL_Renderer* Renderer = nullptr;			// Kept from Awake so we can draw and load textures later
	Player* Hero = nullptr;
	std::list<Enemy*> Enemies;					// The goblins in the current room
	std::list<AIMoveRequest> PendingMoves;		// Moves the goblins have asked for this frame
	Tile Tiles[RoomSize][RoomSize];
	SDL_Texture* TileTextures[TileTextureAmount];
	SDL_Surface* TileSurfaces;
	SDL_Texture* SwordTexture = nullptr;
	SDL_Texture* PotionTexture = nullptr;
	int DungeonLayout[NumRoomsX][NumRoomsY];	// Each room stores an index into RoomFiles
	int CurrentRoomX = NumRoomsX / 2;			// Start in the middle of the dungeon
	int CurrentRoomY = NumRoomsY / 2;

	float MoveCountDown;
	float MoveEverySeconds = 2;

	//Functions
	DungeonGame();	//Constructor
	~DungeonGame(); //Desructor
	void Awake(SDL_Renderer* _renderer);
	void StartNewGame();
	void Update(double _deltaTime);
	void Render();
	void Input(Direction _dir);

	// Dungeon and rooms
	void RandomizeDungeon();
	void LoadRoom(const char* filename);
	void LoadRoom(int x, int y);
	void SetTileNeighbours();
	Tile* GetTile(int x, int y);
	Tile* GetRandomFloorTile();
	Tile* GetEmptyFloorTile();
	void EnterNewRoom(Direction _dir);

	// Characters and movement
	void SpawnEnemies();
	void DeleteEnemies();
	GameCharacter* GetCharacterOnTile(Tile* _tile);
	MoveContext TryMove(GameCharacter* _character, Direction _dir);
	void MoveCharacter(GameCharacter* _character, Tile* _tile);
	void AddPendingMove(AIMoveRequest _move);
	void ProcessPendingMoves();

	// Combat
	CombatResult Fight(GameCharacter& _attacker, GameCharacter& _defender);
	void HandleCombat(GameCharacter* _attacker, GameCharacter* _defender);

	// Pickups
	void SpawnPickups();
	void CollectPickup(Tile* _tile);
	SDL_Texture* GetPickupTexture(PickupType _pickup);
};
