#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define START_BUFFER 256
#define  _POSIX_C_SOURCE 200809L
#define _GNU_SOURCE

int BUFFER_SIZE = 1028;
static int nodeCounter = 0;
uint8_t **adjacencyMatrix = NULL;
uint64_t string_buffer_size = START_BUFFER;
uint64_t *marks = NULL;
struct Node *nodeList = NULL;
int DEBUG = 1;

void printMatrix(uint8_t **memory, int size) {
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            printf("%d ", memory[i][j]);
        }
        printf("\n");
    }
}

struct Node {
    char* name;
    char** neighbourList;
    uint32_t neighbour_count;
    uint32_t mark;

};
//sorts a given List, used for sorting neighbournodes list
void sortList(char** List, int8_t n) {
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
void addNeighbour(struct Node targetNode, char* newNode){
    //char* tmp;
    
    //printf("Current Count: %d, New Neighbour: %s\n",targetNode.neighbour_count,newNode);
    targetNode.neighbourList = realloc(targetNode.neighbourList, (targetNode.neighbour_count+1) * sizeof(char*));
    /*for(uint32_t i = 0; i < targetNode.neighbour_count; i++) {
        if(strcmp(targetNode.neighbourList[i],newNode) > 0 ){
            tmp = targetNode.neighbourList[i];
            targetNode.neighbourList[i] = newNode;
            newNode = tmp;
        }
    }*/
    targetNode.neighbourList[targetNode.neighbour_count] = newNode;
    sortList(targetNode.neighbourList,targetNode.neighbour_count);
    targetNode.neighbour_count++;
    //free(tmp);
}

void addNode(char* node){ 
    struct Node tmp ;
    tmp.name = malloc((strlen(node) + 1) * sizeof(char));
    strcpy(tmp.name, node);
    tmp.neighbourList = malloc(1*sizeof(char*));
    nodeList[nodeCounter] = tmp;
    nodeList[nodeCounter].mark = 0;
    nodeCounter++;
    
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
        if (strcmp(nodeList[i].name, node) == 0) {
            return i;
        }
    }
    return -1;
}

//adds a Node to the global NodeList
/*void addNode2(char *node) {
    char *tmp = malloc((strlen(node) + 1) * sizeof(char));
    strcpy(tmp, node);
    nodeList[nodeCounter] = tmp;
    nodeCounter++;
}*/

//adds an entry of 1 into the column and row in the global adjacency Matrix
void addEdge(int IDbase, int IDadd) {
    if (DEBUG) {
        printf("Adding Edge between %d|%s and %d|%s\n", IDbase, nodeList[IDbase].name, IDadd, nodeList[IDadd].name);
    }
    //Add Edge on Base
    nodeList[IDbase].neighbourList = realloc(nodeList[IDbase].neighbourList, (nodeList[IDbase].neighbour_count+1) * sizeof(char*));
    nodeList[IDbase].neighbourList[nodeList[IDbase].neighbour_count] = nodeList[IDadd].name;
    printf("Neighbourcount: %d\n",nodeList[IDbase].neighbour_count);
    sortList(nodeList[IDbase].neighbourList,(int)nodeList[IDbase].neighbour_count);
    nodeList[IDbase].neighbour_count++;
    
    //add Edge on Add
    nodeList[IDadd].neighbourList = realloc(nodeList[IDadd].neighbourList, (nodeList[IDadd].neighbour_count+1) * sizeof(char*));
    nodeList[IDadd].neighbourList[nodeList[IDbase].neighbour_count] = nodeList[IDbase].name;
    sortList(nodeList[IDadd].neighbourList,(int)nodeList[IDadd].neighbour_count);
    nodeList[IDadd].neighbour_count++;
  
}



//gets the number of neighbour nodes, and also modifies the given list of neighbour nodes in the function
/*int getNeighbourNodes(int nodeId, char** neighbourList) {
    neighbour_count = 0;
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
}*/

