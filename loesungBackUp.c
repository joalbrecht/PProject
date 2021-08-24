#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define START_BUFFER 3000

#define _GNU_SOURCE
int BUFFER_SIZE = 1000;
static int nodeCounter = 0;
uint8_t **adjacencyMatrix = NULL;
uint64_t string_buffer_size = START_BUFFER;
uint32_t *marks = NULL;
char **nodeList = NULL;
int DEBUG = 0;

void printMatrix(uint8_t **memory, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", memory[i][j]);
        }
        printf("\n");
    }
}


void reallocMatrix(int size) {
    if (DEBUG) printf("reallocMatrix() called. size: %d\n", size);
    int previousSize = size-5;
    adjacencyMatrix = (uint8_t**) realloc(adjacencyMatrix, size * sizeof(uint8_t*));
    if (DEBUG) printf("matrix** reallocated. Adress: %p\n", (void*) adjacencyMatrix);
    // TODO error catch allocation error
    for (int i = 0; i < previousSize; i++) {
        adjacencyMatrix[i] = realloc(adjacencyMatrix[i], size * sizeof(uint8_t));
        if (DEBUG) printf("[%d] matrix* reallocated. Adress: %p\n", i, (void*) adjacencyMatrix[i]);
    }
    for (int k = previousSize; k < size; k++) {
        adjacencyMatrix[k] = calloc(size, sizeof(uint8_t));
    }
    if (DEBUG) printf("reallocating matrix done.\n");
    
    for(int i = 0; i < size; i++) { 
        for (int j = 0; j < size; j++) {
            if((i < previousSize) && (j < previousSize)){
                continue;
            }
            adjacencyMatrix[i][j] = 0;
        }
        
    }
}


//checks if Node is Duplicate and returns ID if not duplicate and returns -1 if duplicate
int isDuplicate(char *node) { 
    int i;
    for (i = 0; i < nodeCounter; i++) {
        if (strcmp(nodeList[i], node) == 0) {
            return i;
        }
    }
    return -1;
}

//adds a Node to the global NodeList
void addNode(char *node) {
    char *tmp = malloc((strlen(node) + 1) * sizeof(char));
    strcpy(tmp, node);
    nodeList[nodeCounter] = tmp;
    nodeCounter++;
}

//adds an entry of 1 into the column and row in the global adjacency Matrix
void addEdge(int IDbase, int IDadd) {
    if (DEBUG) {
        printf("Adding Edge between %d|%s and %d|%s\n", IDbase, nodeList[IDbase], IDadd, nodeList[IDadd]);
    }
    adjacencyMatrix[IDbase][IDadd] = 1;
    adjacencyMatrix[IDadd][IDbase] = 1;
}

//sorts a given List, used for sorting neighbournodes list
void sortList(char **List, int n) {
    char *tmp;
    for (int i = 0; i < n - 1; i++) {
        for (int j = i + 1; j < n; j++) {
            if (strcmp(List[i], List[j]) > 0) {
                tmp = List[i];
                List[i] = List[j];
                List[j] = tmp;
            }
        }
    }
}

//gets the number of neighbour nodes, and also modifies the given list of neighbour nodes in the function
int getNeighbourNodes(int nodeId, char** neighbourList) {
    int neighbour_count = 0;
    for (int i = 0; i < nodeCounter; i++)
    {
        if (adjacencyMatrix[nodeId][i] == 1)
        {
            neighbourList[neighbour_count] = nodeList[i];
            neighbour_count++;
        }
    }
    sortList(neighbourList, neighbour_count);
    return neighbour_count;
}

// simulates a step of the ant. calculates where to go by getting the amount of neighbours and the current mark on the Node
int goStep(int currentNode){

    int idNextNode;
    int neighbourStep;
    if(DEBUG)printf("Buffer Size bei nachbarlist: %d\n", BUFFER_SIZE);
    char** neighbourList = malloc(BUFFER_SIZE * sizeof(char*));
    int neighbour_count = getNeighbourNodes(currentNode, neighbourList);
    if (DEBUG)printf("Neighbors von %s: %d\n",nodeList[currentNode], neighbour_count);
    
    neighbourStep = marks[currentNode] % neighbour_count;
   
    idNextNode = isDuplicate(neighbourList[neighbourStep]);
    /*
    for(int i = 0; i < neighbour_count;i++) {
        printf("%s, ", neighbourList[i]);
    } */
    marks[currentNode]++;
    free(neighbourList);
    return idNextNode;
}

//reads the last 2 lines and reads the Starting node, and the number of steps
int getStartConditions(char *input) {
    char *node = malloc((strlen(input)+1) * sizeof(char));
    int startRead = 0;
    int j = 0;

    for (int i = 0; i < (int)strlen(input); i++) {
        if (input[i] == ':') {
            startRead = 1;
            continue;
        }
        if ((startRead == 1 && input[i] != '\n')) {
            node[j] = input[i];
            j++;
        }
        if (input[i] == '\n') {
            node[j] = '\0';
            break;
        }
    }
    if (input[0] == 'A') {
        return isDuplicate(node);
    }
    else {
        return atoi(node);
    }
}

