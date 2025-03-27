#include "../quercus_lib_pico.h"

#include "../libc_builtin.h"
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
        printQueue(queue);
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

void fillLookUpTable(int** lookUp, struct Graph* graph){
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

export int main(void) {
    int look_up[MAX_MODULES][MAX_MODULES];
	Graph* graph;
    char* net_map = get_network_map();
    if(net_map == NULL) return -1;
    int i = 0;

    while(net_map[i] != NULL){
        printf("%c", net_map[i]);
        i++;
    }
    printf("%d\n", net_map);

    graph = convert_to_graph(net_map, SINGLE_VERTEX);
    fillLookUpTable(look_up, graph);
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
                case PATHS_CONFIG:
            }
        }
        e = next_event();
        sleep(10);
    }

    return 0;
}
