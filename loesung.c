#define _GNU_SOURCE
#define  _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define START_BUFFER 256

uint32_t BUFFER_SIZE = 64;
static uint32_t nodeCounter = 0;
struct Node *nodeList = NULL;
int DEBUG = 0;

#define invalidCharERROR 99
#define invalidMarkERROR 88
#define invalidFormatERROR 77
#define noStartNodeERROR 66
#define invalidEdgeERROR 55
#define charInMarkERROR 44
#define doubleNodeERROR 33
#define edgeToSelfERROR 22
#define noNodesERROR 11
#define startNodeMissingERROR 111



struct Node {
    char* name;
    uint32_t* neighbourList;
    uint32_t neighbour_count;
    uint32_t mark;
};



static int isValidChar(char inputChar){
    int castedChar = (int)inputChar;
    if(castedChar>= 97 && castedChar<=122){ //between a-z
        return 1;
    }
    // A,I,:,",",-, \n
    if(castedChar == 65 || castedChar== 73 || castedChar == 58 || castedChar == 44|| castedChar== 45 || castedChar == '\n'){
        return 1;
    }
    if(castedChar>= 48 && castedChar <=57){
        return 1;
    }
    else{
        return 0;
    }
 
}

static int isValidDigit(char inputChar){
    int castedChar = (int)inputChar;
    //0-9
    if(castedChar>= 48 && castedChar <=57){
        return 1;
    }
    else{
        return 0;
    }
}


static int isDuplicate(char *node) { 
    for (uint32_t i = 0; i < nodeCounter; i++) {
        if (strcmp(nodeList[i].name, node) == 0) {
            return i;
        }
    }
    return -1;
}

//sorts a given List, used for sorting neighbournodes list //Static weil schneller
static void insertNeighbour(uint32_t ID, uint32_t node) {
    //char *tmp = malloc((strlen(node) + 1) * sizeof(char));
    //strcpy(tmp,node);
    //uint32_t tmp = node;
    if(DEBUG){
        printf("neighbour add function started \n");
    }
    // check whether its the first neighbour, cause if so the for loop below will fail
    if (nodeList[ID].neighbour_count == 0) {
        
        nodeList[ID].neighbourList[0] = node;
        if(DEBUG){
        printf("%d ist erster Nachbar \n", node);
        }
    }
    else {
        for (uint32_t i = 0; i < nodeList[ID].neighbour_count; i++) {
             
             int compareID = nodeList[ID].neighbourList[i];
             if(strcmp(nodeList[node].name, nodeList[compareID].name) == 0){
                exit(invalidEdgeERROR);
                }
            if(strcmp(nodeList[node].name, nodeList[compareID].name) < 0) { // if the first non-matching character in str1 is lower (in ASCII) than that of str2.;
                if(DEBUG){
                    //printf("%s ist kleiner als %d und  i: %d, neighbourcount: %d \n", node,nodeList[ID].neighbourList[i],i,nodeList[ID].neighbour_count);
                }
                for (uint32_t j = nodeList[ID].neighbour_count; j > i; j--) {
                    if(DEBUG)printf("shifte %d von Position %d nach %d \n",nodeList[ID].neighbourList[j-1], j-1, j);
                    if(DEBUG) printf("nodeList j: %d,\n", nodeList[ID].neighbourList[j]);
                    nodeList[ID].neighbourList[j] = nodeList[ID].neighbourList[j-1];
                    if(DEBUG)printf("nodelist j after: %d\n", nodeList[ID].neighbourList[j]);
                }
                if(DEBUG){
                    printf("neighbours von %s before insert: \n", nodeList[ID].name);
                    
                }
                //if(DEBUG)printf("füge %s ein an Index %d", tmp,i);
                
                nodeList[ID].neighbourList[i] = node;
                return;
            }        
        }
        nodeList[ID].neighbourList[nodeList[ID].neighbour_count] = node;
    }
    if(DEBUG){
        printf("neighbours von %s: \n", nodeList[ID].name);
        for(uint32_t i = 0; i < nodeList[ID].neighbour_count+1; i++){
            printf("%d, \n", nodeList[ID].neighbourList[i]);
        }
    }
}


void addNode(char* node) {
    struct Node tmp ; //= (struct Node*) malloc(1 * sizeof(struct Node));
   
    char* tmpname = malloc((strlen(node) + 1) * sizeof(char));
    strcpy(tmpname, node);

    tmp.name = tmpname;
    
    tmp.neighbourList = malloc(1 * sizeof(int*));
    nodeList[nodeCounter] = tmp;
    nodeList[nodeCounter].mark = 0;
    nodeList[nodeCounter].neighbour_count = 0;
    nodeCounter++;
}


