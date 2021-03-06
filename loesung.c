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


//will free the global memory before an exit
static void freeMemory(){
    for(uint32_t i = 0; i < nodeCounter; i++) {
        free(nodeList[i].name);
        free(nodeList[i].neighbourList);
    }
    free(nodeList);
}

//validates if an input char is valid
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

//validates if a number is valid, different function because it is also used for the mark
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

//returns the ID of a string, if it is already in the nodeList. If not it will return -1
static int isDuplicate(char *node) { 
    for (uint32_t i = 0; i < nodeCounter; i++) {
        if (strcmp(nodeList[i].name, node) == 0) {
            return i;
        }
    }
    return -1;
}

//sorts a given List, used for sorting neighbournodes list //Static weil schneller
static void insertNeighbour(uint32_t ID, uint32_t node, char* input, char* nodeChar) {
    
   
    // check whether its the first neighbour, cause if so the for loop below will fail
    if (nodeList[ID].neighbour_count == 0) {
        
        nodeList[ID].neighbourList[0] = node;
       
    }
    else {
        for (uint32_t i = 0; i < nodeList[ID].neighbour_count; i++) {
             
             int compareID = nodeList[ID].neighbourList[i];
             if(strcmp(nodeList[node].name, nodeList[compareID].name) == 0){
                freeMemory();
                free(input);
                free(nodeChar);
                printf("There was an Invalid Edge in the Input. ERROR Code: %d\n",invalidEdgeERROR);
                exit(invalidEdgeERROR);
                }
            if(strcmp(nodeList[node].name, nodeList[compareID].name) < 0) { // if the first non-matching character in str1 is lower (in ASCII) than that of str2.;
            
                for (uint32_t j = nodeList[ID].neighbour_count; j > i; j--) {
                   
                    nodeList[ID].neighbourList[j] = nodeList[ID].neighbourList[j-1];
                   
                }
                
                nodeList[ID].neighbourList[i] = node;
                return;
            }        
        }
        nodeList[ID].neighbourList[nodeList[ID].neighbour_count] = node;
    }
}

//adds a node to the NodeList
void addNode(char* node) {
    struct Node tmp ; 
   
    char* tmpname = malloc((strlen(node) + 1) * sizeof(char));
    strcpy(tmpname, node);

    tmp.name = tmpname;
    
    tmp.neighbourList = malloc(1 * sizeof(int*));
    nodeList[nodeCounter] = tmp;
    nodeList[nodeCounter].mark = 0;
    nodeList[nodeCounter].neighbour_count = 0;
    nodeCounter++;
}



//adds an entry of 1 into the column and row in the global adjacency Matrix
void addEdge(uint32_t IDbase, uint32_t IDadd,char* input, char* node) {
    
    //Add Edge on Base Node
    nodeList[IDbase].neighbourList = realloc(nodeList[IDbase].neighbourList,(nodeList[IDbase].neighbour_count+1) *sizeof(int*));
    insertNeighbour(IDbase, IDadd, input, node);
    nodeList[IDbase].neighbour_count++;

    //Add Edge on the other Node
    nodeList[IDadd].neighbourList = realloc(nodeList[IDadd].neighbourList,(nodeList[IDadd].neighbour_count+1)*sizeof(int*));
    insertNeighbour(IDadd, IDbase, input, node);
    nodeList[IDadd].neighbour_count++;
}

// simulates a step of the ant. calculates where to go by getting the amount of neighbours and the current mark on the Node
uint32_t goStep(uint32_t currentNode){

    uint32_t idNextNode;
    uint32_t neighbourStep;
   
    //edge case: only one node
    if(nodeCounter == 1){
        nodeList[currentNode].mark++;
        return currentNode;
    }

    // markierung mod number of neighbours
    neighbourStep = nodeList[currentNode].mark % nodeList[currentNode].neighbour_count;
    idNextNode = nodeList[currentNode].neighbourList[neighbourStep];  
    
    nodeList[currentNode].mark++;
    return idNextNode;
}

