#include "../elementary_functions/quercus_lib_pico.h"
#include "../elementary_functions/libc_builtin.h"
#include "../elementary_functions/network.h"
#include "../elementary_functions/graph.c"
#include "../elementary_functions/graph.h"
#include "../elementary_functions/essentials.h"

/// @brief Run BFS from the startVertex on graph and fill the predecessors array and nearest_dest array
/// @param graph The graph that BFS will be run on
/// @param startVertex The starting vertex for BFS
/// @param predecessors A list of predecessors for each vertex
/// @param largest_cycle The largest cycle in the graph
/// @param nearest_dest The array of nearest destinations for each module type
/// @return returns -1 if the graph is empty, 0 otherwise
int bfs(Graph* graph, int startVertex, int* predecessors, Cycle* largest_cycle, uint8_t nearest_dest[NUMBER_OF_DEST_TYPES])
{
    // Check if the graph is empty and return -1 if it is
    if(graph->adjLists[startVertex] == NULL) {
        return -1;
    }

    // Initialize the queue
    struct Node *queue = NULL;

    // Fill in the values in the array for the StartVertex
	predecessors[startVertex] = -1;
    graph->visited[startVertex] = 1;
	graph->adjLists[startVertex]->dist[startVertex] = 0;

    // Add the starting vertex to the queue
    enqueue(&queue, startVertex);

    // while the queue is not empty
    while (!isEmpty(queue))
    {
        // Dequeue the current vertex and save it in to currentVertex
        int currentVertex = dequeue(&queue);
        
        // Get the first entry in the adjacency list for the current vertex
        struct Node *temp = graph->adjLists[currentVertex];
        
        // Loop through all the neighbors of the current vertex
        while (temp)
        {
            // Get the adjacent vertex
            int adjVertex = temp->vertex;
			
            // If the adjacent vertex has not been visited yet
            if (graph->visited[adjVertex] == 0)
            {
                // Save the type of the module, corresponding to the node
                int mod_type = modules[adjVertex].type;

                // Assign nearest destinations for the module types
                if(nearest_dest[mod_type] == 0){
                    if(mod_type < NUMBER_OF_DEST_TYPES){
                        sleep(10);
                        nearest_dest[mod_type] = adjVertex;
                    }
                }

                // Assign nearest storage
                for(int i=0; i < largest_cycle->length; i++){
                    if(largest_cycle->nodes[i] == adjVertex && nearest_dest[STORAGE] == 0){
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
	for(int i = 1; i < MAX_NUMBER_OF_MODULES; i++){
        graph->visited[i] = 0;
	}
    return 0;
}

/// @brief Assign a direction to a module based on its adjacent module ID
/// @param curr_vertex The current vertex (module) ID
/// @param adj_look_up_id The adjacent module ID
/// @return The (Direction) direction in which the module is connected to the adjacent module,
///         or OUT if the adjacent module ID is not found in the current module's connections
uint8_t assign_direction(int curr_vertex, int adj_look_up_id){
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
void fillGraphData(struct Graph* graph, uint8_t look_up[MAX_NUMBER_OF_MODULES][MAX_NUMBER_OF_MODULES][2], Cycle* largest_cycle, uint8_t nearest_dest[MAX_NUMBER_OF_MODULES][NUMBER_OF_DEST_TYPES]){
    // Iterate over possible ids of modules
    for(int i = 1; i < highest_id_module+1; i++){
        
        // Initialize the predecessors array to be empty 
        int predecessors[MAX_NUMBER_OF_MODULES];
        for (int j = 0; j < highest_id_module+1; j++) {
            predecessors[j] = 0;
        }
        
        // Run BFS from the current module
        bfs(graph, i, &predecessors, largest_cycle, nearest_dest[i]);

        // Find the path from the current module to all other modules
		for(int j = 1; j <= highest_id_module; j++){
            if (graph -> adjLists[i] != 0) {
                // If the module is reachable, fill in the look up table with the ID and direction
                if(i == j) look_up[i][j][0] = i;
				else look_up[i][j][0] = route_find(i, j, predecessors);
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
int send_path_config(int sender, uint8_t current_look_up[MAX_NUMBER_OF_MODULES][2], Cycle* cycle, uint8_t nearest_dest[NUMBER_OF_DEST_TYPES]){
    char message[2 + MAX_NUMBER_OF_MODULES*3 + NUMBER_OF_DEST_TYPES+ 3] = {0};
    // Set up the message with the sender ID, message type, and the look up table
    message[MSG_SENDER] = 0;
    message[MSG_TYPE] = PATH_CONFIG;
    for(int i = 0; i < MAX_NUMBER_OF_MODULES; i++){
        message[i+2] = current_look_up[i][0];
    }

    for(int i = 0; i < MAX_NUMBER_OF_MODULES; i++){
        message[i+2+MAX_NUMBER_OF_MODULES] = current_look_up[i][1];
    }
    
    // Fill in the cycle data
    for(int i = 0; i < cycle->length; i++){
        message[i+2+MAX_NUMBER_OF_MODULES*2] = cycle->nodes[i];
    }

    // Fill in the nearest destination data
    for(int i = 0; i < NUMBER_OF_DEST_TYPES; i++) {
        message[i+2+MAX_NUMBER_OF_MODULES*3] = nearest_dest[i];
    }
    // Fill in the neighbour module IDs in the order of LEFT, RIGHT, and RFID
    message[2+MAX_NUMBER_OF_MODULES*3 + NUMBER_OF_DEST_TYPES] = modules[sender].a; //LEFT
    message[1+2+MAX_NUMBER_OF_MODULES*3 + NUMBER_OF_DEST_TYPES] = modules[sender].c; //RIGHT
    message[2+2+MAX_NUMBER_OF_MODULES*3 + NUMBER_OF_DEST_TYPES] = modules[sender].b; //RFID
    
    // Send the packet to the sender module
    return send_packet(sender, message, sizeof(message));
}

/// @brief Broadcast the plane status to all modules except the sender
/// @param sender The id of the module that sent the plane status update
/// @param plane_id The id of the plane that is being updated
/// @return 0 for a broadcast that did not crash
int broadcast_plane_status(int sender, char plane_id){
    //Iterate over all modules
    for(int i = 0; i < MAX_NUMBER_OF_MODULES; i++){
        // Sender already knows, so we ignore it
        if(modules[i].id == sender) continue;
        // If the module does not exist, skip it
        if(modules[i].id == 0) continue;
        
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
    if(net_map == NULL) return -1;
    graph = convert_to_graph(net_map, SINGLE_VERTEX, &largest_cycle);

    // Fill the graph data
    fillGraphData(graph, look_up,&largest_cycle, nearest_dest);

    // Loop to receive messages
    EventType e = next_event();
    printf("I am ready to receive.\n");
    while(1){
        if(e == EVENT_MESSAGE_RECEIVED){
            //Save the messge to msg
            char* msg;
            next_message_address(&msg);
            int sender = msg[MSG_SENDER];
            int type = msg[MSG_TYPE];
            switch(type){
                case PLANE_STATUS:
                    //If a plane status message is received, broadcast it to the other Picos
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
                    printf("Sending configuration back. Result: %d\n", send_path_config(sender, look_up[sender], &largest_cycle, nearest_dest[sender]));
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
