#pragma once
#define MAX_PATH_LENGTH 100
#define MAX_CYCLES 10
#define MAX_NODES 100

#define NULL ((void*)0)

typedef struct {
	int id;
	int type;
	int a;
	int b;
	int c;
} Graph_Node;

typedef struct {
	int nodes[MAX_PATH_LENGTH];
	int length;
} Cycle;

int path[MAX_PATH_LENGTH];
Cycle cycleArr[MAX_CYCLES];
int cycleCount = 0;

// Finds a Node by id in the nodes array.
Graph_Node* findNode(Graph_Node* nodes, int count, int id) {
	for (int i = 0; i < count; i++) {
		if (nodes[i].id == id) return &nodes[i];
	}
	return NULL;
}

// Prints a cycle (list of module IDs without the "M" prefix).
void printCycle(int cycle[], int len) {
	for (int i = 0; i < len; i++) {
		printf("%d ", cycle[i]);
	}
	printf("\n");
}

// Compares two cycles (arrays of node IDs) for equality.
int compareCycles(int cycle1[], int len1, int cycle2[], int len2) {
	if (len1 != len2) return 0;
	for (int i = 0; i < len1; i++) {
		if (cycle1[i] != cycle2[i]) return 0;
	}
	return 1;
}

// Canonicalizes a cycle by rotating it so that the smallest ID is first,
// and then comparing the forward and reverse orders lexicographically to choose the smallest.
void canonicalizeCycle(int cycle[], int len, int canonical[]) {
	int minIndex = 0;
	for (int i = 1; i < len; i++) {
		if (cycle[i] < cycle[minIndex]) minIndex = i;
	}

	int forward[MAX_PATH_LENGTH];
	int reverse[MAX_PATH_LENGTH];
	// Build forward rotation.
	for (int i = 0; i < len; i++) {
		forward[i] = cycle[(minIndex + i) % len];
	}
	// Build reverse rotation.
	for (int i = 0; i < len; i++) {
		reverse[i] = cycle[(minIndex - i + len) % len];
	}
	// Choose the lexicographically smallest order.
	for (int i = 0; i < len; i++) {
		if (forward[i] < reverse[i]) {
			for (int j = 0; j < len; j++)
				canonical[j] = forward[j];
			return;
		} else if (forward[i] > reverse[i]) {
			for (int j = 0; j < len; j++)
				canonical[j] = reverse[j];
			return;
		}
	}
	// They are identical.
	for (int j = 0; j < len; j++)
		canonical[j] = forward[j];
}

// Checks if the canonical representation of a cycle is unique.
// If unique, it stores the cycle in the global cycleArr and returns 1.
int isCycleUniqueCanonical(int canonical[], int len) {
	for (int i = 0; i < cycleCount; i++) {
		if (cycleArr[i].length == len) {
			int duplicate = 1;
			for (int j = 0; j < len; j++) {
				if (cycleArr[i].nodes[j] != canonical[j]) {
					duplicate = 0;
					break;
				}
			}
			if (duplicate) return 0;
		}
	}
	// Store the new unique cycle.
	for (int j = 0; j < len; j++) {
		cycleArr[cycleCount].nodes[j] = canonical[j];
	}
	cycleArr[cycleCount].length = len;
	cycleCount++;
	return 1;
}

// Depth-first search that builds a DFS path and detects cycles.
// 'start' is the anchor node; only neighbors with id >= start are considered.
// When a cycle is found (with length > 2 to ignore two-node back-and-forth edges),
// its canonical form is computed and, if unique, stored in cycleArr.
void dfs(Graph_Node* nodes, int count, int start, int current, int depth) {
	path[depth] = current;
	depth++;

	Graph_Node* node = findNode(nodes, count, current);
	if (!node) return;

	int neighbors[3] = {node->a, node->b, node->c};
	for (int i = 0; i < 3; i++) {
		int neigh = neighbors[i];
		if (neigh == 0) continue; // No edge.

		// Only follow edges to nodes with id >= start.
		if (neigh < start) continue;

		int foundIndex = -1;
		for (int j = 0; j < depth; j++) {
			if (path[j] == neigh) {
				foundIndex = j;
				break;
			}
		}
		if (foundIndex != -1) {
			// Only report a cycle if it returns to the starting node and has more than 2 nodes.
			if (neigh == start && depth > 2) {
				int cycleLength = depth - foundIndex;
				int cycle[MAX_PATH_LENGTH];
				for (int j = 0; j < cycleLength; j++) {
					cycle[j] = path[foundIndex + j];
				}
				int canonical[MAX_PATH_LENGTH];
				canonicalizeCycle(cycle, cycleLength, canonical);
				if (isCycleUniqueCanonical(canonical, cycleLength)) printCycle(canonical, cycleLength);
			}
			continue;
		}
		dfs(nodes, count, start, neigh, depth);
	}
}
