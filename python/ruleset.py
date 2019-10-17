# Decompose an integer a number of possible states 
# and a neighbour range into
# a set of 1-D CA tranmission rules
# Currentlty just for a single neighbour

from math import log

from int_utils import *

class ruleset:
    "1-D Cellular automata ruleset, represented as an integer in base # of states"

    def __init__(self,r,s):
        "Init rulset from rule number, number of states and neighbour range"

        self.perms = pow(s,3)
        if r > 0 :assert log(r,s) < self.perms, "Rule number is outside range given parameters"
        
        self.states = s # Number of possible states
        self.rule = r   # Rule number (in base states)

        self.upStates = intToArr(self.rule,self.states,self.perms) # Updates mapping for this rule
        self.adjMat = [[] for i in xrange(self.perms)] # Adjencency Matrix
        self.transMat = [ [ 0 for i in xrange(self.perms)] for j in xrange(self.perms) ] # State Transition Matrix

        self.populateAdjMatrix()
        self.populateTransMatrix()

    def getUpState(self,n):
        "Get an update state, given a permutation number"
        
        assert n < self.perms, "State outside possible range"
        return self.upStates[n]

    def populateAdjMatrix(self):
        "Populate the state adjacency matrix of state triples to state triples"
        
        for i in xrange(self.perms):
        
            S = intToArr(i,self.states,3)
            L = []; R = []

            for j in xrange(self.states):
                L.append(arrToInt([j]+S[:2],self.states))
                R.append(arrToInt(S[1:]+[j],self.states))
            self.adjMat[i] += [ arrToInt([self.getUpState(a),self.getUpState(i),self.getUpState(b)],self.states) for a in L for b in R ]

    def populateTransMatrix(self):
        "Populate transmission matrix from enries in adjency matrix"

        for i in xrange(self.perms):
            for j in self.adjMat[i]: self.transMat[j][i] += 1
        for i in xrange(self.perms):
            assert sum([ self.transMat[j][i] for j in xrange(self.perms)]) == self.states*self.states, "Transsion Matrix overfill"

    def toString(self):
        
        ret = "Ruleset: "+str(self.rule)+" for "+str(self.states)+" states\n"
        ret += "Update rules: ["+",".join(str(i) for i in self.upStates)+"]\n"
        ret += " --- Adjency Matrix ---\n"
        for i in xrange(len(self.adjMat)): ret += '{:02}'.format(i)+"->|"+",".join(['{:02}'.format(j) for j in self.adjMat[i]])+"|\n"
        ret += " --- Unomralized Transition Matrix ---\n"
        for i in xrange(len(self.transMat)): ret+= '{:02}'.format(i)+"->|"+",".join(['{:02}'.format(j) for j in self.transMat[i]])+"|\n"
        return ret