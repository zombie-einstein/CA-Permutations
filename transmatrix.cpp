#include "transmatrix.h"

/* ===== TRANSITION MATRI METHOD IMPLEMENTATIONS ===== */

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
int transMatrix::printPaths( ofstream& aFile ){

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

    int counter =  printPathsToFile( nodeList, 8, aFile );

    return 0;
}
