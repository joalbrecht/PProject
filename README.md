# PProject
This a code to solve a algorithmic problem given for a task in university.
The task is:
An ant comes across a network of nodes.
Each node has a mark, and a set of neighbours. The mark of the node will be highered by 1 as soon as the ant leaves the node to go to a neighbournode.
The next node is selected by: mark of current node % number of neighbours.

Input is given in the format of:
a:b33,c
b33:c,e-2
cc:e
e:-255
A:c
I:5


The input letter before the : is giving a base node, and the nodes after the : are edges/neighbors of the node.
No Edge will be put in twice, means the node e for the example will not have the edge to the node cc again in it.
The number behind the - is the current mark of the base node, if no number is given it has a default value of 0.
A: describes the start node, and I: the number of steps the ant has to do.

The output will be given in all nodes, that are there, and the number of marks after the ant has gone through it: Example:
a:1
b33:3
c:1
cc:1
e:256
E:e

There are example test files for the algorithm.
