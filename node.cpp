#include "node.h"

/* ===== NODECHILD FUNTION DEFINITIONS ===== */
// Initialize with node pointer & set visited to false
nodeChild::nodeChild( node* child ){

    pntr  = child;
    check = false;
}

// Mark this child as visited
void nodeChild::mark(){ check = true; }

// Un-mark child as visited
void nodeChild::unmark(){ check = false; }

/* ===== NODE FUNCTION DEFINTIONS ===== */
// Default constructor
node::node(){}

// Constructor with value
node::node( int n ){ value = n; }

// Add a child to this node
void node::addChild( nodeChild newChild ){

    children.push_back( newChild );
}

// Un-mark all the children as visited
void node::unmarkChildren(){

    for ( list<nodeChild>::iterator chIt = children.begin(); chIt != children.end(); ++chIt ){ chIt->unmark(); }
}

// Print the node and it's children to file
void node::printNodeToFile( ofstream& aFile ){

    aFile << value << "-> ";

    for ( list<nodeChild>::iterator chIt = children.begin(); chIt != children.end(); ++chIt ){
        aFile << chIt->pntr->value << ",";
    }
    aFile << endl;
}

/* ===== FUNCTION DEFINITIONS ===== */

int printPathsToFile( node nodeList[], int listSize, ofstream& aFile ){

    // Store the paths
    list< list<node*> > paths;

    list< list<node*> >::iterator listIt;
    list<node*>::iterator nodeIt;
    list<nodeChild>::iterator chldIt;

    for ( int i = 0; i < listSize; ++i ){

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
            stackSize   = listIt->size();     // Store current stack size to compare against
            currentNode = listIt->back();   // Point to the last node on the current stack

            // Iterate over current nodes children
            for ( chldIt = currentNode->children.begin(); chldIt !=  currentNode->children.end(); ++chldIt ){

                // Check if this state has been visited on the tree or by this state
                if ( !(chldIt->pntr->visited) && !(chldIt->check) ){
                    chldIt->pntr->visited = true;                           // Mark child as visited globally
                    chldIt->mark();                                         // Mark child as visited by this state
                    listIt->push_back( chldIt->pntr );                      // Push Child to stack
                    break;                                                  // Leave loop to go back to current end of stack
                }
                // Check if the state is globally visited (i.e. a repetition) but not from this state
                if ( chldIt->pntr->visited && !(chldIt->check) ){
                    chldIt->mark();                                         // Mark child as visited locally
                    paths.insert( listIt, *listIt );                        // Duplicate the current stack (i.e. save the cycle)
                    (--listIt)->push_back( chldIt->pntr );                  // Push the child to the previous stack (to display the repeated node value)
                    ++listIt;                                               // Reset iterator to current position
                }
                // Is this is the last child to be checked
                if ( next(chldIt) == currentNode->children.end() ){
                    currentNode->visited = false;                           // Mark the current node as unvisited
                    currentNode->unmarkChildren();                          // Mark all it's children as unvisited
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
    for ( int i = 0; i < listSize; ++i ){ nodeList[i].printNodeToFile(aFile); }
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

    return paths.size();
}
