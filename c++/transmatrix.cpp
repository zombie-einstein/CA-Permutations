#include "transmatrix.h"

/* CONSTRUCTORS */
// Constructor, sets matrix size and sets entries to zero
transMatrix::transMatrix( int dimensions ){

    N.resize(dimensions,dimensions);
    N.setZero();
    accessLists.resize(dimensions,vector<int>());
}

/* OPERATOR OVERLOADS */
// Matrix multiplication operator
transMatrix transMatrix::operator* ( transMatrix& foo ){

    transMatrix temp;
    temp.N = this->N * foo.N;
    return temp;
}

// Division by scalar operator
void transMatrix::operator/ ( float divisor){

    N /= divisor;
}

// Access values of matrix using bracket operator
float& transMatrix::operator() ( int i, int j ){

    return N(i,j);
}

// Matrix exponentiation
transMatrix transMatrix::operator^ ( int n ){

    Matrix<float,Dynamic,Dynamic> tempA;
    tempA.resize( N.rows(), N.cols() );
    tempA = N;

    for ( int i = 0; i < n; ++i ){
        tempA *= N;
    }

    transMatrix tempB;
    tempB.N = tempA;
    return tempB;
}

/* FUNCTION DEFINITIONS */
// Normalize the sum of row entries
void transMatrix::normalize(){

    N /= N.col(0).sum();
}

// Print the transmission matrix to console
void transMatrix::printMatToConsole(){

    cout << "Transmission matrix:" << endl;

    for ( int i = 0; i < N.cols(); ++i ){
        for (  int j = 0; j < N.rows()-1; ++j ){
            cout << "|" << N(j,i) << ",";
        }
        cout << N(N.rows()-1,i) << "|" << endl;
    }
    cout << endl;
}

// Print the transmission matrix to a file
void transMatrix::printMatToFile( ofstream& aFile ){

    aFile << "Transmission matrix:" << endl;

    for ( int i = 0; i < N.cols(); ++i ){
        for (  int j = 0; j < N.rows()-1; ++j ){
            aFile << N(j,i) << ",";
        }
        aFile << N(N.rows()-1,i) << endl;
    }
    aFile << endl;
}

// Print degree (P_in - P_out) of each state
void transMatrix::printDegrees(){

    for (  int i = 0; i < N.cols(); ++i ){
            cout << sumOfColumn(i)-1 << ",";
    }
    cout << endl << endl;
}

// Return the number of ones on the diagonal
int transMatrix::onesOnDiagonal(){

    int numOnes = 0;

    for ( int i = 0; i < N.cols(); ++i ){
        if ( N(i,i) == 1 ){ ++numOnes; }
    }

    return numOnes;
}

// Return ones off of the Diagonal
int transMatrix::onesOffDiagonal(){

    int numOnes = 0;
    for ( int i = 0; i < N.cols(); ++i ){
        for ( int j = 0; j < N.rows(); ++j ){
            if ( i != j && N(i,j) == 1 ){ ++numOnes; }
        }
    }
    return numOnes;
}

// Check if the matrix contains any zeros
bool transMatrix::noZeros(){

    return (N.array() > 0).all();
}

// Check if a columns entries match (within tolerance)
bool transMatrix::matchColumn( int n ){

    return ( abs(N.row(n).array() - N(n,0)) < 0.01 ).all();
}

// Check if all the columns of the matrix match
bool transMatrix::columnsMatch(){

    for ( int i = 0; i < N.cols(); i++ ){
        if ( !matchColumn(i) ){  return false; }
    }
    return true;
}

// Check if all the cells match
bool transMatrix::cellsMatch(){

    return ( abs(N.array() - N(0,0)) < 0.01 ).all();
}

// Print eigenvalues of transmission matrix to file
void transMatrix::printEigValToFile( ofstream& aFile ){

    EigenSolver<Matrix<float,8,8>> solver;
    solver.compute(N,false);
    aFile << "Eigenvalues: " << solver.eigenvalues().transpose() << endl << endl;
}

// Populate the access lists of each state
void transMatrix::getAccessLists(){

    // Push states accessible on first step from transmission matrix
    for ( int i = 0; i < N.rows(); ++i ){

        for( int j = 0; j < N.cols(); ++j ){ if ( N(j,i) > 0 ) accessLists[i].push_back( j ); }
    }

    // Iterators
    vector< vector<int> >::iterator sttIt;
    vector<int>::iterator itA, itB;

    // Loop over states
    for ( sttIt = accessLists.begin(); sttIt != accessLists.end(); ++sttIt ){

        int vecLength; // Store vector length as check

        do{
            vecLength = sttIt->size();

            vector<int> append;

            for ( itA = sttIt->begin(); itA != sttIt->end(); ++itA ){

                for ( itB = accessLists[*itA].begin(); itB != accessLists[*itA].end(); ++itB ){

                    append.push_back( *itB );
                }
            }
            sttIt->insert( sttIt->end(), append.begin(), append.end() );
            sort( sttIt->begin(), sttIt->end() );
            sttIt->erase( unique( sttIt->begin(), sttIt->end() ), sttIt->end() );
        }
        while( sttIt->size() != vecLength );
    }
}