//reads the last 2 lines and reads the Starting node, and the number of steps
uint32_t getStartConditions(char *input) {
    char *node = malloc((strlen(input)+1) * sizeof(char));
    uint32_t startRead = 0;
    uint32_t j = 0;
    uint32_t returnID = 0;
    for (uint32_t i = 0; i < strlen(input); i++) {
        if (input[i] == ':') {
            startRead = 1;
            continue;
        }
        if(input[i] == '-'){ //start node is not allowed to have a mark
            free(node);
            freeMemory();
            free(input);
            printf("Invalid Format. ERROR Code: %d\n",invalidFormatERROR);
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
    if (input[0] == 'A') { //there is no Start node declaration when there shoud be one
        if(isDuplicate(node) == -1){
            free(node);
            freeMemory();
            free(input);
            printf("Start Node is node inlcuded in the Input. ERROR Code: %d\n",startNodeMissingERROR);
            exit(startNodeMissingERROR);
        }
        returnID = isDuplicate(node);
        free(node);
        return returnID;
    }
    else {
        returnID = atoi(node);
        free(node);
        return returnID;
    }
}



void getNode(char *input){
   
    char *node = malloc((strlen(input)+1) * sizeof(char));
    uint32_t nodeSize = 0;
    uint32_t currentNodeIndex = 0;
    uint32_t isFirstNode = 1;
    uint64_t idFirstNode = UINT32_MAX;
    uint32_t idCurrentNode = 0;
    uint32_t markerMode = 0;
    int noMoreColon = 0;
    int noMoreMinus = 0;
    
    //iterates through the Whole input line
    for (uint32_t i = 0; i < strlen(input); i++)
    {   
        if((input[i] == ':') && noMoreColon ==1){ //double : in one line
            free(node);
            free(input);
            freeMemory();
            printf("Invalid Format. ERROR Code: %d\n",invalidFormatERROR);
            exit(invalidFormatERROR);
        }
        if((input[i] == '-') && noMoreMinus ==1){ // double - in one line
            free(node);
            free(input);
            freeMemory();
            printf("Invalid Format. ERROR Code: %d\n",invalidFormatERROR);
            exit(invalidFormatERROR);
        }
        if((isValidChar(input[i]) == 0)){ //invalid Char is entered
            free(node);
            free(input);
            freeMemory();
            printf("Invalid Char was read. ERROR Code: %d\n",invalidCharERROR);
            exit(invalidCharERROR); 
        }
        //reads the mark when a '-' has been read
        if (markerMode == 1 ) { // '-' was found, we want node score here
            if(input[i]=='-'){
                continue;
                
            }
            if((isValidDigit(input[i]) == 0) && (input[i] != '\n')){ //char in Mark, only numbers allowed
                free(node);
                free(input);
                freeMemory();
                printf("There was a char in the Mark. ERROR Code: %d\n",charInMarkERROR);
                exit(charInMarkERROR);
            }
            
            node[currentNodeIndex] = input[i];
            if (input[i] == '\n') {
                long mark = atoi(node);
                if(mark < 0 || mark > INT32_MAX ){ // mark out of range
                    free(node);
                    free(input);
                    freeMemory();
                    printf("Invalid Format. ERROR Code: %d\n",invalidFormatERROR);
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
            if(nodeSize == 0 && input[i] == '\n'){ //leerer Node, oder knoten alleine
                free(node);
                free(input);
                freeMemory();
                printf("Invalid Format. ERROR Code: %d\n",invalidFormatERROR);
                exit(invalidFormatERROR); 
            }
            if ((isDuplicate(node) == -1) && strcmp(node, "") != 0) {
                if(nodeCounter + 3 > BUFFER_SIZE){
                 
                    BUFFER_SIZE = BUFFER_SIZE +5;
                    nodeList = (struct Node*) realloc(nodeList, BUFFER_SIZE * sizeof(struct Node));

                }
                
                addNode(node);
            }
            if (isFirstNode == 0) {
                
                idCurrentNode = isDuplicate(node);
                if((idFirstNode == idCurrentNode)  && idFirstNode != (INT32_MAX) ){ // node as edge to itself
                    free(node);
                    free(input);
                    freeMemory();
                    printf("Node has an Edge to itself. ERROR Code: %d\n",edgeToSelfERROR);
                    exit(edgeToSelfERROR);
                }
                else{ 
                    addEdge(idFirstNode, idCurrentNode, input, node);
 
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

        node[currentNodeIndex] = input[i]; 
        currentNodeIndex++; 
        
        nodeSize = nodeSize + 1;
        node[nodeSize] = '\0';
    }

    free(node);
}


int main (void) {
     
  
   

    char *input_ptr = NULL; 
    nodeList = malloc(BUFFER_SIZE * sizeof(struct Node));
   
    
    size_t len = 0;
    int startNodeMode = 0;
    uint32_t steps = 0;
    uint32_t startNodeId = 0;
    while (getline(&input_ptr, &len, stdin) != -1) {
          
        if (input_ptr[0] == 'A') { //exits the reading loop to get into the StartCondition
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
        free(input_ptr);
        freeMemory();
        printf("There are no Nodes in the Input. ERROR Code: %d\n",noNodesERROR);
        exit(noNodesERROR);

    }
    if(startNodeMode == 0){
        free(input_ptr);
        freeMemory();
        printf("There is no given Start Node in the Input. ERROR Code: %d\n",noStartNodeERROR);
        exit(noStartNodeERROR);
    }
    if(getline(&input_ptr, &len, stdin) != -1){
        free(input_ptr);
        freeMemory();
        printf("Invalid Format. ERROR Code: %d\n",invalidFormatERROR);
        exit(invalidFormatERROR);
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
    free(input_ptr);
    freeMemory();
    return 0;
}
