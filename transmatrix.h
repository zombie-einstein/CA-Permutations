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
        transMatrix( int dimensions = 8 );

        /* CONTAINERS */
        // Matrix of transmission probabilities
        Matrix<float,Dynamic,Dynamic> N;

        // Vector to contain communicating class
        vector<int> commClasses;

        /* OPERATOR OVERLOADS */
        // Matrix multiplication
        transMatrix operator*( transMatrix foo );
        // Matrix entry access
        float& operator() (int i, int j);

        /* FUNCTIONS DEFINITIONS */
        // +1 to a entry of the matrix
        void setValue( int x, int y ){ N(x,y) += 1; }

        // Normalize the entries of the matrix (for four outputs per permutation)
        void normalize(){ N /= 4; }

        // Print the matrix
        void print(){
            cout << "Transmission matrix:" << endl;
            for ( int i = 0; i < 8; i++ ){
                for (  int j = 0; j < 7; j++ ){
                    cout << N(j,i) << ",";
                }
                cout << N(7,i) << endl;
            }
            cout << endl;
        }

        // Print the matrix to a file
        void printToFile( ofstream& aFile ){
            aFile << "Transmission matrix:" << endl;
            for ( int i = 0; i < 8; i++ ){
                for (  int j = 0; j < 7; j++ ){
                    aFile << N(j,i) << ",";
                }
                aFile << N(7,i) << endl;
            }
            aFile << endl;
        }

        // Return the result of multiplication of the this and an object matrix
        transMatrix multiply( transMatrix * object ){
            transMatrix temp;
            temp.N = N;
            temp.N *= object->N;
            return temp;
        }

        // Return transmission matrix raised to power
        transMatrix toPower( int n ){
            transMatrix temp;
            temp.N = N;
            for ( int i = 0; i < n; i++ ){ temp.N *= N; }
            return temp;
        }

        // Return sum of values of column
        float sumOfColumn( int n ){ return N.row(n).sum(); }

        // Print degree (P-in - P-out) of each permutation
        void printDegrees(){
            for (  int i = 0; i < 8; i++ ){ cout << sumOfColumn(i)-1 << ","; }
            cout << endl << endl;
        }

        // Return number of 1's on diagonal
        int onesOnDiagonal(){
            int numOnes = 0;
            for ( int i = 0; i < 8; i++ ){
                    if ( N(i,i) == 1 ){ numOnes++; }
            }
            return numOnes;
        }

        // Return number of ones off the diagonal
        int onesOffDiagonal(){
            int numOnes = 0;
            for ( int i = 0; i < 8; i++ ){
                for ( int j = 0; j < 8; j++ ){
                    if ( i != j && N(i,j) == 1 ){ numOnes++; }
               }
            }
            return numOnes;
        }

        // Checks of the matrix contains any zeros
        bool noZeros(){
            if ( (N.array() > 0).all() ){ return true; }
            return false;
        }

        // Compare if column elements match
        bool matchColumn( int n ){
            if ( ( abs(N.row(n).array() - N(n,0)) < 0.01 ).all() ){ return true; }
            return false;
        }

        // Check if all column matches
        bool columnsMatch(){
            for ( int i = 0; i < 8; i++ ){
                if ( !matchColumn(i) ){  return false; }
            }
            return true;
        }

        // Compare if all cells match
        bool cellsMatch(){
            if ( ( abs(N.array() - N(0,0)) < 0.01 ).all() ){ return true; }
            return false;
        }

        // Print the eigenvalues of this matrix
        void printEigenValues( ofstream& aFile ){
            EigenSolver<Matrix<float,8,8>> solver;
            solver.compute(N,false);
            aFile << "Eigenvalues: " << solver.eigenvalues().transpose() << endl << endl;
        }

        // Find the communicating classes of this transmission matrix
        void printCommClasses( ofstream& aFile ){

            vector< vector<int> > accessLists;  // Stores vectors of access lists from each state

            for ( int i = 0; i < 8; ++i ){
                vector<int> temp;   // Temp access list for this state
                // Get states accessible on first step
                for( int j = 0; j < 8; ++j ){ if ( N(j,i) > 0 ){ temp.push_back( j ); } }


                int accessLength;   // Save the length of this vector for comparison

                do{
                    accessLength = temp.size();   // update current length of temp
                    // Loop over accessible states and push their accessible states to temp
                    for( int j = 0; j < accessLength; ++j ){
                        for( int k = 0; k < 8; ++k ){
                            if ( N(k,temp[j]) > 0 ){ temp.push_back( k ); }
                        }
                    }
                    // Sort and remove any duplicates
                    sort( temp.begin(), temp.end() );
                    temp.erase( unique( temp.begin(), temp.end() ), temp.end() );
                }
                // Continue if the length of temp has changed (i.e new access members have been added)
                while( temp.size() != accessLength );
                // Add this states access list to
                accessLists.push_back( temp );
            }

            aFile << "State accesibility:" << endl;

            for ( int i = 0; i < 8; ++i ){
                aFile << i << "--> ";
                vector<int>::iterator it;
                for ( it = accessLists[i].begin(); it != accessLists[i].end()-1; ++it ){
                    aFile << *it << ",";
                }
                it = accessLists[i].end()-1;
                aFile << *it << endl;
            }
            aFile << endl;
        }

        // Print the paths of this markov chain
        void printPaths( ofstream& aFile ){

            // Node class required for path search
            class node{
                public:
                    int value;
                    vector<node*> children;
                    bool visited = false;
                    bool localVisit[8] = {false,false,false,false,false,false,false,false};
                    void addChild( node* child ){ children.push_back( child ); }
                    void clearVisits(){ for( int i = 0; i < 8; ++i ){ localVisit[i] = false; } }
                    void printNode( ofstream& aFile ){
                        aFile << value << "-> ";
                        for ( vector<node*>::iterator it = children.begin(); it != children.end(); ++it ){
                            aFile << (*it)->value << ",";
                        }
                        aFile << endl;
                    }
            };

            // States as nodes
            node nodeList[8];

            // Populate nodes from
            for ( int i = 0; i < 8; ++i ){
                nodeList[i].value = i;
                for ( int j = 0; j < 8; ++j ){
                    if ( N(j,i) > 0 ){
                            nodeList[i].addChild( nodeList+j ); }
                }
            }

            // Store the paths
            list< vector<node*> > paths;
            // Push first node to vector
            nodeList[0].visited = true;
            vector<node*> temp = { nodeList };
            paths.push_back( temp );
            // Iterator to current place on list
            list< vector <node*> >::iterator listIt = paths.begin();
            vector<node*>::iterator nodeIt;

            int stackSize;

            do{
                stackSize = listIt->size(); // Store current stack size to compare against

                // Iterate over current states children
                for ( nodeIt = (listIt->back())->children.begin(); nodeIt !=  (listIt->back())->children.end(); ++ nodeIt ){

                    // Check if this state has been visited on the tree or by this state
                    if ( !((*nodeIt)->visited) && !((listIt->back())->localVisit[(*nodeIt)->value]) ){
                        (*nodeIt)->visited = true;                              // Mark child as visited globally
                        (listIt->back())->localVisit[(*nodeIt)->value] = true;  // Mark child as visited by this state
                        listIt->push_back( *nodeIt );                           // Push Child to stack
                        break;                                                  // Leave loop to go back to current end of stack
                    }
                    // Check if the state is globally visited (i.e. a repetition) but not from this state
                    if ( (*nodeIt)->visited && !((listIt->back())->localVisit[(*nodeIt)->value]) ){
                        (listIt->back())->localVisit[(*nodeIt)->value] = true;  // Mark child as visited locally
                        paths.insert( listIt, *listIt );                        // Duplicate the current stack (i.e. save the cycle)
                        (--listIt)->push_back( *nodeIt );                       // Push the child to the previous stack (to display loop value)
                        ++listIt;                                               // Reset iterator to current position
                    }
                    // Is this is the last child to be checked
                    if ( next(nodeIt) == (listIt->back())->children.end() ){
                        (listIt->back())->visited = false;                      // Mark the current node as unvisited
                        (listIt->back())->clearVisits();                        // Mark all it's children as unvisited
                        listIt->pop_back();                                     // Remove the current node from the stack
                        break;                                                  // Break out of the loop to avoid using iterator nodeIt again
                    }
                }
            }
            while( listIt->size() != stackSize && listIt->size() > 0 );         // Terminate if the current stack size is unchanged, but also avoid completely deleting it

            // Print the node relationship list
            aFile << "Nodes:" << endl;
            for ( int i = 0; i < 8; ++i ){ nodeList[i].printNode(aFile); }
            aFile << endl;
            // Print the cycles
            aFile << "Cycles from 0: "<< endl;
            for ( listIt = paths.begin(); listIt != paths.end(); ++listIt ){
                for ( nodeIt = listIt->begin(); nodeIt != listIt->end(); ++nodeIt ){
                    if ( next(nodeIt) == listIt->end() ){ aFile << (*nodeIt)->value; }
                    else{ aFile << (*nodeIt)->value << "->"; }
                }
                aFile << endl;
            }
            aFile << endl;


        }
};

#endif // TRANSMATRIX_H
