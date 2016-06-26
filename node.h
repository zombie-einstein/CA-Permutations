#ifndef NODE_H
#define NODE_H

// File and console streams
#include <iostream>
#include <fstream>

// STD Containers
#include <list>

// Name-spaces
using namespace std;

// Forward definition
class node;

/* ===== GRAPH NODE CHILD CLASS ===== */
// Links pointers to nodes and a boolean value
// used by node to store it child nodes (i.e. adjacent nodes on graph)
// and mark if they have been visited from that node

class nodeChild{

    public:
        /* CONSTRUCTORS */
        nodeChild( node* child );

        /* VARIABLES */
        node* pntr;             // Pointer to child node
        bool check;             // Node visit check

        /* METHODS */
        void mark();            // Mark this child as visited
        void unmark();          // Un-mark this child as visited
        void markVisited();     // Mark this child as globally visited
        void unMarkVisited();   // UnMark this child as globally visited
};

/* ===== GRAPH NODE CLASS ===== */
// Stores the nodes value, whether it has been previously visited
// and the children (adjacent) nodes of this

class node{

    public:
        /* CONSTRUCTORS */
        // Default
        node();
        // With value
        node( int n );

        /* VARIABLES */
        // Nodes value
        int value;
        // Mark if this node has been previously visited (i.e. globally)
        bool visited = false;

        /* CONTAINERS */
        // List of children pointer (i.e. on accessible from this state)
        // allows checking if a child has been visited form this state (i.e. locally)
        list<nodeChild> children;

        /* METHODS */
        // Add a child to this node
        void addChild( nodeChild newChild );

        // Mark all the children of this state as unvisited
        void unmarkChildren();

        // Print the value and children of this node
        void printNodeToFile( ofstream& aFile );
};

/* ===== GRAPH CYCLE CLASS ===== */
// Stores the cycles (closed paths) of a graph given by a list of nodes
// and methods for populating the graph

class cycleList{

    public:

        /* TYPEDEFS */
        typedef list< list<node*> >::iterator listIt;  // Iterator over list
        typedef list<node*>::iterator stckIt;          // Iterator over stack
        typedef list<nodeChild>::iterator childIt;     // Iterator over children

        /* CONTAINERS */
        // List of nodes to be considered
        // using node children allows checking visitation
        list< nodeChild > nodeList;

        // List of stacks which store cycles of nodes
        list< list<node*> > stackList;

        /* METHODS */
        // Add a node to the list
        void addNode( node* newNode );

        // Add cycles from a node to the list
        void cyclesFromNode( childIt start );

        // Remove excess nodes from a path
        void cleanCycles();

        // Re-order paths to put lowest node value at start
        void re_orderCycles();

        // Remove repeated cycles from the list
        void removeRepeated();

        // Print the cycles to a file
        void printCyclesToFile( ofstream& aFile );

};

// Prints the closed cycles of a given set of nodes to file
int printPathsToFile( node nodeList[], int listSize, ofstream& aFile );

#endif // NODE_H
