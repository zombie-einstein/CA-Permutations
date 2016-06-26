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

// Mark this child as visited (globally)
void nodeChild::markVisited(){

    pntr->visited = true;
}

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

    aFile << value << " -> ";

    for ( list<nodeChild>::iterator chIt = children.begin(); chIt != children.end(); ++chIt ){
        aFile << chIt->pntr->value << ",";
    }
    aFile << endl;
}

/* ===== CYCLE LIST METHODS ===== */
// Add a node to the node list as node child
void cycleList::addNode( node* newNode ){

    nodeList.push_back( nodeChild( newNode ) );
}

// Add cycles from specific node to cycle list
void cycleList::cyclesFromNode( childIt start ){

    // Mark start node as visited locally and on list
    start->mark();
    start->markVisited();

    // Push start node to new stack then to list
    list<node*> temp(1, start->pntr );
    stackList.push_back( temp );

    // Iterator to current stack and list positions
    listIt currentStackIt = --stackList.end();

    int stackSize;      // Store the current size of the stack to later compare against
    node* currentNode;  // Pointer to final node on stack

    do{
        stackSize   = currentStackIt->size();
        currentNode = currentStackIt->back();

        // Iterate over the current nodes children
        for ( childIt it = currentNode->children.begin(); it != currentNode->children.end(); ++it ){

                // If the child node has not been visited on this path, or previously from this node
                // then mark it visited and push it to the stack, then move on to stack
                if ( !it->pntr->visited && !it->check ){

                    it->markVisited();
                    it->mark();
                    currentStackIt->push_back( it->pntr );
                    break;
                }

                // If the child node has been previously visited, but not from this state, it forms a cycle
                // so copy it and start on a new stack
                if ( it->pntr->visited && !it->check ){

                    it->mark();
                    stackList.insert( currentStackIt, *currentStackIt );
                    (--currentStackIt)->push_back( it->pntr );
                    ++currentStackIt;
                }

                // If this is the last child on the list, then remove the node from the list
                // and mark it as unvisited and it's children as unvisited from this node
                if ( next(it) == currentNode->children.end() ){

                    currentNode->visited = false;
                    currentNode->unmarkChildren();
                    currentStackIt->pop_back();
                    break;
                }
        }
    }
    while( currentStackIt->size() != stackSize && currentStackIt->size() > 0 );

}

// Remove any nodes not inside cycle from the front of a path
void cycleList::cleanCycles(){

    // Loop over the list of stacks
    for ( listIt it = stackList.begin(); it!= stackList.end(); ++it ){

        if ( it->begin() == it->end() ){ continue; } // If the start and end nodes match then continue

        stckIt penIt = --(it->end());   // Iterator pointing to penultimate node

        // Check nodes before penultimate for a match
        for ( stckIt jt = it->begin(); jt != penIt; ++jt ){

            if ( *jt == it->back() ){

                it->erase( it->begin(), jt );  // Delete nodes before the match
                break;
            }
        }
    }
}

// Re-order the cycles so that the lowest node value appears first
void cycleList::re_orderCycles(){

    stckIt minIt;   // Iterator pointing to minimum node

    // Loop over the cycle list
    for ( listIt it = stackList.begin(); it != stackList.end(); ++it ){

        minIt = it->begin();    // First place min at start of cycle

        for ( stckIt jt = it->begin(); jt != it->end(); ++jt ){
            if ( (*jt)->value < (*minIt)->value ) minIt = jt;
        }

        if ( minIt == it->begin() ) continue;

        it->pop_back();

        for ( stckIt jt = it->begin(); jt != minIt; ++jt ){
            it->splice( it->end(), *it, jt );
        }

        it->push_back( *(it->begin()) );
    }
}

// Remove any repeated cycles from the list
void cycleList::removeRepeated(){

    stackList.sort();
    stackList.unique();
}

// Print the cycles to a file
void cycleList::printCyclesToFile( ofstream& aFile ){

    for ( listIt it = stackList.begin(); it != stackList.end(); ++it ){
        for ( stckIt jt = it->begin(); jt != it->end(); ++jt ){
            if ( next(jt) == it->end() ) aFile << (*jt)->value;
            else{ aFile << (*jt)->value << "->"; }
        }
        aFile << endl;
    }
}

/* ===== FUNCTION DEFINITIONS ===== */
int printPathsToFile( node nodeList[], int listSize, ofstream& aFile ){

    cycleList newGraph;

    for ( int i = 0; i < listSize; ++i ){

        newGraph.addNode( nodeList+i );
    }

    aFile << "Node Children:" << endl;

    for ( list<nodeChild>::iterator it = newGraph.nodeList.begin(); it != newGraph.nodeList.end(); ++it ){

        it->pntr->printNodeToFile( aFile );
        newGraph.cyclesFromNode( it );

    }

    newGraph.cleanCycles();
    newGraph.re_orderCycles();
    newGraph.removeRepeated();

    aFile << endl << "Closed Cycles:" << endl;

    newGraph.printCyclesToFile( aFile );

    return newGraph.stackList.size();
}
