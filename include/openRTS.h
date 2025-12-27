#ifndef OPENRTS_H
#define OPENRTS_H

#define MAX_UNITS	1024

#define SPEED_SOLDIER		100.0f
#define MAX_HEALTH_SOLDIER	100
#define TILE_SIZE_X_SOLDIER			2
#define TILE_SIZE_Y_SOLDIER			2
#define PX_SIZE_X_SOLDIER			64
#define PX_SIZE_Y_SOLDIER			64

#include <stdbool.h>

#include "raylib.h"

typedef enum
{
	UNIT_SOLDIER
} e_unit_type;

typedef enum
{
	FACING_UP,
	FACING_DOWN,
	FACING_LEFT,
	FACING_RIGHT
} e_facing_direction;

typedef struct
{
	short int	health;
	Vector2		position;
	Vector2		destination;
	float		speed;
	Vector2		tile_size;
	Vector2		px_size;

	bool		selected;

	e_unit_type			type;
	e_facing_direction	facing;
} t_unit;

typedef struct
{
	int			width;
	int			height;
} t_map;

struct s_sprites
{
	Texture2D	sprite_soldier;
};

#endif