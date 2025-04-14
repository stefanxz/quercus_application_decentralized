#include "graph.h"
#include "find_loop.c"
#include "essentials.h"

ModulePi modules[MAX_NUMBER_OF_MODULES];
int count = 0;  
int highest_id_module = 0;

/// @brief Maps the type string to an integer value.
/// @param type A string representing the type of the module.
/// @return An integer value corresponding to the type string, or -1 if the type is not recognized.
int map_type_to_int(const char *type) {
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
    if (data[i] == '\n') i++;  // Skip newline

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

        //Parse type string
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
        while (data[i] && data[i] != '\n') i++;
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
    graph->adjLists = (Node**)malloc((vertices+1) * sizeof(Node*));
    
    // Initialize adjacency lists as empty
    for (int i = 0; i <= vertices; i++)
        graph->adjLists[i] = NULL;

    for (int i = 0; i <= vertices; i++){
        graph->visited[i] = 0;
    }
    return graph;
}

// Function to create a new adjacency list node
Node* createNode(int vertex) {
    Node* newNode = (Node*) malloc(sizeof(Node));
    newNode->vertex = vertex;
    newNode->next = NULL;
    for(int i = 0; i < MAX_NUMBER_OF_MODULES; i++){
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
        if (!modules[i].id) { continue; }
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

// Main function to test the adjacency list implementation
Graph* convert_to_graph(char *layout, int vertex_type, Cycle* largest_cycle) {
    parse_config(layout);

    // Start a DFS from each node.
    // The  only follows neighbors with id >= start so that each cycle's canonical representation is encountered once.
    for (int i = 0; i < highest_id_module; i++) {
        int start = modules[i].id;
        dfs(modules, highest_id_module+1, start, start, 0);
    }
    
    printf("Total unique cycles: %d\n", cycleCount);

    largest_cycle->length = 0;
    for (int i = 0; i < count; i++) {
        if (cycleArr[i].length > largest_cycle -> length) {
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
        if (!modules[i].id) { continue; }
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
    for (int i = largest_cycle -> length-1; i >= 0; i--) {
        if (i == 0) {
            next_node = largest_cycle -> nodes[largest_cycle -> length-1];
        } else {
            next_node = largest_cycle -> nodes[i-1];
        }
        if (modules[largest_cycle -> nodes[i]].a == next_node || modules[largest_cycle -> nodes[i]].c == next_node) {
            sleep(1000);
            removeEdge(graph, largest_cycle -> nodes[i], next_node);
        } else if(modules[largest_cycle -> nodes[i]].b == next_node){
            sleep(1000);
            removeEdge(graph, largest_cycle -> nodes[i], next_node);
        }
    }
    // printGraph(graph);
    return graph;
}


int isEmpty(Node *queue)
{
    return queue == 0;
}
 
void enqueue(Node **queue, int value)
{
    Node *newNode = createNode(value);
    if (isEmpty(*queue))
    {
        *queue = newNode;
    }
    else
    {
        Node *temp = *queue;
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}
 
int dequeue(Node **queue)
{
    int nodeData = (*queue)->vertex;
    Node *temp = *queue;
    *queue = (*queue)->next;
    free(temp);
    return nodeData;
}
 
void printQueue(Node *queue)
{
    while (queue)
    {
        printf("%d ", queue->vertex);
        queue = queue->next;
    }
    printf("\n");
}
 
int route_find(int from, int to, int* predec) {
	while (1){
        if(predec[to] <= 0) {
            return -1;
        }
		if(predec[to] == from) {
			return to;
	  	} else {
			to = predec[to];
	  	}
        sleep(100);
	}
	return -1;
}
