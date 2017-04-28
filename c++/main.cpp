#include <iostream>

#include "transmatrix.h"
#include "classes.h"    // All the classes are defined here

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

        transMatrix testMatrix;

        for ( int i = 0; i < 8; i++ ){
            for ( int j = 0; j < 4; j++ ){
                testMatrix( permList[i].updates[j], i ) += 1;
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

        transMatrix testMatrix;

        for ( int i = 0; i < 8; i++ ){
            for ( int j = 0; j < 4; j++ ){
                testMatrix(permList[i].updates[j],i) += 1;
            }
        }
        cout << endl;

        testMatrix.normalize();
        testMatrix.printMatToConsole();

        testMatrix.printDegrees();

        transMatrix powers[reps];

        powers[0] = testMatrix * testMatrix;

        for ( int i = 1; i < reps; i++ ){
            powers[i] = powers[i-1] * testMatrix;
        }

        powers[reps-1].printMatToConsole();
        powers[reps-1].printDegrees();

}

int printMatrixToFile( int r, ofstream& aFile, ofstream& bFile ){

        int reps = 50;

        aFile << "Rule: " << r << endl << endl;

        permutation permList[8];

        ruleset test(r);

        aFile << "Permutations" << endl;

        for ( int i = 0; i < 8; i++ ){
            permList[i].setValue(i);
            permList[i].setUpdates( &test );
            permList[i].printUpdatesToFile( aFile );
        }

        transMatrix testMatrix;

        for ( int i = 0; i < 8; i++ ){
            for ( int j = 0; j < 4; j++ ){
                testMatrix( permList[i].updates[j], i ) += 1;
            }
        }

        aFile << endl;

        testMatrix.normalize();
        testMatrix.printMatToFile( aFile );
        //testMatrix.printEigenValues( aFile );

        testMatrix.printCommClasses( aFile );
        aFile << endl;

        testMatrix.printPaths( aFile );

        transMatrix powers = testMatrix^reps;

        //powers.printMatToFile( aFile );

        bFile << r << ":\t" << testMatrix.onesOnDiagonal() << "\t" << testMatrix.onesOffDiagonal() << "\t" << powers.noZeros() << "\t" << powers.columnsMatch() << "\t" << powers.cellsMatch() << "\t" << powers.onesOnDiagonal()+powers.onesOffDiagonal();

        if ( testMatrix.onesOnDiagonal() == 1 ){ bFile << "\t Class 1" << endl; return 0; }
        else if ( powers.cellsMatch() ){ bFile << "\t Class 3" << endl; return 2; }
        else if ( powers.noZeros() && powers.columnsMatch() ){ bFile << "\t Class 4" << endl; return 3; }
        else if ( !powers.noZeros() ){ bFile << "\t Class 2" << endl; return 1; }
        else{ bFile << endl; return -1; }

}

int main(){

    ofstream statFile;
    statFile.open( "data/stats.txt" );
    statFile << boolalpha;
    statFile << "Rule \t 1's Dgl \t 1's Off \t No Zeros \t Column \t All" << endl;

    vector<int> classes[4];

    for ( int i = 0; i< 256; i++ ){
        ofstream file;
        file.open ( "data/CA_Matrices"+to_string(i)+".txt" );
        classes[printMatrixToFile( i, file, statFile )].push_back(i);
        file.close();
    }

    statFile.close();

    ofstream classFile;
    classFile.open( "data/classes.txt" );
    classFile << "******* CA Classifications *******" << endl;
    classFile << "Class 1: " << classes[0].size();
    classFile << " Class 2: " << classes[1].size();
    classFile << " Class 3: " << classes[2].size();
    classFile << " Class 4: " << classes[3].size() << endl;

    for ( int i = 0; i < 4; i++ ){
        classFile << "Class " << i+1 << ":" << endl;
        for ( vector<int>::iterator it = classes[i].begin(); it != classes[i].end(); ++it ){
            classFile << *it << endl;
        }
        classFile << endl;
    }

    return 0;
}
