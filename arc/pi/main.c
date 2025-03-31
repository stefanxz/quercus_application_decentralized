#include "../quercus_lib_pico.h"

#include "../libc_builtin.h"
#include "../functions/graph.c"
#include "../functions/graph.h"
#include "../functions/network.h"


int bfs(Graph* graph, int startVertex, int* predecessors)
{
    
    if(graph->adjLists[startVertex] == NULL) {
        printf("I am returning%d\n", startVertex);
        return -1;
    }
    struct Node *queue = 0;
	predecessors[startVertex] = -1;
    graph->visited[startVertex] = 1;
	graph->adjLists[startVertex]->dist[startVertex] = 0;
    enqueue(&queue, startVertex);
	printf("Bossman is here\n");
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
	for(int i = 1; i < MAX_NUMBER_OF_MODULES; i++){
        graph->visited[i] = 0;
	}
    return 0;
}

void fillLookUpTable(uint8_t look_up[MAX_NUMBER_OF_MODULES][MAX_NUMBER_OF_MODULES], struct Graph* graph){
    
    for(int i = 1; i < highest_id_module+1; i++){
        int predecessors[MAX_NUMBER_OF_MODULES];
        for (int j = 0; j < highest_id_module+1; j++) {
            predecessors[j] = 0;
        }
        
        bfs(graph, i, &predecessors);
		
		for(int j = 1; j < highest_id_module+1; j++){
            if (graph -> adjLists[i] != 0) {
                if(i == j) look_up[i][j] = i;
				else look_up[i][j] = route_find(i, j, predecessors);
			}
		}
        
	}
}

int handle_send_path_config(int sender, uint8_t* look_up, Cycle* cycle){
    char message[2+ MAX_NUMBER_OF_MODULES + cycle->length];
    message[MSG_SENDER] = 0;
    message[MSG_TYPE] = PATHS_CONFIG;
    for(int i = 0; i < MAX_NUMBER_OF_MODULES; i++){
        message[i+2] = look_up[i];
    }
    for(int i = 0; i < cycle->length; i++){
        message[i+2+MAX_NUMBER_OF_MODULES] = cycle->nodes[i];
    }
    send_packet(sender, message, 2 + MAX_NUMBER_OF_MODULES + cycle->length);
    return 0;
}

export int main(void) {
    static uint8_t look_up[MAX_NUMBER_OF_MODULES][MAX_NUMBER_OF_MODULES] = {0};
	Graph* graph;
    Cycle largest_cycle;

    char* net_map = get_network_map();
    if(net_map == NULL) return -1;
    graph = convert_to_graph(net_map, SINGLE_VERTEX, &largest_cycle);
    printGraph(graph);
    fillLookUpTable(look_up, graph);
    char text[8][18] = {0};

    for(int i = 1; i < 9; i++){
        text[i][0] = '0' + i;
        for(int j = 1; j < 9; j++){
            text[i][2*j-1] = '0' + look_up[i][j];
            text[i][2*j] = ' '; 
        }
        text[i][17] = '\n';
        printf("%s", text[i]);
        sleep(100);
    }
    EventType e = next_event();
    while(1){
        if(e == EVENT_MESSAGE_RECEIVED){
            char* msg;
            next_message_address(&msg);
            int sender = msg[MSG_SENDER];
            int type = msg[MSG_TYPE];
            switch(type){
                case PLANE_STATUS:
                    printf("Plane status\n");
                    break;
                case REQUEST_MOVEMENT:
                    printf("I am Pi, I should not be receiving movement requests.\n");
                    break;
                case REQUEST_RESPONSE:
                    printf("I am Pi, I should not be receiving movement request responses.\n");
                    break;
                case REQUEST_PATH_CONFIG:
                    printf("kablami\n");
                    handle_send_path_config(sender, look_up[sender], cycleArr);
                    break;
                default:
                    break;
            }
        }
        e = next_event();
        sleep(10);
    }

    return 0;
}