// simulates a step of the ant. calculates where to go by getting the amount of neighbours and the current mark on the Node
int goStep(int currentNode){

    int idNextNode;
    int neighbourStep;
    //if(DEBUG)printf("Buffer Size bei nachbarlist: %d\n", BUFFER_SIZE);
    
    //printf("Neighbors von %s: %d\n",nodeList[currentNode].name, nodeList[currentNode].neighbour_count);
    
    neighbourStep = nodeList[currentNode].mark % nodeList[currentNode].neighbour_count;
    //printf("neighbourstep: %d\n",neighbourStep);
    idNextNode = isDuplicate(nodeList[currentNode].neighbourList[neighbourStep]);
    /*
    for(int i = 0; i < neighbour_count;i++) {
        printf("%s, ", neighbourList[i]);
    } */
    //idNextNode = 0 ;
    nodeList[currentNode].mark++;
    //free(neighbourList);
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
                nodeList[idFirstNode].mark = atoi(node);
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
                    
                    if(DEBUG)printf("Matrix reallocated\n");
                    if(DEBUG) printf("nodeList Ptr: %p \n", (void*) nodeList);
                    nodeList = (struct Node*) realloc(nodeList, BUFFER_SIZE * sizeof(struct Node));
                    if(DEBUG)printf("nodeList reallocated\n");
                    if (DEBUG) printf("marks Ptr: %p \n", (void*) marks);
                    marks = (uint64_t*) realloc(marks, BUFFER_SIZE * sizeof(uint64_t));
                    if(DEBUG)printf("marks reallocated\n");
                    //printf("here2\n");
                }
                
                addNode(node);
                if(DEBUG)printf("node: %s Added Successfully\n", node);
            }
            if (isFirstNode == 0) {
                idCurrentNode = isDuplicate(node);

                if(idFirstNode!=idCurrentNode){
                    if(DEBUG)printf("Neighbors von %s vorher: %d\n",nodeList[idFirstNode].name,nodeList[idFirstNode].neighbour_count);
                    addEdge(idFirstNode, idCurrentNode);
                    if(DEBUG)printf("Neighbors von %s nachher: %d\n",nodeList[idFirstNode].name,nodeList[idFirstNode].neighbour_count);

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
            /*if(input[i] == '\n'){
                marks[idFirstNode] = 0;

            }*/
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
    
     
    
    char *input_ptr = malloc(START_BUFFER * sizeof(char*));
    nodeList = malloc(BUFFER_SIZE * sizeof(struct Node));
    marks = calloc(BUFFER_SIZE, sizeof(unsigned int));
    
    size_t len = 0;
    //printf("Here\n");
    
    while (strcmp(input_ptr, "")) {
        if(DEBUG)printf("input main: %s",input_ptr);
        memset(input_ptr, 0, strlen(input_ptr) + 1); // check whether +1 is enough to reset whole string
        getline(&input_ptr, &len, stdin);
        while (!(input_ptr[strlen(input_ptr) - 1] == '\n')){
            
            string_buffer_size = string_buffer_size * 2; // double buffer size
            if(DEBUG)printf("Reallocate: new string buffer: %d\n", (int)string_buffer_size);
            char *tmp = malloc(string_buffer_size * sizeof(char));
            getline(&tmp, &len, stdin);
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
    getline(&input_ptr, &len, stdin);
    int steps = 0;
    steps = getStartConditions(input_ptr); //line starting with I

    if (DEBUG) {
        printf("Schritte: %d\n", steps);
        printf("Meine Nodes: \n");
        for (int i = 0; i < nodeCounter; i++)
        {
            printf("Node: %s, Markierung: %llu\n", nodeList[i].name, marks[i]);
        }
        printf("Node Counter: %d\n", nodeCounter);
        printf("Matrix: \n");
        //printMatrix(adjacencyMatrix,BUFFER_SIZE);
    }
    
    int nextNode = startNodeId;
    while (steps > 0) {
     
        steps--;
        //nextNode = goStep(nextNode);
    }
    free(adjacencyMatrix);
    for (int j = 0; j < nodeCounter; j++) {
        printf("%s:%llu\n", nodeList[j].name, marks[j]);
    }

    printf("E:%s\n", nodeList[nextNode].name);

    return 0;
}
