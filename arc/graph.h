#pragma once
#include "find_loop.c"

#define MAX_PATH_LENGTH 100
#define MAX_CYCLES 10
#define MAX_MODULES 50

typedef enum GRAPH {
	SINGLE_VERTEX = 0,
	DOUBLE_VERTEX = 1,
} GRAPH;

// Structure a hardware module
typedef struct ModulePi{
    int id;
    int type;
    int a;
    int b;
    int c;
} ModulePi;

// Structure for an adjacency list node
typedef struct Node {
    int vertex;
    int dist[MAX_MODULES];
    struct Node* next;
} Node;

// Structure for an adjacency list
typedef struct Graph {
    int visited[MAX_MODULES];
    int vertices;
    Node** adjLists; // Array of linked lists
} Graph;

// Converts the module 'layout' into a graph given the specific 'vertex_type'.
Graph* convert_to_graph(char* layout, int vertex_type, Cycle* largest_cycle);