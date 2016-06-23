#include "transmatrix.h"

transMatrix::transMatrix( int dimensions ){

    N.resize(dimensions,dimensions);
    N.setZero();
}

transMatrix transMatrix::operator*( transMatrix foo ){

    transMatrix temp;
    temp.N = this->N * foo.N;
    return temp;
}

float& transMatrix::operator() ( int i, int j ){

    return N(i,j);
}
