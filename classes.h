#ifndef CLASSES_H_INCLUDED
#define CLASSES_H_INCLUDED

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

};

// Transmission matrix class
class transmissionMatrix{

    public:
        // Store values
        float M[8][8]={};

        // +1 to a entry of the matrix
        void setValue( int x, int y ){ M[x][y] += 1; }

        // Normalize the entries of the matrix (for four outputs per permutation)
        void normalize(){
            for ( int i = 0; i < 8; i++ ){
                for (  int j = 0; j < 8; j++ ){
                    M[i][j] /= 4;
            }
        }}

        // Print the matrix
        void print(){
            cout << "Transmission matrix:" << endl;
            for ( int i = 0; i < 8; i++ ){
                for (  int j = 0; j < 7; j++ ){
                    cout << M[j][i] << ",";
                }
                cout << M[7][i] << endl;
            }
            cout << endl;
        }

        // Return the result of multiplication of the this and an object matrix
        transmissionMatrix multiply( transmissionMatrix * object ){
            transmissionMatrix temp;
            for( int i = 0; i < 8; i++ ){
                for ( int j = 0; j < 8; j++ ){
                    float sum = 0;
                    for ( int k = 0; k < 8; k++ ){
                        sum += M[k][i]*(object->M[j][k]);
                    }
                    temp.M[j][i] = sum;
                }
            }
            return temp;
        }

        float sumOfRow( int n ){
            float sum = 0;
            for ( int i = 0; i < 8; i++ ){ sum += M[i][n]; }
            return sum;
        }

        float sumOfColumn( int n ){
            float sum = 0;
            for ( int i = 0; i < 8; i++ ){ sum += M[n][i]; }
            return sum;
        }

        // Print degree (P-in - P-out) of each permutation
        void printDegrees(){
            for (  int i = 0; i < 8; i++ ){
                cout << sumOfColumn(i)-sumOfRow(i) << ",";
            }
            cout << endl << endl;
        }
};

#endif // CLASSES_H_INCLUDED
