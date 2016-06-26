#ifndef NODE_H
#define NODE_H

// File and console streams
#include <iostream>
#include <fstream>

// STD Containers
#include <vector>
#include <list>
#include <array>

// Name-spaces
using namespace std;

/* ===== NODE GRAPH CLASS USED TO FIND PATHS OF TRANSMISSION GRAPH ===== */
// Stores the nodes value and whether it has been visited on this path
// Also stores pointers to child nodes and whether they have been visited from this state

class node;

class nodeChild{

    public:
        /* CONSTRUCTORS */
        nodeChild( node* child );

        /* VARIABLES */
        node* pntr;     // Pointer to child node
        bool check;     // Node visit check

        /* METHODS */
        void mark();    // Mark this child as visited
        void unmark();  // Un-mark this child as visited
};

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

// Prints the closed cycles of a given set of nodes to file
int printPathsToFile( node nodeList[], int listSize, ofstream& aFile );

#endif // NODE_H