// Print the communication classes of this transmission matrix
void transMatrix::printCommClasses(  ofstream& aFile ){

    if ( accessLists.front().empty() ) getAccessLists();

    aFile << "State accesibility:" << endl;

    vector< vector<int> >::iterator sttIt;
    vector<int>::iterator accIt;
    int count = 0;

    for ( sttIt = accessLists.begin(); sttIt != accessLists.end(); ++sttIt ){

        aFile << count++ << "->";

        for ( accIt = sttIt->begin(); accIt != sttIt->end(); ++accIt ){

            aFile << *accIt << ",";
        }
        aFile << endl;
    }
    aFile << endl;
}

// Print the closed cycles of thee transmission graph of this matrix
void transMatrix::printPaths( ofstream& aFile ){

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
    list< list<node*> > paths;

    list< list<node*> >::iterator listIt;
    list<node*>::iterator nodeIt;

    for ( int i = 0; i < 8; ++i ){

        // Push first node to the current stack
        nodeList[i].visited = true;
        list<node*> temp( 1, &nodeList[i] );
        paths.push_back( temp );

        // Iterator to current place on lists an stack
        listIt = paths.end();
        --listIt;


        int stackSize;      // Store stack size at start of iteration
        node* currentNode;  // Pointer to the current node being checked

        do{
            stackSize = listIt->size();     // Store current stack size to compare against
            currentNode = listIt->back();   // Point to the last node on the current stack

            // Iterate over current nodes children
            for ( nodeIt = currentNode->children.begin(); nodeIt !=  currentNode->children.end(); ++ nodeIt ){

                // Check if this state has been visited on the tree or by this state
                if ( !((*nodeIt)->visited) && !(currentNode->localVisit[(*nodeIt)->value]) ){
                    (*nodeIt)->visited = true;                              // Mark child as visited globally
                    currentNode->localVisit[(*nodeIt)->value] = true;       // Mark child as visited by this state
                    listIt->push_back( *nodeIt );                           // Push Child to stack
                    break;                                                  // Leave loop to go back to current end of stack
                }
                // Check if the state is globally visited (i.e. a repetition) but not from this state
                if ( (*nodeIt)->visited && !(currentNode->localVisit[(*nodeIt)->value]) ){
                    currentNode->localVisit[(*nodeIt)->value] = true;  // Mark child as visited locally
                    paths.insert( listIt, *listIt );                        // Duplicate the current stack (i.e. save the cycle)
                    (--listIt)->push_back( *nodeIt );                       // Push the child to the previous stack (to display the repeated node value)
                    ++listIt;                                               // Reset iterator to current position
                }
                // Is this is the last child to be checked
                if ( next(nodeIt) == currentNode->children.end() ){
                    currentNode->visited = false;                      // Mark the current node as unvisited
                    currentNode->clearVisits();                        // Mark all it's children as unvisited
                    listIt->pop_back();                                     // Remove the current node from the stack
                    break;                                                  // Break out of the loop to avoid using iterator nodeIt again
                }
            }
        }
        while( listIt->size() != stackSize && listIt->size() > 0 );         // Terminate if the current stack size is unchanged, but also avoid completely deleting it

        nodeList[i].visited = false;
        paths.pop_back();
    }

    // Clean up paths that have excess nodes at start
    for ( listIt = paths.begin(); listIt != paths.end(); ++listIt ){

        if ( listIt->begin() == listIt->end() ){ continue; }

        list<node*>::iterator penIt = listIt->end();
        --penIt;
        for ( nodeIt = listIt->begin(); nodeIt != penIt; ++nodeIt ){
            if ( *nodeIt == listIt->back()  ){
                listIt->erase( listIt->begin(), nodeIt );
                break;
            }
        }
    }

    // Rotate cycle until lowest value first (for comparison)
    list<node*>::iterator minIt;
    for ( listIt = paths.begin(); listIt != paths.end(); ++listIt ){
        minIt = listIt->begin();
        for ( nodeIt = listIt->begin(); nodeIt != listIt->end(); ++nodeIt ){
            if ( (*nodeIt)->value < (*minIt)->value ){ minIt = nodeIt; }
        }
        if ( minIt == listIt->begin() ){ continue; }

        listIt->pop_back();
        for ( nodeIt = listIt->begin(); nodeIt != minIt; ++nodeIt ){
            listIt->splice( listIt->end(), *listIt, nodeIt );
        }
        listIt->push_back( *(listIt->begin()) );
    }

    // Remove and repeated cycles from the list
    paths.sort();
    paths.unique();

    // Print the node relationship list
    aFile << "Nodes:" << endl;
    for ( int i = 0; i < 8; ++i ){ nodeList[i].printNode(aFile); }
    aFile << endl;

    // Print the cycles and their state representation
    aFile << "Cycles: "<< endl;
    for ( listIt = paths.begin(); listIt != paths.end(); ++listIt ){
        for ( nodeIt = listIt->begin(); nodeIt != listIt->end(); ++nodeIt ){
            if ( next(nodeIt) == listIt->end() ){ aFile << (*nodeIt)->value; }
            else{ aFile << (*nodeIt)->value << "->"; }
        }
        aFile << endl;

        list<node*>::reverse_iterator disIt;
        for ( disIt = listIt->rbegin(); disIt != listIt->rend(); ++disIt ){
            for ( int i = 2; i > -1; --i ){
                if ( ( (*disIt)->value >> i ) % 2 == 0 ){ aFile << "\u2591"; }
                else{ aFile << "\u2588"; }
            }
            aFile << endl;
        }
    }
    aFile << endl << "Number of cycles: " << paths.size() << endl;


}
