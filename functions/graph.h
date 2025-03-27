#include "find_loop.c"

#define MAX_PATH_LENGTH 100
#define MAX_CYCLES 10
#define MAX_MODULES 100

enum GRAPH {
	SINGLE_VERTEX = 0,
	DOUBLE_VERTEX = 1,
};

// Structure a hardware module
typedef struct {
	int id;
	int type;
	int a;
	int b;
	int c;
} Module;

// Structure for an adjacency list node
typedef struct Node {
	int dest;
	struct Node* next;
} Node;

// Structure for an adjacency list
typedef struct Graph {
	int vertices;
	Node** adjLists; // Array of linked lists
} Graph;

// Converts the module 'layout' into a graph given the specific 'vertex_type'.
Graph* convert_to_graph(char* layout, int vertex_type);