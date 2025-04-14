#pragma once
#include "find_loop.c"

#define MAX_PATH_LENGTH 100
#define MAX_CYCLES 10
#define MAX_NUMBER_OF_MODULES 50

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
    int dist[MAX_NUMBER_OF_MODULES];
    struct Node* next;
} Node;

// Structure for an adjacency list
typedef struct Graph {
    int visited[MAX_NUMBER_OF_MODULES];
    int vertices;
    Node** adjLists; // Array of linked lists
} Graph;

int map_type_to_int(const char* type_str);
void parse_value_data(int* value, char* data, int* i);
int parse_config(char* data);
Graph* createGraph(int vertices);
Node* createNode(int vertex);
void addEdge(Graph* graph, int src, int vertex);
void removeEdge(Graph* graph, int src, int vertex);
void printGraph(Graph* graph);
void freeGraph(Graph* graph);
Graph* convert_to_graph(char* layout, int vertex_type, Cycle* largest_cycle);
int isEmpty(Node *queue);
void enqueue(Node **queue, int value);
int dequeue(Node **queue);
void printQueue(Node *queue);
int route_find(int start, int end, int* predecessors);