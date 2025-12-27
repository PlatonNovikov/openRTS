#ifndef OPENRTS_H
#define OPENRTS_H

#define MAX_UNITS	1024

#define SPEED_SOLDIER		100.0f
#define MAX_HEALTH_SOLDIER	100

#include "raylib.h"

typedef enum
{
	UNIT_SOLDIER
} e_unit_type;

typedef struct
{
	short int	health;
	Vector2		position;
	Vector2		destination;
	float		speed;

	e_unit_type	type;
} t_unit;

struct s_sprites
{
	Texture2D	sprite_soldier;
};

#endif