#include "Player.h"

void Player::Heal(int _amount)
{
	Health += _amount;

	// Can't heal past full health
	if (Health > MaxHealth)
	{
		Health = MaxHealth;
	}
	std::cout << "Hero health: " << Health << " / " << MaxHealth << "\n";
}
