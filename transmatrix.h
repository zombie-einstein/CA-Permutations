#ifndef TRANSMATRIX_H
#define TRANSMATRIX_H

#include "node.h"

// Eigen matrix headers
#include <Core>
#include <Dense>
#include <Eigenvalues>

// Name-spaces
using namespace Eigen;
using namespace std;

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

        /* METHOD DECLERATIONS */
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
        int printPaths( ofstream& aFile );
};

#endif // TRANSMATRIX_H
