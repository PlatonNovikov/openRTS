#ifndef A_STAR_H
#define A_STAR_H

#include <stdlib.h>

#include "raylib.h"
#include "types.h"

t_node	**create_node_grid(t_map map);
t_path	pathfinding(t_map map, Vector2 start, Vector2 end);

#endif