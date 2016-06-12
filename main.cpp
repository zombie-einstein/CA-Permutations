#include <iostream>

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

        void setUpdates( ruleset * r ){
            updates[0] = (r->applyRule(l1)<<2) + (r->applyRule(n)<<1) + r->applyRule(r1);
            updates[1] = (r->applyRule(l1)<<2) + (r->applyRule(n)<<1) + r->applyRule(r0);
            updates[2] = (r->applyRule(l0)<<2) + (r->applyRule(n)<<1) + r->applyRule(r1);
            updates[3] = (r->applyRule(l0)<<2) + (r->applyRule(n)<<1) + r->applyRule(r0);
        }

        void printNeighbours(){
            cout << "l1(" << l1 << "),l0(" << l0 << ")= " << n << " =r1(" << r1 << "),r0(" << r0 << ")" << endl;
        }

        void printUpdates(){
            cout << updates[0] << "," << updates[1] << "," << updates[2] << "," << updates[3] << endl;
        }

};

// Transmission matrix class
class transmissionMatrix{

    public:
        unsigned int M[8][8]={};

        void setValue( int x, int y ){ M[x][y] += 1; }

        void print(){
            cout << "Transmission matrix:" << endl;
            for ( int i = 0; i < 8; i++ ){
                for (  int j = 0; j < 7; j++ ){
                    cout << M[j][i] << ",";
                }
                cout << M[7][i] << endl;
            }
        }

        void multiply( int n ){
            transmissionMatrix temp;
            for( int i = 0; i < 8; i++ ){
                for ( int j = 0; j < 8; j++ ){
                    //temp.M[i][j] = M[]
                }
            }
        }
};

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

    testMatrix.print();

    return 0;
}
