#ifndef CLASSES_H_INCLUDED
#define CLASSES_H_INCLUDED

#include <iostream>
#include <fstream>
#include <math.h>
#include <Core>
#include <Dense>
#include <Eigenvalues>
#include <vector>
#include <list>

using namespace Eigen;
using namespace std;

// Class that contains cellular automata ruleset, numbered according to Wolfram system (base 2)
class ruleset{

    public:

        unsigned int value, n[8];

        ruleset(){}

        ruleset( unsigned int x ){ loadRules(x); }

        // Load appropriate states into array for a given int
        void loadRules( unsigned int x ){
            value = x;
            for ( int i = 0; i < 8; i++ ){
                n[i] = (( x >> i ) & 1); // Sets value from digits of base 2 representation of x
            }
        }

        // Print this ruleset
        void print(){
            cout << "Ruleset " << value << endl;
            for ( int i = 0; i < 8; i++ ){ cout << i << "(" << n[i] << ")"; }
            cout << endl;
        }

        // return value of state for given integer
        unsigned int applyRule( unsigned int x ){ return n[x]; }
};

// Class for a permutation of states (3 wide, 2 states)
// Also generates possible neighbouring left and right shifted permutations
class permutation{

    public:

        unsigned int n, r1, r0, l1, l0, updates[4];

        permutation(){};

        permutation( unsigned int x ){ setValue(x); }

        void setValue( unsigned int x ){
            n = x;                          // Number of this permutation (base 2)
            r1 = (( n << 1 ) | 8) -8 +1;    // Right-hand neighbour appended with 1
            r0 = (( n << 1 ) | 8) -8;       // Right-hand neighbour appended with 0
            l1 = ( n >> 1 ) + 4;            // Left-hand neighbour appended with 1
            l0 = ( n >> 1 );                // Left-hand neighbour appended with 0
        }

        // Generate the possible update states from a given ruleset
        void setUpdates( ruleset * r ){
            updates[0] = (r->applyRule(l1)<<2) + (r->applyRule(n)<<1) + r->applyRule(r1);
            updates[1] = (r->applyRule(l1)<<2) + (r->applyRule(n)<<1) + r->applyRule(r0);
            updates[2] = (r->applyRule(l0)<<2) + (r->applyRule(n)<<1) + r->applyRule(r1);
            updates[3] = (r->applyRule(l0)<<2) + (r->applyRule(n)<<1) + r->applyRule(r0);
        }

        // Print the possible neighbour permutations
        void printNeighbours(){
            cout << "l1(" << l1 << "),l0(" << l0 << ")= " << n << " =r1(" << r1 << "),r0(" << r0 << ")" << endl;
        }

        // Print the possible update permutations
        void printUpdates(){
            cout << updates[0] << "," << updates[1] << "," << updates[2] << "," << updates[3] << endl;
        }

        // Print the possible update permutations to a text file
        void printUpdatesToFile( ofstream& aFile ){
            aFile << updates[0] << "," << updates[1] << "," << updates[2] << "," << updates[3] << endl;
        }

};

// Transmission matrix class
class transMatrix{

    public:
        // Default constructor sets matrix to zero
        transMatrix(){  N.setZero(); }

        // Matrix of transmission probabilities
        Matrix<float,8,8> N;

        // Vector to contain communicating class
        vector<int> commClasses;

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

#endif // CLASSES_H_INCLUDED
