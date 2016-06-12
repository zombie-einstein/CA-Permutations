#include <iostream>
#include "classes.h"    // All the classes are defined here


int main()
{
    unsigned int r;

    cout << "Enter rule number:";

    cin >> r;

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

    transmissionMatrix powers[3];

    powers[0] = testMatrix.multiply(&testMatrix);

    for ( int i = 1; i < 3; i++ ){
        powers[i] = powers[i-1].multiply(&testMatrix);
    }

    for ( int i = 0; i < 3; i++ ){  powers[i].print(); }


    return 0;
}
