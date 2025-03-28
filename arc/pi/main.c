#include "../quercus_lib_pico.h"

#include "../libc_builtin.h"
#include "../functions/graph.c"
#include "../functions/graph.h"
#include "../functions/network.h"
 
void bfs(Graph* graph, int startVertex, int* predecessors)
{
    struct Node *queue = 0;
	predecessors[startVertex] = -1;
    graph->visited[startVertex] = 1;
	graph->adjLists[startVertex]->dist[startVertex] = 0;
	printf("Distance from %d to %d is %d\n", startVertex, startVertex, graph->adjLists[startVertex]->dist[startVertex]);
    enqueue(&queue, startVertex);
	
    while (!isEmpty(queue))
    {
        int currentVertex = dequeue(&queue);
        printf("Visited %d\n", currentVertex);
 
        struct Node *temp = graph->adjLists[currentVertex];
 
        while (temp)
        {
            int adjVertex = temp->vertex;
			
            if (graph->visited[adjVertex] == 0)
            {
				predecessors[adjVertex] = currentVertex;
                graph->visited[adjVertex] = 1;
                enqueue(&queue, adjVertex);
				graph->adjLists[startVertex]->dist[adjVertex] = graph->adjLists[startVertex]->dist[currentVertex] + 1;
            }
            temp = temp->next;
        }
    }
	for(int i = 1; i < MAX_MODULES; i++){
		graph->visited[i] = 0;
	}
}

void fillLookUpTable(uint8_t** lookUp, struct Graph* graph){
	for(int i = 0; i < MAX_MODULES; i++){
		for(int j = 0; j < MAX_MODULES; j++){
			lookUp[i][j] = -1;
		}
	}

	for(int i = 1; i < MAX_MODULES; i++){
		int predecessors[MAX_MODULES];

		bfs(graph, i, predecessors);
		
		for(int j = 1; j < MAX_MODULES; j++){
			if (graph -> adjLists[i] != 0) {
                if(i == j) lookUp[i][j] = i;
				else lookUp[i][j] = route_find(i, j, predecessors);
			}
		}
	}
}

int handle_send_path_config(int sender, uint8_t* look_up, Cycle* cycle){
    char message[2+ MAX_MODULES + cycle->length];
    message[MSG_SENDER] = 0;
    message[MSG_TYPE] = PATHS_CONFIG;
    for(int i = 0; i < MAX_MODULES; i++){
        message[i+2] = look_up[i];
    }
    for(int i = 0; i < cycle->length; i++){
        message[i+2+MAX_MODULES] = cycle->nodes[i];
    }
    send_packet(sender, message, 2 + MAX_MODULES + cycle->length);
    return 0;
}


export int main(void) {
    printf("waduhek\n");
    static uint8_t look_up[MAX_MODULES][MAX_MODULES];
	Graph* graph;
    Cycle largest_cycle;

    char* net_map = get_network_map();
    if(net_map == NULL) return -1;
    printf("%s\n", net_map);

    graph = convert_to_graph(net_map, SINGLE_VERTEX, &largest_cycle);
    // printf("WTF is a Kilometer: %d", largest_cycle.length);
    // fillLookUpTable(look_up, graph);
    // EventType e = next_event();
    // while(1){
    //     if(e == EVENT_MESSAGE_RECEIVED){
    //         char* msg;
    //         next_message_address(&msg);
    //         int sender = msg[MSG_SENDER];
    //         int type = msg[MSG_TYPE];
    //         switch(type){
    //             case PLANE_STATUS:
    //                 printf("Plane status\n");
    //                 break;
    //             case REQUEST_MOVEMENT:
    //                 printf("I am Pi, I should not be receiving movement requests.\n");
    //                 break;
    //             case REQUEST_RESPONSE:
    //                 printf("I am Pi, I should not be receiving movement request responses.\n");
    //                 break;
    //             case REQUEST_PATH_CONFIG:
    //                 printf("kablami\n");
    //                 handle_send_path_config(sender, look_up[sender], cycleArr);
    //                 break;
    //             default:
    //                 break;
    //         }
    //     }
    //     e = next_event();
    //     sleep(10);
    // }

    return 0;
}
