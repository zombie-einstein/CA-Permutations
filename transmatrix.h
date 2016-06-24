#ifndef TRANSMATRIX_H
#define TRANSMATRIX_H

// File and console streams
#include <iostream>
#include <fstream>

// Eigen matrix headers
#include <Core>
#include <Dense>
#include <Eigenvalues>

// STD Containers
#include <vector>
#include <list>
#include <array>

// Name-spaces
using namespace Eigen;
using namespace std;

/* ===== NODE GRAPH CLASS USED TO FIND PATHS OF TRANSMISSION GRAPH ===== */
// Stores the nodes value and whether it has been visited on this path
// Also stores pointers to child nodes and whether they have been visited from this state
class node{

    public:

        /* CONTAINERS */
        // Nodes value
        int value;

        // References to children (i.e. on accessible from this state) of this node
        list<node*> children;

        // Check if this node has been visited
        bool visited = false;

        // Store whether children have been visited from this state
        array<bool,8> localVisit = {{false,false,false,false,false,false,false,false}};

        /* METHODS */
        // Add a child reference to this node
        void addChild( node* child ){ children.push_back( child ); }

        // Mark all the children of this state as unvisited
        void clearVisits(){ for( int i = 0; i < 8; ++i ){ localVisit[i] = false; } }

        // Print the value and children of this node
        void printNode( ofstream& aFile ){
            aFile << value << "-> ";
            for ( list<node*>::iterator it = children.begin(); it != children.end(); ++it ){
                aFile << (*it)->value << ",";
            }
            aFile << endl;
        }
};

/* ====== TRANSMISSION MATRIX CLASS ====== */
// Markov chain transmission matrix class, with several useful methods
// Required Eigen (http://eigen.tuxfamily.org/) headers to be included or libraries linked


class transMatrix{

    public:
        /* CONSTRUCTOR */
        // Default constructor sets all entries to zero
        // and resizes containers appropriately
        transMatrix( int dimensions = 8 );

        /* CONTAINERS */
        // Matrix of transmission probabilities
        Matrix<float,Dynamic,Dynamic> N;

        // Stores vectors of access lists from each state
        vector< vector<int> > accessLists;

        // Vector to contain communicating class
        vector<int> commClasses;

        /* OPERATOR OVERLOADS */
        // Matrix multiplication
        transMatrix operator* ( transMatrix& foo );
        // Division by scalar
        void operator/ ( float divisor );
        // Matrix entry access
        float& operator() (int i, int j);
        // Matrix exponentiation
        transMatrix operator^ ( int n );

        /* FUNCTIONS DEFINITIONS */
        // Normalize the sum of row entries
        void normalize();

        // Print the transmission matrix to console
        void printMatToConsole();

        // Print the matrix to a file
        void printMatToFile( ofstream& aFile );

        // Return sum of values of column
        float sumOfColumn( int n ){ return N.row(n).sum(); }

        // Print degree (P-in - P-out) of each state
        void printDegrees();

        // Return number of 1's on diagonal
        int onesOnDiagonal();

        // Return number of ones off the diagonal
        int onesOffDiagonal();

        // Checks of the matrix contains any zeros
        bool noZeros();

        // Compare if column elements match
        bool matchColumn( int n );

        // Check if all column matches
        bool columnsMatch();

        // Compare if all cells match
        bool cellsMatch();

        // Print the eigenvalues of this matrix
        void printEigValToFile( ofstream& aFile );

        // Populate the access lists
        void getAccessLists();

        // Find the communicating classes of this transmission matrix
        void printCommClasses( ofstream& aFile );

        // Print the closed cycle paths of this markov chain
        void printPaths( ofstream& aFile );
};

#endif // TRANSMATRIX_H