void getNode(char *input){
    if(DEBUG)printf("strlen input bei GetNode: %lu\n",strlen(input));
    char *node = malloc((strlen(input)+1) * sizeof(char));
    int nodeSize = 0;
    int currentNodeIndex = 0;
    int isFirstNode = 1;
    int idFirstNode = 0;
    int idCurrentNode = 0;
    int markerMode = 0;
    
    //iterates through the Whole input line
    for (int i = 0; i < (int)strlen(input); i++)
    {   
        //reads the mark when a '-' has been read
        if (markerMode == 1 ) { // '-' was found, we want node score here
            if(input[i]=='-'){
                continue;
            }
            node[currentNodeIndex] = input[i];
            if (input[i] == '\n') {
                marks[idFirstNode] = atoi(node);
                break;
            }
        }

        //node ist fertig
        if (((input[i] == ':') || (input[i] == ',') || (input[i] == '\n') || input[i] == '-') && (markerMode == 0)) {
            currentNodeIndex = 0;        
            if ((isDuplicate(node) == -1) && strcmp(node, "") != 0) {
                //printf("bis hier\n");
                if(nodeCounter + 3 > BUFFER_SIZE){
                    if(DEBUG)printf("muss reallocated werden: weil: NodeCounter: %d, BUFFERSIZE: %d\n",nodeCounter,BUFFER_SIZE);
                    BUFFER_SIZE = BUFFER_SIZE +5;
                    reallocMatrix(BUFFER_SIZE);
                    if(DEBUG){
                        printf("Matrix nach reallocaten: \n");
                        printMatrix(adjacencyMatrix,BUFFER_SIZE);
                    }
                    if(DEBUG)printf("Matrix reallocated\n");
                    if(DEBUG) printf("nodeList Ptr: %p \n", (void*) nodeList);
                    nodeList = (char**) realloc(nodeList, BUFFER_SIZE * sizeof(char*));
                    if(DEBUG)printf("nodeList reallocated\n");
                    if (DEBUG) printf("marks Ptr: %p \n", (void*) marks);
                    marks = (uint32_t*) realloc(marks, BUFFER_SIZE * sizeof(uint32_t));
                    if(DEBUG)printf("marks reallocated\n");
                    //printf("here2\n");
                }
                
                addNode(node);
                if(DEBUG)printf("node Added Successfully\n");
            }
            if (isFirstNode == 0) {
                idCurrentNode = isDuplicate(node);

                if(idFirstNode!=idCurrentNode){
                    addEdge(idFirstNode, idCurrentNode);
                }
                
            }
            if (input[i] == ':') {
                isFirstNode = 0;
                if (isDuplicate(node) == -1) {
                    idFirstNode = nodeCounter;
                }
                else {
                    idFirstNode = isDuplicate(node);
                    
                }
            }
            if(input[i] == '\n'){
                marks[idFirstNode] = 0;
            }
            nodeSize = 0;
            if (input[i] == '-' ) {
                markerMode = 1;
                continue;
                //add Markierung
            }
            continue;
        }
        
        if (input[i] == '\n') {
            break;
        }

        node[currentNodeIndex] = input[i]; // copy chars to node
        currentNodeIndex++; // TODO j is the same as nodeSize
        
        nodeSize = nodeSize + 1;
        node[nodeSize] = '\0';
    }
    if(DEBUG) printf("Get Node komplett für: %s\n", input);
}


int main (void) {
     
    //INIT MATRIX 
    adjacencyMatrix = calloc(BUFFER_SIZE, sizeof(uint8_t*));
    for (int i = 0; i < BUFFER_SIZE; i++) {
        adjacencyMatrix[i] = calloc(BUFFER_SIZE, sizeof(uint8_t));
    }
    // INIT MATRIX END
    if(DEBUG){
        printf("Matrix: \n");
        printMatrix(adjacencyMatrix,BUFFER_SIZE);
    }
     
    
    char *input_ptr = malloc(START_BUFFER * sizeof(char));
    nodeList = malloc(BUFFER_SIZE * sizeof(char*));
    marks = calloc(BUFFER_SIZE, sizeof(unsigned int));
    
    char *fget = 0;
    //printf("Here\n");
    
    while (strcmp(input_ptr, "")) {
        if(DEBUG)printf("input main: %s",input_ptr);
        memset(input_ptr, 0, strlen(input_ptr) + 1); // check whether +1 is enough to reset whole string
        fget = fgets(input_ptr, string_buffer_size, stdin);
        while (!(input_ptr[strlen(input_ptr) - 1] == '\n')){
            
            string_buffer_size = string_buffer_size * 2; // double buffer size
            if(DEBUG)printf("Reallocate: new string buffer: %d\n", (int)string_buffer_size);
            char *tmp = malloc(string_buffer_size * sizeof(char));
            fget = fgets(tmp, string_buffer_size, stdin);
            input_ptr = realloc(input_ptr, string_buffer_size * sizeof(char) + string_buffer_size / 2);
            input_ptr = strcat(input_ptr, tmp);
            free(tmp);
        }
        
        if (input_ptr[0] == 'A') { //exits the reading loop to get into the StartCondition mode
            break;
        }
        getNode(input_ptr);
    }
    
    int startNodeId = 0;
    startNodeId = getStartConditions(input_ptr); //line starting with A
    fget = fgets(input_ptr, string_buffer_size, stdin);
    int steps = 0;
    steps = getStartConditions(input_ptr); //line starting with I

    if (DEBUG) {
        printf("Schritte: %d\n", steps);
        printf("Meine Nodes: \n");
        for (int i = 0; i < nodeCounter; i++)
        {
            printf("Node: %s, Markierung: %d\n", nodeList[i], marks[i]);
        }
        printf("Node Counter: %d\n", nodeCounter);
        printf("Matrix: \n");
        printMatrix(adjacencyMatrix,BUFFER_SIZE);
    }
    
    int nextNode = startNodeId;
    while (steps > 0) {
     
        steps--;
        nextNode = goStep(nextNode);
    }
    free(adjacencyMatrix);
    for (int j = 0; j < nodeCounter; j++) {
        printf("%s:%d\n", nodeList[j], marks[j]);
    }

    printf("E:%s\n", nodeList[nextNode]);

    return 0;
}
