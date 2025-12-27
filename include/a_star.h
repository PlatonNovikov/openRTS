#ifndef A_STAR_H
#define A_STAR_H

#define MAX_STEPS	10000

#include <stdlib.h>

#include "raylib.h"

typedef struct	s_tile t_tile;
typedef struct	s_map t_map;
typedef struct	vector2i Vector2i;

typedef struct	s_node
{
	int			x;
	int			y;
	int			g_cost;
	int			h_cost;
	int			f_cost;
	bool		is_blocked;

	struct s_node	*parent;
	struct s_node	*neighbors[8];
}	t_node;

typedef struct	s_path
{
	t_node		*nodes[MAX_STEPS];
	int			size;
}	t_path;

t_node	**create_node_grid(t_map map);
t_path	pathfinding(t_map map, Vector2i start, Vector2i end);

#endif