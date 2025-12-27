#include <stdlib.h>

#include "../include/a_star.h"
#include "../include/openRTS.h"

void	list_push(t_path *list, t_node *node)
{
	list->nodes[list->size++] = node;
}

int	list_contains(t_path *list, t_node *node)
{
	for (int i = 0; i < list->size; i++)
		if (list->nodes[i] == node)
			return 1;
	return 0;
}

t_node	*get_lowest_f(t_path *list)
{
	t_node	*best = list->nodes[0];

	for (int i = 1; i < list->size; i++)
	{
		if (list->nodes[i]->f_cost < best->f_cost)
			best = list->nodes[i];
	}
	return best;
}

void	list_remove(t_path *list, t_node *node)
{
	for (int i = 0; i < list->size; i++)
	{
		if (list->nodes[i] == node)
		{
			list->nodes[i] = list->nodes[--list->size];
			return;
		}
	}
}

int	heuristic(t_node *a, t_node *b)
{
	return abs(a->x - b->x) + abs(a->y - b->y);
}

t_node **create_node_grid(t_map map)
{
	t_node	**nodes;
	int		x, y;

	nodes = calloc(map.height, sizeof(t_node *));
	if (!nodes)
		return NULL;

	for (y = 0; y < map.height; y++)
	{
		nodes[y] = calloc(map.width, sizeof(t_node));
		if (!nodes[y])
		{
			// Освобождение ранее выделенной памяти в случае ошибки
			for (int i = 0; i < y; i++)
				free(nodes[i]);
			free(nodes);
			return NULL;
		}

		for (x = 0; x < map.width; x++)
		{
			nodes[y][x].x = x;
			nodes[y][x].y = y;
			nodes[y][x].is_blocked = map.tiles[y][x].is_blocked;
			nodes[y][x].g_cost = 0;
			nodes[y][x].h_cost = 0;
			nodes[y][x].f_cost = 0;
			nodes[y][x].parent = NULL;

			// Обнуляем всех соседей
			for (int i = 0; i < 8; i++)
				nodes[y][x].neighbors[i] = NULL;
		}
	}

	// Второй проход для установки соседей (чтобы избежать обращения к неинициализированным узлам)
	for (y = 0; y < map.height; y++)
	{
		for (x = 0; x < map.width; x++)
		{
			// Пропускаем непроходимые узлы
			if (nodes[y][x].is_blocked)
				continue;

			// Север (0)
			if (y > 0 && !nodes[y - 1][x].is_blocked)
				nodes[y][x].neighbors[0] = &nodes[y - 1][x];

			// Юг (1)
			if (y < map.height - 1 && !nodes[y + 1][x].is_blocked)
				nodes[y][x].neighbors[1] = &nodes[y + 1][x];

			// Запад (2)
			if (x > 0 && !nodes[y][x - 1].is_blocked)
				nodes[y][x].neighbors[2] = &nodes[y][x - 1];

			// Восток (3)
			if (x < map.width - 1 && !nodes[y][x + 1].is_blocked)
				nodes[y][x].neighbors[3] = &nodes[y][x + 1];

			// Проверка диагональных соседей с учётом стен
			// Северо-запад (4) - проверяем также север и запад
			if (y > 0 && x > 0 &&
				!nodes[y - 1][x - 1].is_blocked &&
				!nodes[y - 1][x].is_blocked &&   // Проверяем, чтобы не было "сквозь угол"
				!nodes[y][x - 1].is_blocked)
				nodes[y][x].neighbors[4] = &nodes[y - 1][x - 1];

			// Северо-восток (5)
			if (y > 0 && x < map.width - 1 &&
				!nodes[y - 1][x + 1].is_blocked &&
				!nodes[y - 1][x].is_blocked &&
				!nodes[y][x + 1].is_blocked)
				nodes[y][x].neighbors[5] = &nodes[y - 1][x + 1];

			// Юго-запад (6)
			if (y < map.height - 1 && x > 0 &&
				!nodes[y + 1][x - 1].is_blocked &&
				!nodes[y + 1][x].is_blocked &&
				!nodes[y][x - 1].is_blocked)
				nodes[y][x].neighbors[6] = &nodes[y + 1][x - 1];

			// Юго-восток (7)
			if (y < map.height - 1 && x < map.width - 1 &&
				!nodes[y + 1][x + 1].is_blocked &&
				!nodes[y + 1][x].is_blocked &&
				!nodes[y][x + 1].is_blocked)
				nodes[y][x].neighbors[7] = &nodes[y + 1][x + 1];
		}
	}

	return nodes;
}

t_node	**clone_node_grid(t_map map)
{
	t_node	**nodes;
	int		x;
	int		y;

	nodes = calloc(map.height, sizeof(t_node *));
	for (y = 0; y < map.height; y++)
	{
		nodes[y] = calloc(map.width, sizeof(t_node));
		for (x = 0; x < map.width; x++)
		{
			nodes[y][x] = map.nodes[y][x];
		}
	}
	return (nodes);
}

void	free_node_grid(t_node **grid, int height)
{
	for (int y = 0; y < height; y++)
	{
		free(grid[y]);
	}
	free(grid);
}

t_path pathfinding(t_map map, Vector2i start, Vector2i end)
{
	t_node	**grid = create_node_grid(map);
	t_path	path = {0};

	t_node	*start_node = &grid[start.y][start.x];
	t_node	*end_node   = &grid[end.y][end.x];

	t_path	open = {0};
	t_path	closed = {0};

	bool found_path = false;

	start_node->g_cost = 0;
	start_node->h_cost = heuristic(start_node, end_node);
	start_node->f_cost = start_node->g_cost + start_node->h_cost;

	list_push(&open, start_node);

	while (open.size > 0)
	{
		t_node *current = get_lowest_f(&open);

		if (current->x == end_node->x && current->y == end_node->y)
		{
			found_path = true;
			break;
		}

		list_remove(&open, current);
		list_push(&closed, current);

		for (int i = 0; i < 8; i++)
		{
			t_node *neighbor = current->neighbors[i];
			if (!neighbor || list_contains(&closed, neighbor))
				continue;

			int tentative_g = current->g_cost + 1;

			if (!list_contains(&open, neighbor) || tentative_g < neighbor->g_cost)
			{
				neighbor->parent = current;
				neighbor->g_cost = tentative_g;
				neighbor->h_cost = heuristic(neighbor, end_node);
				neighbor->f_cost = neighbor->g_cost + neighbor->h_cost;

				if (!list_contains(&open, neighbor))
					list_push(&open, neighbor);
			}
		}
	}

	if (found_path)
	{
		t_node *current = &grid[end.y][end.x];
		while (current)
		{
			path.nodes[path.size++] = current;
			current = current->parent;
		}

		// Разворачиваем путь
		for (int i = 0; i < path.size / 2; i++)
		{
			t_node *tmp = path.nodes[i];
			path.nodes[i] = path.nodes[path.size - i - 1];
			path.nodes[path.size - i - 1] = tmp;
		}
	}
	else
		path.size = 0; // Path not found

	free_node_grid(grid, map.height);

	return path;
}
