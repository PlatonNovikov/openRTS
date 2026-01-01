#ifndef TYPES_H
#define TYPES_H

#define MAX_STEPS	10000

#include <stdbool.h>
#include "raylib.h"
#include <stdlib.h>

typedef struct	s_node
{
	Vector2	pos;
	int	g_cost;
	int	h_cost;
	int	f_cost;
	bool	is_blocked;

	struct s_node	*parent;
	struct s_node	*neighbors[8];
}	t_node;

typedef struct	s_path
{
	t_node	*nodes[MAX_STEPS];
	int	size;
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

typedef struct s_unit
{
	short int		health;
	Vector2			pos;		//coordinates of the tile
	Vector2			destination;	//coordinates of the tile
	int			tpf;			//tiles per frame
	Vector2			tile_size;
	Vector2			px_size;

	bool			selected;

	t_path			path;

	e_unit_type		type;
	e_facing_direction	facing;
} t_unit;

typedef struct s_tile
{
	bool	is_blocked;
} t_tile;

typedef struct s_map
{
	int	width;
	int	height;
	t_tile	**tiles;
	t_node	**nodes;
} t_map;

struct s_sprites
{
	Texture2D	sprite_soldier;
};

#endif