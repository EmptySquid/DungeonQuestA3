#pragma once
//Cardinal directions for movement and tile locations relative to game characters
enum Direction { North, East, South, West };

// What happened when a character tried to move
enum MoveResult { MoveOK, Blocked, Combat, LoadNewRoom };

// The outcome of a fight, from the attacker's point of view
enum CombatResult { Undefined, Win, Draw, Loss };

// What is lying on a tile, waiting to be picked up
enum PickupType { NoPickup, Sword, Potion };
