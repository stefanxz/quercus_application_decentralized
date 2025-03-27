#include "../quercus_lib_pi.h"

#include "../libc_builtin.h"
#include "../functions/graph.c"
#include "../functions/graph.h"

int lookUp[MAX_MODULES][MAX_MODULES];

void createLookUpTable(int** id_lookup){
	
	return lookUp;
}


void printGraph(struct Graph* graph)
{
    int v;
    for (v = 0; v < MAX_MODULES; v++)
    {
        struct node *temp = graph->adjLists[v];
        printf("\n Adjacency list of vertex %d\n ", v);
        while (temp)
        {
            printf("%d -> ", temp->vertex);
            temp = temp->next;
        }
        printf("\n");
    }
}
 
void bfs(struct Graph* graph, int startVertex, int* predecessors)
{
    struct node *queue = 0;
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
 
        struct node *temp = graph->adjLists[currentVertex];
 
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
	for(int i = 1; i < NUMBER_OF_MODULES; i++){
		graph->visited[i] = 0;
	}
}

int isEmpty(struct node *queue)
{
    return queue == 0;
}
 
void enqueue(struct node **queue, int value)
{
    struct node *newNode = createNode(value);
    if (isEmpty(*queue))
    {
        *queue = newNode;
    }
    else
    {
        struct node *temp = *queue;
        while (temp->next)
        {
            temp = temp->next;
        }
        temp->next = newNode;
    }
}
 
int dequeue(struct node **queue)
{
    int nodeData = (*queue)->vertex;
    struct node *temp = *queue;
    *queue = (*queue)->next;
    free(temp);
    return nodeData;
}
 
void printQueue(struct node *queue)
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
		if(predec[to] == from) {
			return to;
	  	} else {
			to = predec[to];
	  	}
	}
	return -1;
}

void fillLookUpTable(int** lookUp, struct Graph* graph){
	for(int i = 0; i < NUMBER_OF_MODULES; i++){
		for(int j = 0; j < NUMBER_OF_MODULES; j++){
			lookUp[i][j] = -1;
		}
	}

	for(int i = 1; i < NUMBER_OF_MODULES; i++){
		int predecessors[NUMBER_OF_MODULES];
		for(int j = 0; j < NUMBER_OF_MODULES; j++){
			predecessors[j] = -2;
		}

		bfs(graph, i, predecessors);
		
		for(int j = 1; j < NUMBER_OF_MODULES; j++){
			if (i != j && graph -> adjLists[i] != 0) {
				lookUp[i][j] = route_find(i, j, predecessors);
			}
		}
	}
}

export int main(void) {
	struct Graph *graph = createGraph(NUMBER_OF_MODULES);
    printf("\nWhat do you want to do?\n");
    printf("1. Add edge\n");
    printf("2. Print graph\n");
    printf("3. BFS\n");
    printf("4. Exit\n");
    int choice;
    scanf("%d", &choice);
    while (choice != 4)
    {
        if (choice == 1)
        {
            int src, dest;
            printf("Enter source and destination: ");
            scanf("%d %d", &src, &dest);
            addEdge(graph, src, dest);
        }
        else if (choice == 2)
        {
            printGraph(graph);
        }
        else if (choice == 3)
        {
            int startVertex;
            printf("Enter starting vertex: ");
            scanf("%d", &startVertex);
			int predecessor[NUMBER_OF_MODULES];
			for (int i = 0; i < NUMBER_OF_MODULES; i++)
			{
				predecessor[i] = -2;
			}
			
            bfs(graph, startVertex, predecessor);
			for(int j = 0; j < NUMBER_OF_MODULES; j++){
				printf("Distance from %d to %d is %d\n", 0, j, graph->adjLists[startVertex]->dist[j]);
			}
			for(int i = 0; i < NUMBER_OF_MODULES; i++){
				printf("Predecessor of %d is %d\n", i, predecessor[i]);
			}
			int** look_up = createLookUpTable();
			fillLookUpTable(look_up, graph);
			for(int i = 1; i < NUMBER_OF_MODULES; i++){
				for(int j = 1; j < NUMBER_OF_MODULES; j++){
					printf("Route from %d to %d is %d\n", i, j, look_up[i][j]);
				}
			}
        }
        else
        {
            printf("Invalid choice\n");
        }
        printf("What do you want to do?\n");
        printf("1. Add edge\n");
        printf("2. Print graph\n");
        printf("3. BFS\n");
        printf("4. Exit\n");
        scanf("%d", &choice);
    }
    return 0;
}