//checks if Node is Duplicate and returns ID if not duplicate and returns -1 if duplicate


//adds an entry of 1 into the column and row in the global adjacency Matrix
void addEdge(uint32_t IDbase, uint32_t IDadd) {
    if (DEBUG) {
        printf("Adding Edge between %d|%s and %d|%s\n", IDbase, nodeList[IDbase].name, IDadd, nodeList[IDadd].name);
        printf("Neighbourcount von base %s: %d\n",nodeList[IDbase].name, (int)nodeList[IDbase].neighbour_count);
        printf("fail? \n");
        for(uint32_t i = 0; i < nodeList[IDbase].neighbour_count; i++){
            printf("%d, ", nodeList[IDbase].neighbourList[i]);
        }
    }
    //Add Edge on Base
    
    nodeList[IDbase].neighbourList = realloc(nodeList[IDbase].neighbourList,(nodeList[IDbase].neighbour_count+1) *sizeof(int*));
    insertNeighbour(IDbase, IDadd);
    nodeList[IDbase].neighbour_count++;

    if(DEBUG){
        printf("Neighbourcount von base %s: %d\n",nodeList[IDbase].name, (int)nodeList[IDbase].neighbour_count);
        for(uint32_t i = 0; i < nodeList[IDbase].neighbour_count; i++){
            printf("%d, ", nodeList[IDbase].neighbourList[i]);
        }
    printf("Neighbourcount von add %s: %d\n",nodeList[IDadd].name, (int)nodeList[IDadd].neighbour_count);
    for(uint32_t i = 0; i < nodeList[IDadd].neighbour_count; i++){
            printf("%d, ", nodeList[IDadd].neighbourList[i]);
        }
    }
    //Add Edge on Add
    
    nodeList[IDadd].neighbourList = realloc(nodeList[IDadd].neighbourList,(nodeList[IDadd].neighbour_count+1)*sizeof(int*));
    insertNeighbour(IDadd, IDbase);
    nodeList[IDadd].neighbour_count++;
    //free(tmp);
}

// simulates a step of the ant. calculates where to go by getting the amount of neighbours and the current mark on the Node
uint32_t goStep(uint32_t currentNode){

    uint32_t idNextNode;
    uint32_t neighbourStep;
    if(DEBUG){
        printf("Step: documentation: \n");
        
    }
    //edge case: only one node
    if(nodeCounter == 1){
        nodeList[currentNode].mark++;
    }

    neighbourStep = nodeList[currentNode].mark % nodeList[currentNode].neighbour_count;
    idNextNode = nodeList[currentNode].neighbourList[neighbourStep];
    if(DEBUG){
        printf("Neighbours von current node %s: \n", nodeList[currentNode].name);
        for(uint32_t i = 0; i < nodeList[currentNode].neighbour_count; i++){
            printf("%d, ",nodeList[currentNode].neighbourList[i]);
        }
        printf("current mark: %d, neighbourcount: %d\n", nodeList[currentNode].mark,nodeList[currentNode].neighbour_count);
        printf("neighbourstep: %d, idNextNode: %d, nextNode: %s, currentNode: %s \n",neighbourStep,idNextNode,nodeList[idNextNode].name,nodeList[currentNode].name);
    }
  
    
    nodeList[currentNode].mark++;
    return idNextNode;
}

