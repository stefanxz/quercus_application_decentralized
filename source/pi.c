#include "../lib/quercus_lib_pico.h"
#include "../lib/libc_builtin.h"
#include "../lib/network.h"
#include "../lib/graph.c"
#include "../lib/graph.h"
#include "../lib/essentials.h"

/// @brief Run BFS from the startVertex on graph and fill the predecessors array and nearest_dest array
/// @param graph The graph that BFS will be run on
/// @param startVertex The starting vertex for BFS
/// @param predecessors A list of predecessors for each vertex
/// @param largest_cycle The largest cycle in the graph
/// @param nearest_dest The array of nearest destinations for each module type
/// @return returns -1 if the graph is empty, 0 otherwise
int bfs(Graph* graph, int startVertex, int* predecessors, Cycle* largest_cycle, uint8_t nearest_dest[NUMBER_OF_DEST_TYPES])
{
    if(graph->adjLists[startVertex] == NULL) {
        return -1;
    }
    struct Node *queue = 0;
	predecessors[startVertex] = -1;
    graph->visited[startVertex] = 1;
	graph->adjLists[startVertex]->dist[startVertex] = 0;
    enqueue(&queue, startVertex);
    while (!isEmpty(queue))
    {
        int currentVertex = dequeue(&queue);
        
        struct Node *temp = graph->adjLists[currentVertex];
        
        while (temp)
        {
            int adjVertex = temp->vertex;
			
            if (graph->visited[adjVertex] == 0)
            {
                int mod_type = modules[adjVertex].type;
                //assign module type destinations
                if(nearest_dest[mod_type] == 0){
                    if(mod_type < NUMBER_OF_DEST_TYPES){
                        sleep(10);
                        nearest_dest[mod_type] = adjVertex;
                    }
                }

                //assign storage type destinations
                for(int i=0; i < largest_cycle->length; i++){
                    if(largest_cycle->nodes[i] == adjVertex && nearest_dest[STORAGE] == 0){
                        nearest_dest[STORAGE] = adjVertex;
                        break;
                    }
                }
				predecessors[adjVertex] = currentVertex;
                graph->visited[adjVertex] = 1;
                enqueue(&queue, adjVertex);
				graph->adjLists[startVertex]->dist[adjVertex] = graph->adjLists[startVertex]->dist[currentVertex] + 1;
            }
            temp = temp->next;
        }
    }
	for(int i = 1; i < MAX_NUMBER_OF_MODULES; i++){
        graph->visited[i] = 0;
	}
    return 0;
}

uint8_t assign_direction(int curr_vertex, int adj_look_up_id){
    if (modules[curr_vertex].a == adj_look_up_id) return LASER_LEFT;
    if (modules[curr_vertex].b == adj_look_up_id) return RFID;
    if (modules[curr_vertex].c == adj_look_up_id) return LASER_RIGHT;
    return OUT;
}

void fillGraphData(struct Graph* graph, uint8_t look_up[MAX_NUMBER_OF_MODULES][MAX_NUMBER_OF_MODULES][2], Cycle* largest_cycle, uint8_t nearest_dest[MAX_NUMBER_OF_MODULES][NUMBER_OF_DEST_TYPES]){
    
    for(int i = 1; i < highest_id_module+1; i++){
        int predecessors[MAX_NUMBER_OF_MODULES];
        for (int j = 0; j < highest_id_module+1; j++) {
            predecessors[j] = 0;
        }
        
        bfs(graph, i, &predecessors, largest_cycle, nearest_dest[i]);
		for(int j = 1; j <= highest_id_module; j++){
            if (graph -> adjLists[i] != 0) {
                if(i == j) look_up[i][j][0] = i;
				else look_up[i][j][0] = route_find(i, j, predecessors);
                look_up[i][j][1] = assign_direction(i, look_up[i][j][0]);
			}
		}
        
	}
}

int send_path_config(int sender, uint8_t current_look_up[MAX_NUMBER_OF_MODULES][2], Cycle* cycle, uint8_t nearest_dest[NUMBER_OF_DEST_TYPES]){
    char message[2 + MAX_NUMBER_OF_MODULES*3 + NUMBER_OF_DEST_TYPES+ 3] = {0};
    message[MSG_SENDER] = 0;
    message[MSG_TYPE] = PATH_CONFIG;
    for(int i = 0; i < MAX_NUMBER_OF_MODULES; i++){
        message[i+2] = current_look_up[i][0];
    }

    for(int i = 0; i < MAX_NUMBER_OF_MODULES; i++){
        message[i+2+MAX_NUMBER_OF_MODULES] = current_look_up[i][1];
    }
    
    for(int i = 0; i < cycle->length; i++){
        message[i+2+MAX_NUMBER_OF_MODULES*2] = cycle->nodes[i];
    }

    for(int i = 0; i < NUMBER_OF_DEST_TYPES; i++) {
        message[i+2+MAX_NUMBER_OF_MODULES*3] = nearest_dest[i];
    }
    message[2+MAX_NUMBER_OF_MODULES*3 + NUMBER_OF_DEST_TYPES] = modules[sender].a; //LEFT
    message[1+2+MAX_NUMBER_OF_MODULES*3 + NUMBER_OF_DEST_TYPES] = modules[sender].c; //RIGHT
    message[2+2+MAX_NUMBER_OF_MODULES*3 + NUMBER_OF_DEST_TYPES] = modules[sender].b; //RFID
    
    return send_packet(sender, message, sizeof(message));
}

int broadcast_plane_status(int sender, char plane_id){
    for(int i = 0; i < MAX_NUMBER_OF_MODULES; ++i){
        //sender already knows
        if(modules[i].id == sender) continue;
        if(modules[i].id == 0) continue;
        printf("Sending plane data to: %d\n", modules[i].id);

        char data[ARR_LENGTH];
        data[MSG_SENDER] = 0;
        data[MSG_TYPE] = PLANE_STATUS;
        data[ARR_PLANE_ID] = plane_id;
        data[ARR_MODULE_ID] = sender;

        send_packet(modules[i].id, data, sizeof(data));
    }
    return 0;
}

export int main(void) {
    subscribe_to_event(EVENT_MESSAGE_RECEIVED);
    static uint8_t look_up[MAX_NUMBER_OF_MODULES][MAX_NUMBER_OF_MODULES][2] = {0};
    static uint8_t nearest_dest[MAX_NUMBER_OF_MODULES][NUMBER_OF_DEST_TYPES] = {0};
	Graph* graph;
    Cycle largest_cycle;

    char* net_map = get_network_map();
    if(net_map == NULL) return -1;
    graph = convert_to_graph(net_map, SINGLE_VERTEX, &largest_cycle);
    fillGraphData(graph, look_up,&largest_cycle, nearest_dest);
    EventType e = next_event();
    printf("I am ready to receive.\n");
    while(1){
        if(e == EVENT_MESSAGE_RECEIVED){
            char* msg;
            next_message_address(&msg);
            int sender = msg[MSG_SENDER];
            int type = msg[MSG_TYPE];
            // printf("Message received: sender = %d, type = %d\n", sender, type);
            switch(type){
                case PLANE_STATUS:
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
                    printf("I received a paths configuration request from: %d\n", sender);
                    printf("Sending configuration back. Result: \n", send_path_config(sender, look_up[sender], &largest_cycle, nearest_dest[sender]));
                    break;
                default:
                    break;
            }
        }
        e = next_event();
        sleep(100);
    }

    return 0;
}
