#include "common.h"

#define MAX_PATH_LENGTH 100
#define MAX_CYCLES 10
#define MAX_NUMBER_OF_MODULES 50

#define MAX_PATH_LENGTH 100
#define MAX_CYCLES 10
#define MAX_NODES 100

#define NULL ((void*)0)

typedef enum GRAPH {
	SINGLE_VERTEX = 0,
	DOUBLE_VERTEX = 1,
} GRAPH;

// Structure a hardware module
typedef struct ModulePi {
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

ModulePi modules[MAX_NUMBER_OF_MODULES];
int count = 0;
int highest_id_module = 0;

/// @brief Maps the type string to an integer value.
/// @param type A string representing the type of the module.
/// @return An integer value corresponding to the type string, or -1 if the type is not recognized.
int map_type_to_int(const char* type) {
	if (strcmp(type, "tgate") == 0) return PLANE;
	if (strcmp(type, "tdrop-off") == 0) return DROPOFF;
	if (strcmp(type, "tsecurity") == 0) return SECURITY;
	if (strcmp(type, "tquarantine") == 0) return QUARANTINE;
	if (strcmp(type, "tdefault") == 0) return 5;
	if (strcmp(type, "tcheck-in") == 0) return 6;
	return -1;
}

/// @brief Parses the value data from the string.
/// @param val Pointer to the integer value to be assigned.
/// @param data The string containing the data to be parsed.
/// @param index Pointer to the current index in the string.
void parse_value_data(int* val, char* data, int* index) {
	while (data[*index] >= '0' && data[*index] <= '9') {
		*val = *val * 10 + (data[*index] - '0');
		 *index++;
	}
}

/// @brief Parses the configuration data from a string and populates the modules array.
/// @param data A string containing the configuration data.
/// @return The rotation of the configuration data.
int parse_config(char* data) {
	int i = 0;
	int firstLineNumber = 0;

	// Read first line (number)
	while (data[i] && data[i] != '\n') {
		firstLineNumber = firstLineNumber * 10 + (data[i] - '0');
		i++;
	}
	if (data[i] == '\n') i++; // Skip newline

	while (data[i]) {
		// Parse line manually
		int m_num = 0, a_val = 0, b_val = 0, c_val = 0;
		char type_str[50];
		int j = 0;

		// Expecting: M<num>,
		if (data[i] == 'M') {
			i++;
			while (data[i] >= '0' && data[i] <= '9') {
				m_num = m_num * 10 + (data[i] - '0');
				i++;
			}
		}

		// Skip comma
		if (data[i] == ',') i++;

		// Parse type string
		j = 0;
		while (data[i] && data[i] != ',') {
			if (j < 49) type_str[j++] = data[i];
			i++;
		}
		type_str[j] = '\0';
		if (data[i] == ',') i++;

		// Expecting: a<num>,
		if (data[i] == 'a') i++;
		parse_value_data(&a_val, data, &i);
		if (data[i] == ',') i++;

		// Expecting: b<num>,
		if (data[i] == 'b') i++;
		parse_value_data(&b_val, data, &i);
		if (data[i] == ',') i++;

		// Expecting: c<num>
		if (data[i] == 'c') i++;
		parse_value_data(&a_val, data, &i);

		// Skip to next line
		while (data[i] && data[i] != '\n')
			i++;
		if (data[i] == '\n') i++;

		// Store in modules
		modules[m_num].id = m_num;
		modules[m_num].type = map_type_to_int(type_str);
		modules[m_num].a = a_val;
		modules[m_num].b = b_val;
		modules[m_num].c = c_val;
		count++;
		if (highest_id_module < m_num) {
			highest_id_module = m_num;
		}
	}

	return firstLineNumber;
}

/// @brief Create a graph with the given number of vertices.
/// @param vertices number of vertices in the graph
/// @return Pointer to the created graph.
Graph* createGraph(int vertices) {
	Graph* graph = (Graph*)malloc(sizeof(Graph));
	graph->vertices = vertices;

	// Allocate memory for adjacency lists
	graph->adjLists = (Node**)malloc((vertices + 1) * sizeof(Node*));

	// Initialize adjacency lists as empty
	for (int i = 0; i <= vertices; i++)
		graph->adjLists[i] = NULL;

	for (int i = 0; i <= vertices; i++) {
		graph->visited[i] = 0;
	}
	return graph;
}

// Function to create a new adjacency list node
Node* createNode(int vertex) {
	Node* newNode = (Node*)malloc(sizeof(Node));
	newNode->vertex = vertex;
	newNode->next = NULL;
	for (int i = 0; i < MAX_NUMBER_OF_MODULES; i++) {
		newNode->dist[i] = 0;
	}
	return newNode;
}

// Function to add an edge to an undirected graph
void addEdge(Graph* graph, int src, int vertex) {
	// Add edge from src to vertex
	Node* newNode = createNode(vertex);
	newNode->next = graph->adjLists[src];
	graph->adjLists[src] = newNode;
}

// Function to remove an edge from the graph
void removeEdge(Graph* graph, int src, int vertex) {
	Node* temp = graph->adjLists[src];
	Node* prev = NULL;

	// Search for the destination node in the adjacency list of the source
	while (temp != NULL && temp->vertex != vertex) {
		prev = temp;
		temp = temp->next;
	}

	// If the destination node is found, remove it
	if (temp != NULL) {
		if (prev != NULL) {
			prev->next = temp->next;
		} else {
			graph->adjLists[src] = temp->next;
		}
		free(temp);
	}
}

// Function to print the adjacency list
void printGraph(Graph* graph) {
	for (int i = 0; i <= graph->vertices; i++) {
		if (!modules[i].id) {
			continue;
		}
		Node* temp = graph->adjLists[i];
		printf("Adjacency list of vertex %d: ", i);
		while (temp) {
			printf("%d -> ", temp->vertex);
			temp = temp->next;
		}
		printf("NULL\n");
	}
}

// Free memory for graph (NOT USED IN THIS FILE BUT MIGHT BE USEFUL LATER)
void freeGraph(Graph* graph) {
	for (int i = 0; i < graph->vertices; i++) {
		Node* temp = graph->adjLists[i];
		while (temp) {
			Node* toFree = temp;
			temp = temp->next;
			free(toFree);
		}
	}
	free(graph->adjLists);
	free(graph);
}

int isEmpty(Node* queue) { return queue == 0; }

void enqueue(Node** queue, int value) {
	Node* newNode = createNode(value);
	if (isEmpty(*queue)) {
		*queue = newNode;
	} else {
		Node* temp = *queue;
		while (temp->next) {
			temp = temp->next;
		}
		temp->next = newNode;
	}
}

int dequeue(Node** queue) {
	int nodeData = (*queue)->vertex;
	Node* temp = *queue;
	*queue = (*queue)->next;
	free(temp);
	return nodeData;
}

void printQueue(Node* queue) {
	while (queue) {
		printf("%d ", queue->vertex);
		queue = queue->next;
	}
	printf("\n");
}

int route_find(int from, int to, int* predec) {
	while (1) {
		if (predec[to] <= 0) {
			return -1;
		}
		if (predec[to] == from) {
			return to;
		} else {
			to = predec[to];
		}
		sleep(100);
	}
	return -1;
}

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

/// @brief Run BFS from the startVertex on graph and fill the predecessors array and nearest_dest array
/// @param graph The graph that BFS will be run on
/// @param startVertex The starting vertex for BFS
/// @param predecessors A list of predecessors for each vertex
/// @param largest_cycle The largest cycle in the graph
/// @param nearest_dest The array of nearest destinations for each module type
/// @return returns -1 if the graph is empty, 0 otherwise
int bfs(Graph* graph, int startVertex, int* predecessors, Cycle* largest_cycle,
		uint8_t nearest_dest[NUMBER_OF_DEST_TYPES]) {
	// Check if the graph is empty and return -1 if it is
	if (graph->adjLists[startVertex] == NULL) {
		return -1;
	}

	// Initialize the queue
	struct Node* queue = NULL;

	// Fill in the values in the array for the StartVertex
	predecessors[startVertex] = -1;
	graph->visited[startVertex] = 1;
	graph->adjLists[startVertex]->dist[startVertex] = 0;

	// Add the starting vertex to the queue
	enqueue(&queue, startVertex);

	// while the queue is not empty
	while (!isEmpty(queue)) {
		// Dequeue the current vertex and save it in to currentVertex
		int currentVertex = dequeue(&queue);

		// Get the first entry in the adjacency list for the current vertex
		struct Node* temp = graph->adjLists[currentVertex];

		// Loop through all the neighbors of the current vertex
		while (temp) {
			// Get the adjacent vertex
			int adjVertex = temp->vertex;

			// If the adjacent vertex has not been visited yet
			if (graph->visited[adjVertex] == 0) {
				// Save the type of the module, corresponding to the node
				int mod_type = modules[adjVertex].type;

				// Assign nearest destinations for the module types
				if (nearest_dest[mod_type] == 0) {
					if (mod_type < NUMBER_OF_DEST_TYPES) {
						sleep(10);
						nearest_dest[mod_type] = adjVertex;
					}
				}

				// Assign nearest storage
				for (int i = 0; i < largest_cycle->length; i++) {
					if (largest_cycle->nodes[i] == adjVertex && nearest_dest[STORAGE] == 0) {
						nearest_dest[STORAGE] = adjVertex;
						break;
					}
				}

				// Set the predecessor of the adjacent vertex to the current vertex
				predecessors[adjVertex] = currentVertex;
				// Mark the adjacent vertex as visited and enqueue it
				graph->visited[adjVertex] = 1;
				enqueue(&queue, adjVertex);
				// Set the distance of the adjacent vertex to the distance of the current vertex + 1
				graph->adjLists[startVertex]->dist[adjVertex] = graph->adjLists[startVertex]->dist[currentVertex] + 1;
			}
			// Move to the next node in the adjacency list
			temp = temp->next;
		}
	}

	// reset the visited array for the next BFS
	for (int i = 1; i < MAX_NUMBER_OF_MODULES; i++) {
		graph->visited[i] = 0;
	}
	return 0;
}

// Main function to test the adjacency list implementation
Graph* convert_to_graph(char* layout, int vertex_type, Cycle* largest_cycle) {
	parse_config(layout);

	// Start a DFS from each node.
	// The  only follows neighbors with id >= start so that each cycle's canonical representation is encountered once.
	for (int i = 0; i < highest_id_module; i++) {
		int start = modules[i].id;
		dfs(modules, highest_id_module + 1, start, start, 0);
	}

	printf("Total unique cycles: %d\n", cycleCount);

	largest_cycle->length = 0;
	for (int i = 0; i < count; i++) {
		if (cycleArr[i].length > largest_cycle->length) {
			largest_cycle->length = cycleArr[i].length;
			for (int j = 0; j < cycleArr[i].length; j++) {
				largest_cycle->nodes[j] = cycleArr[i].nodes[j];
			}
		}
	}

	printf("The longest cycle is:\n");
	for (int i = 0; i < largest_cycle->length; i++) {
		printf("%d ", largest_cycle->nodes[i]);
	}
	printf("\n");
	Graph* graph = createGraph(highest_id_module);
	for (int i = 0; i <= highest_id_module; i++) {
		if (!modules[i].id) {
			continue;
		}
		// Long belt left
		if (modules[i].a) {
			addEdge(graph, i, modules[i].a);
		}
		// Long belt right
		if (modules[i].c) {
			addEdge(graph, i, modules[i].c);
		}
		// Short belt
		if (modules[i].b) {
			addEdge(graph, i, modules[i].b);
		}
	}
	int next_node;
	for (int i = largest_cycle->length - 1; i >= 0; i--) {
		if (i == 0) {
			next_node = largest_cycle->nodes[largest_cycle->length - 1];
		} else {
			next_node = largest_cycle->nodes[i - 1];
		}
		if (modules[largest_cycle->nodes[i]].a == next_node || modules[largest_cycle->nodes[i]].c == next_node) {
			sleep(1000);
			removeEdge(graph, largest_cycle->nodes[i], next_node);
		} else if (modules[largest_cycle->nodes[i]].b == next_node) {
			sleep(1000);
			removeEdge(graph, largest_cycle->nodes[i], next_node);
		}
	}
	// printGraph(graph);
	return graph;
}

/// @brief Assign a direction to a module based on its adjacent module ID
/// @param curr_vertex The current vertex (module) ID
/// @param adj_look_up_id The adjacent module ID
/// @return The (Direction) direction in which the module is connected to the adjacent module,
///         or OUT if the adjacent module ID is not found in the current module's connections
uint8_t assign_direction(int curr_vertex, int adj_look_up_id) {
	if (modules[curr_vertex].a == adj_look_up_id) return LASER_LEFT;
	if (modules[curr_vertex].b == adj_look_up_id) return RFID;
	if (modules[curr_vertex].c == adj_look_up_id) return LASER_RIGHT;
	return OUT;
}

/// @brief Fill up the graph data: look up table, largest cycle, and nearest destination array
/// @param graph The graph to fill data for
/// @param look_up The 3D array to store the look up table for each module: [X][Y][0] for ID, [X][Y][1] for direction
/// @param largest_cycle The largest cycle in the graph
/// @param nearest_dest The array to store the nearest destination for each module type
void fillGraphData(struct Graph* graph, uint8_t look_up[MAX_NUMBER_OF_MODULES][MAX_NUMBER_OF_MODULES][2],
				   Cycle* largest_cycle, uint8_t nearest_dest[MAX_NUMBER_OF_MODULES][NUMBER_OF_DEST_TYPES]) {
	// Iterate over possible ids of modules
	for (int i = 1; i < highest_id_module + 1; i++) {

		// Initialize the predecessors array to be empty
		int predecessors[MAX_NUMBER_OF_MODULES];
		for (int j = 0; j < highest_id_module + 1; j++) {
			predecessors[j] = 0;
		}

		// Run BFS from the current module
		// TODO LEA: CHECK THIS WHEN TESTING - WAS 		bfs(graph, i, &predecessors, largest_cycle, nearest_dest[i]);
		bfs(graph, i, predecessors, largest_cycle, nearest_dest[i]);

		// Find the path from the current module to all other modules
		for (int j = 1; j <= highest_id_module; j++) {
			if (graph->adjLists[i] != 0) {
				// If the module is reachable, fill in the look up table with the ID and direction
				if (i == j) {
					look_up[i][j][0] = i;
				} else {
					look_up[i][j][0] = route_find(i, j, predecessors);
				}
				look_up[i][j][1] = assign_direction(i, look_up[i][j][0]);
			}
		}
	}
}

/// @brief Send the path configuration to the module that requested it
/// @param sender Id of the module that sent the request for the path configuration
/// @param current_look_up The look up table, only for the current module
/// @param cycle The largest cycle in the graph
/// @param nearest_dest The nearest destination for each module type from the current module
/// @return the result of the send_packet function
int send_path_config(int sender, uint8_t current_look_up[MAX_NUMBER_OF_MODULES][2], Cycle* cycle,
					 uint8_t nearest_dest[NUMBER_OF_DEST_TYPES]) {
	char message[2 + MAX_NUMBER_OF_MODULES * 3 + NUMBER_OF_DEST_TYPES + 3] = {0};
	// Set up the message with the sender ID, message type, and the look up table
	message[MSG_SENDER] = 0;
	message[MSG_TYPE] = PATH_CONFIG;
	for (int i = 0; i < MAX_NUMBER_OF_MODULES; i++) {
		message[i + 2] = current_look_up[i][0];
	}

	for (int i = 0; i < MAX_NUMBER_OF_MODULES; i++) {
		message[i + 2 + MAX_NUMBER_OF_MODULES] = current_look_up[i][1];
	}

	// Fill in the cycle data
	for (int i = 0; i < cycle->length; i++) {
		message[i + 2 + MAX_NUMBER_OF_MODULES * 2] = cycle->nodes[i];
	}

	// Fill in the nearest destination data
	for (int i = 0; i < NUMBER_OF_DEST_TYPES; i++) {
		message[i + 2 + MAX_NUMBER_OF_MODULES * 3] = nearest_dest[i];
	}
	// Fill in the neighbour module IDs in the order of LEFT, RIGHT, and RFID
	message[2 + MAX_NUMBER_OF_MODULES * 3 + NUMBER_OF_DEST_TYPES] = modules[sender].a;	   // LEFT
	message[1 + 2 + MAX_NUMBER_OF_MODULES * 3 + NUMBER_OF_DEST_TYPES] = modules[sender].c; // RIGHT
	message[2 + 2 + MAX_NUMBER_OF_MODULES * 3 + NUMBER_OF_DEST_TYPES] = modules[sender].b; // RFID

	// Send the packet to the sender module
	return send_packet(sender, message, sizeof(message));
}

/// @brief Broadcast the plane status to all modules except the sender
/// @param sender The id of the module that sent the plane status update
/// @param plane_id The id of the plane that is being updated
/// @return 0 for a broadcast that did not crash
int broadcast_plane_status(int sender, char plane_id) {
	// Iterate over all modules
	for (int i = 0; i < MAX_NUMBER_OF_MODULES; i++) {
		// Sender already knows, so we ignore it
		if (modules[i].id == sender) continue;
		// If the module does not exist, skip it
		if (modules[i].id == 0) continue;

		printf("Sending plane data to: %d\n", modules[i].id);

		// Create the message to send
		char data[ARR_LENGTH];
		data[MSG_SENDER] = 0;
		data[MSG_TYPE] = PLANE_STATUS;
		data[ARR_PLANE_ID] = plane_id;
		data[ARR_MODULE_ID] = sender;

		// Send the packet to the module
		send_packet(modules[i].id, data, sizeof(data));
	}
	return 0;
}

export int main(void) {
	// Subscribe to the event of receiving a message
	subscribe_to_event(EVENT_MESSAGE_RECEIVED);

	// Initialize the states
	static uint8_t look_up[MAX_NUMBER_OF_MODULES][MAX_NUMBER_OF_MODULES][2] = {0};
	static uint8_t nearest_dest[MAX_NUMBER_OF_MODULES][NUMBER_OF_DEST_TYPES] = {0};
	Graph* graph;
	Cycle largest_cycle;

	// Create a graph based on the network map
	char* net_map = get_network_map();
	if (net_map == NULL) return -1;
	graph = convert_to_graph(net_map, SINGLE_VERTEX, &largest_cycle);

	// Fill the graph data
	fillGraphData(graph, look_up, &largest_cycle, nearest_dest);

	// Loop to receive messages
	EventType e = next_event();
	printf("I am ready to receive.\n");
	while (1) {
		if (e == EVENT_MESSAGE_RECEIVED) {
			// Save the messge to msg
			uint8_t* msg;
			next_message_address(&msg);
			int sender = msg[MSG_SENDER];
			int type = msg[MSG_TYPE];
			switch (type) {
			case PLANE_STATUS:
				// If a plane status message is received, broadcast it to the other Picos
				printf("I have received a plane update from: %d, with plane id: %d \n", sender, msg[ARR_PLANE_ID]);
				printf("%d \n", broadcast_plane_status(sender, msg[ARR_PLANE_ID]));
				break;
			case REQUEST_MOVEMENT:
				printf("I am Pi, I should not be receiving movement requests.\n");
				break;
			case REQUEST_RESPONSE:
				printf("I am Pi, I should not be receiving movement request responses.\n");
				break;
			case REQUEST_PATH_CONFIG:
				// If a path configuration request is received, send the path configuration back to the sender
				printf("I received a paths configuration request from: %d\n", sender);
				printf("Sending configuration back. Result: %d\n",
					   send_path_config(sender, look_up[sender], &largest_cycle, nearest_dest[sender]));
				break;
			default:
				break;
			}
		}
		// Get a new event
		e = next_event();

		// Sleep for a bit to add reliability
		sleep(100);
	}

	return 0;
}