//reads the last 2 lines and reads the Starting node, and the number of steps
uint32_t getStartConditions(char *input) {
    char *node = malloc((strlen(input)+1) * sizeof(char));
    uint32_t startRead = 0;
    uint32_t j = 0;

    for (uint32_t i = 0; i < strlen(input); i++) {
        if (input[i] == ':') {
            startRead = 1;
            continue;
        }
        if(input[i] == '-'){
            exit(invalidFormatERROR);
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
        if(isDuplicate(node) == -1){
            exit(startNodeMissingERROR);
        }
        return isDuplicate(node);
    }
    else {
        return atoi(node);
    }
}



void getNode(char *input){
    if(DEBUG)printf("strlen input bei GetNode: %lu\n",strlen(input));
    char *node = malloc((strlen(input)+1) * sizeof(char));
    uint32_t nodeSize = 0;
    uint32_t currentNodeIndex = 0;
    uint32_t isFirstNode = 1;
    uint32_t idFirstNode = 0;
    uint32_t idCurrentNode = 0;
    uint32_t markerMode = 0;
    int noMoreColon = 0;
    int noMoreMinus = 0;
    
    //iterates through the Whole input line
    for (uint32_t i = 0; i < strlen(input); i++)
    {   
        if((input[i] == ':') && noMoreColon ==1){
            exit(invalidFormatERROR);
        }
        if((input[i] == '-') && noMoreMinus ==1){
            exit(invalidFormatERROR);
        }
        if((isValidChar(input[i]) == 0)){//|| isValidDigit(input[i]) == 0){
            exit(invalidCharERROR); 
        }
        //reads the mark when a '-' has been read
        if (markerMode == 1 ) { // '-' was found, we want node score here
            if(input[i]=='-'){
                continue;
                
            }
            if((isValidDigit(input[i]) == 0) && (input[i] != '\n')){
                exit(charInMarkERROR);
            }
            
            node[currentNodeIndex] = input[i];
            if (input[i] == '\n') {
                //marks[idFirstNode] = atoi(node);
                long mark = atoi(node);
                if(mark < 0 || mark > INT32_MAX ){
                    exit(invalidMarkERROR);
                }
                nodeList[idFirstNode].mark =  atoi(node);
                break;
            }
        }
        if((input[i] == ':')){
            noMoreColon = 1;
        }
        if((input[i] == '-')){
            noMoreMinus = 1;
        }
        if(i>1){
            if(input[i] == '-' && input[i-1] == ':'){
            markerMode = 1;
            continue;
            }
        }
        

        //node ist fertig
        if (((input[i] == ':') || (input[i] == ',') || (input[i] == '\n') || input[i] == '-') && (markerMode == 0)) {
            currentNodeIndex = 0;
            if(nodeSize == 0 && input[i] == '\n'){
                exit(invalidFormatERROR); //leerer Node, oder knoten alleine
            }
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
                
                if(idFirstNode != idCurrentNode){
                    if(DEBUG)printf("Neighbors von %s vorher: %d\n",nodeList[idFirstNode].name,nodeList[idFirstNode].neighbour_count);
                    addEdge(idFirstNode, idCurrentNode);
                    if(DEBUG)printf("Neighbors von %s nachher: %d\n",nodeList[idFirstNode].name,nodeList[idFirstNode].neighbour_count);

                }
                if((idFirstNode == idCurrentNode)&& idFirstNode!= 0 ){
                    exit(edgeToSelfERROR);
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
    free(node);
}


int main (void) {
     
  
   

    char *input_ptr = NULL; //malloc(START_BUFFER * sizeof(char*));
    nodeList = malloc(BUFFER_SIZE * sizeof(struct Node));
   
    
    size_t len = 0;
    int startNodeMode = 0;
    uint32_t steps = 0;
    uint32_t startNodeId = 0;
    while (getline(&input_ptr, &len, stdin) != -1) {
        if(DEBUG)printf("input main: %s",input_ptr);
       
        if (input_ptr[0] == 'A') { //exits the reading loop to get into the StartCondition mode
            startNodeMode = 1;
            startNodeId = getStartConditions(input_ptr);
        }
        if(input_ptr[0] == 'I'){
            steps = getStartConditions(input_ptr);
            break;
        }
        if(startNodeMode == 0){
            getNode(input_ptr);
        }
        
    }
    if(nodeCounter == 0){
        exit(noNodesERROR);
    }
    if(startNodeMode == 0){
        exit(noStartNodeERROR);
    }
    if(getline(&input_ptr, &len, stdin) != -1){
        exit(invalidFormatERROR);
    }

    if(DEBUG) {
         for(uint32_t i = 0; i < nodeCounter; i++){
        printf("Neighbours von %s am Ende: \n", nodeList[i].name);
        for(uint32_t j = 0; j < nodeList[i].neighbour_count; j++){
            printf("%d, ", nodeList[i].neighbourList[j]);
        }
    }
    }

    if (DEBUG) {
        printf("Schritte: %d\n", steps);
        printf("Meine Nodes: \n");
        for (uint32_t i = 0; i < nodeCounter; i++)
        {
            printf("Node: %s, Markierung: %d\n", nodeList[i].name, (int)nodeList[i].mark);
        }
        printf("Node Counter: %d\n", nodeCounter);
        printf("Matrix: \n");
        //printMatrix(adjacencyMatrix,BUFFER_SIZE);
    }
    
    uint32_t nextNode = startNodeId;
    while (steps > 0) {
     
        steps--;
        nextNode = goStep(nextNode);
    }

    for (uint32_t j = 0; j < nodeCounter; j++) {
        printf("%s:%d\n", nodeList[j].name, (int)nodeList[j].mark);
    }

    printf("E:%s\n", nodeList[nextNode].name);

    return 0;
}
