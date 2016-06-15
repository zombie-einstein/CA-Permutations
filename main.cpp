//#include <iostream>
#include "classes.h"    // All the classes are defined here
#include <sstream>
#include <string.h>
using namespace std;

void printClass(){

    ruleset ruleA;

    permutation permList[8];

    for ( int n = 0; n < 256; n++ ){

        ruleA.loadRules(n);

        for ( int i = 0; i < 8; i++ ){
            permList[i].setValue(i);
            permList[i].setUpdates( &ruleA );
        }

        transmissionMatrix testMatrix;

        for ( int i = 0; i < 8; i++ ){
            for ( int j = 0; j < 4; j++ ){
                testMatrix.setValue( permList[i].updates[j], i );
            }
        }

        testMatrix.normalize();

    }
}

void printMatrix( int r ){

        unsigned reps = 50;

        permutation permList[8];

        ruleset test(r);

        for ( int i = 0; i < 8; i++ ){
            permList[i].setValue(i);
            permList[i].setUpdates( &test );
            //permList[i].printNeighbours();
            permList[i].printUpdates();
        }

        transmissionMatrix testMatrix;

        for ( int i = 0; i < 8; i++ ){
            for ( int j = 0; j < 4; j++ ){
                testMatrix.setValue( permList[i].updates[j], i );
            }
        }
        cout << endl;

        testMatrix.normalize();
        testMatrix.print();

        testMatrix.printDegrees();

        transmissionMatrix powers[reps];

        powers[0] = testMatrix.multiply(&testMatrix);

        for ( int i = 1; i < reps; i++ ){
            powers[i] = powers[i-1].multiply(&testMatrix);
        }

        //for ( int i = 0; i < reps; i++ ){  powers[i].print(); }

        powers[reps-1].print();
        powers[reps-1].printDegrees();

}

void printMatrixToFile( int r, ofstream& aFile, ofstream& bFile ){

        unsigned reps = 50;

        aFile << "Rule: " << r << endl << endl;

        permutation permList[8];

        ruleset test(r);

        aFile << "Permutations" << endl;

        for ( int i = 0; i < 8; i++ ){
            permList[i].setValue(i);
            permList[i].setUpdates( &test );
            permList[i].printUpdatesToFile( aFile );
        }

        transmissionMatrix testMatrix;

        for ( int i = 0; i < 8; i++ ){
            for ( int j = 0; j < 4; j++ ){
                testMatrix.setValue( permList[i].updates[j], i );
            }
        }

        aFile << endl;

        testMatrix.normalize();
        testMatrix.printToFile( aFile );

        transmissionMatrix powers = testMatrix.toPower( reps );

        powers.printToFile( aFile );

        bFile << r << ":\t" << testMatrix.onesOnDiagonal() << "\t" << testMatrix.onesOffDiagonal() << "\t" << powers.noZeros() << "\t" << powers.columnsMatch() << "\t" << powers.cellsMatch();

        if ( testMatrix.onesOnDiagonal() == 1 ){ bFile << "\t Class 1" << endl; }
        else if ( powers.cellsMatch() ){ bFile << "\t Class 3" << endl; }
        else if ( powers.noZeros() && powers.columnsMatch() ){ bFile << "\t Class 4" << endl; }
        else if ( !powers.noZeros() ){ bFile << "\t Class 2" << endl; }
        else{ bFile << endl; }

}

int main()
{

    //printMatrix(110);

    ofstream statFile;
    statFile.open( "data/stats.txt" );
    statFile << boolalpha;
    statFile << "Rule \t 1's Dgl \t 1's Off \t No Zeros \t Column \t All" << endl;

    for ( int i = 0; i< 256; i++ ){
        ofstream file;
        file.open ( "data/CA_Matrices"+to_string(i)+".txt" );
        printMatrixToFile( i, file, statFile );
        file.close();
    }

    statFile.close();


    return 0;
}
