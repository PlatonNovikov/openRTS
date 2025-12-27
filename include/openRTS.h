#ifndef OPENRTS_H
#define OPENRTS_H

#define MAX_UNITS	1024

#define TILE_SIZE_PX				64

//TPF - tiles per frame

#define TPF_SOLDIER					100
#define MAX_HEALTH_SOLDIER			100
#define TILE_SIZE_X_SOLDIER			2
#define TILE_SIZE_Y_SOLDIER			2
#define PX_SIZE_X_SOLDIER			64
#define PX_SIZE_Y_SOLDIER			64

#include <stdbool.h>
#include <stdlib.h>

#include "raylib.h"

//stuff from a_star.h
#ifndef MAX_STEPS
#define MAX_STEPS	10000
#endif
typedef struct	s_node	t_node;
typedef struct	s_path
{
	t_node		*nodes[MAX_STEPS];
	int			size;
}	t_path;


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

typedef struct vector2i
{
	int	x;
	int	y;
} Vector2i;

typedef struct s_unit
{
	short int			health;
	Vector2i			position;		//coordinates of the tile
	Vector2i			destination;	//coordinates of the tile
	int					tpf;			//tiles per frame
	Vector2i			tile_size;
	Vector2i			px_size;

	bool				selected;

	t_path				path;

	e_unit_type			type;
	e_facing_direction	facing;
} t_unit;

typedef struct s_tile
{
	bool	is_blocked;
} t_tile;

typedef struct s_map
{
	int			width;
	int			height;
	t_tile		**tiles;
	t_node		**nodes;
} t_map;

struct s_sprites
{
	Texture2D	sprite_soldier;
};

#endif