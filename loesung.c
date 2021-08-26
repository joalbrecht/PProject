#define _GNU_SOURCE
#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define START_BUFFER 256



int BUFFER_SIZE = 64;
static int nodeCounter = 0;
uint64_t string_buffer_size = START_BUFFER;
//uint64_t *marks = NULL;
struct Node *nodeList = NULL;
int DEBUG = 1;


struct Node {
    char* name;
    char** neighbourList;
    int neighbour_count;
    uint32_t mark;

};
//sorts a given List, used for sorting neighbournodes list //Static weil schneller
static void insertNeighbour(int ID, char* node) {
    char *tmp = malloc((strlen(node) + 1) * sizeof(char));

    // check whether its the first neighbour, cause if so the for loop below will fail
    if (nodeList[ID].neighbour_count == 0) {
        strcpy(tmp,node);
        nodeList[ID].neighbourList[0] = tmp;
    }
    else {
        for (int i = 0; i < nodeList[ID].neighbour_count; i++) {
            if(strcmp(node, nodeList[ID].neighbourList[i]) < 0) { // if the first non-matching character in str1 is lower (in ASCII) than that of str2.;;
                strcpy(tmp,node);
                for(int j = nodeList[ID].neighbour_count-1; j > i; j--) {
                    nodeList[ID].neighbourList[j] = nodeList[ID].neighbourList[j-1];
                }
                nodeList[ID].neighbourList[i] = tmp;
                break;
            }
        }
    }
}


void addNode(char* node) { 
    struct Node tmp = malloc(1 * sizeof(struct Node));
   
    char* tmpname = malloc((strlen(node) + 1) * sizeof(char));
    strcpy(tmpname, node);

    tmp.name = tmpname;
    
    tmp.neighbourList = malloc(1 * sizeof(char*));
    nodeList[nodeCounter] = tmp;
    nodeList[nodeCounter].mark = 0;
    nodeList[nodeCounter].neighbour_count = 0;
    nodeCounter++;
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



//adds an entry of 1 into the column and row in the global adjacency Matrix
void addEdge(int IDbase, int IDadd) {
    if (DEBUG) {
        
    
    printf("Adding Edge between %d|%s and %d|%s\n", IDbase, nodeList[IDbase].name, IDadd, nodeList[IDadd].name);
    printf("Neighbourcount von base %s: %d\n",nodeList[IDbase].name, (int)nodeList[IDbase].neighbour_count);
    
    for(int i = 0; i < (int)nodeList[IDbase].neighbour_count; i++){
        printf("%s, ", nodeList[IDbase].neighbourList[i]);
    }
    }
    //Add Edge on Base
    
    nodeList[IDbase].neighbourList = realloc(nodeList[IDbase].neighbourList,(nodeList[IDbase].neighbour_count+1) *sizeof(char*));
    insertNeighbour(IDbase, nodeList[IDadd].name);
    nodeList[IDbase].neighbour_count++;

    if(DEBUG){
        printf("Neighbourcount von base %s: %d\n",nodeList[IDbase].name, (int)nodeList[IDbase].neighbour_count);
        for(int i = 0; i < (int)nodeList[IDbase].neighbour_count; i++){
            printf("%s, ", nodeList[IDbase].neighbourList[i]);
        }
    printf("Neighbourcount von add %s: %d\n",nodeList[IDadd].name, (int)nodeList[IDadd].neighbour_count);
    }
    //Add Edge on Add
    
    nodeList[IDadd].neighbourList = realloc(nodeList[IDadd].neighbourList,(nodeList[IDadd].neighbour_count+1)*sizeof(char*));
    insertNeighbour(IDadd, nodeList[IDbase].name);
    nodeList[IDadd].neighbour_count++;
    //free(tmp);
    
}




// simulates a step of the ant. calculates where to go by getting the amount of neighbours and the current mark on the Node
int goStep(int currentNode){

    int idNextNode;
    int neighbourStep;
    if(DEBUG){
        printf("Step: documentation: \n");
        
    }
    neighbourStep = nodeList[currentNode].mark % nodeList[currentNode].neighbour_count;
    idNextNode = isDuplicate(nodeList[currentNode].neighbourList[neighbourStep]);
    if(DEBUG){
        printf("Neighbours von current node %s: \n", nodeList[currentNode].name);
        for(int i = 0; i < nodeList[currentNode].neighbour_count; i++){
            printf("%s, ",nodeList[currentNode].neighbourList[i]);
        }
        printf("current mark: %d, neighbourcount: %d\n", nodeList[currentNode].mark,nodeList[currentNode].neighbour_count);
        printf("neighbourstep: %d, idNextNode: %d, nextNode: %s, currentNode: %s \n",neighbourStep,idNextNode,nodeList[idNextNode].name,nodeList[currentNode].name);
    }
    /*
    for(int i = 0; i < neighbour_count;i++) {
        printf("%s, ", neighbourList[i]);
    } */
    
    nodeList[currentNode].mark++;
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
                //marks[idFirstNode] = atoi(node);
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
                    
                    if(DEBUG) printf("nodeList Ptr: %p \n", (void*) nodeList);
                    nodeList = (struct Node*) realloc(nodeList, BUFFER_SIZE * sizeof(struct Node));
                    if(DEBUG)printf("nodeList reallocated\n");
                   
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
     
  
   

    char *input_ptr = NULL; //malloc(START_BUFFER * sizeof(char*));
    nodeList = malloc(BUFFER_SIZE * sizeof(struct Node));
   
    
    size_t len = 0;

    while (getline(&input_ptr, &len, stdin) != -1) {
        if(DEBUG)printf("input main: %s",input_ptr);
       
        if (input_ptr[0] == 'A') { //exits the reading loop to get into the StartCondition mode
            break;
        }
        getNode(input_ptr);
    }
    if(DEBUG) {
         for(int i = 0; i < nodeCounter; i++){
        printf("Neighbours von %s am Ende: \n", nodeList[i].name);
        for(int j = 0; j < (int)nodeList[i].neighbour_count; j++){
            printf("%s, ", nodeList[i].neighbourList[j]);
        }
    }
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
            printf("Node: %s, Markierung: %d\n", nodeList[i].name, (int)nodeList[i].mark);
        }
        printf("Node Counter: %d\n", nodeCounter);
        printf("Matrix: \n");
        //printMatrix(adjacencyMatrix,BUFFER_SIZE);
    }
    
    int nextNode = startNodeId;
    while (steps > 0) {
     
        steps--;
        nextNode = goStep(nextNode);
    }

    for (int j = 0; j < nodeCounter; j++) {
        printf("%s:%d\n", nodeList[j].name, (int)nodeList[j].mark);
    }

    printf("E:%s\n", nodeList[nextNode].name);

    return 0;
}
