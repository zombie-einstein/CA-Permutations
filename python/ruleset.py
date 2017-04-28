# Decompose an integer a number of possible states 
# and a neighbour range into
# a set of 1-D CA tranmission rules
# Currentlty just for a single neighbour

from math import log

def intToArr(n,base,l):
    "Convert int n to array of digits in base for fixed digits l"
    ret = []
    for i in xrange(l):
        ret.append(n%base)
        n /= base
    return ret

def arrToInt(arr,base):
    "Convert array of digits back to int using base"
    return sum([arr[i]*pow(base,i) for i in range(0,len(arr))])

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
                L.append(arrToInt([j]+S[1:],self.states))
                R.append(arrToInt(S[:2]+[j],self.states))
            self.adjMat[i] += [ arrToInt([self.getUpState(a),S[1],self.getUpState(b)],self.states) for a in L for b in R ]

    def populateTransMatrix(self):
        "Populate transmission matrix from enries in adjency matrix"

        for i in xrange(self.perms):
            for j in self.adjMat[i]:
                self.transMat[i][j] += 1
            assert sum(self.transMat[i]) == self.states*self.states, "Transsion Matrix overfill"

    def printRule(self):
        
        print "Ruleset: "+str(self.rule)+" for "+str(self.states)+" states"
        print "Update rules:",self.upStates
        print " -- Adjency Matrix --"
        for i in xrange(len(self.adjMat)): print '{:02}'.format(i)+"->|"+",".join(['{:02}'.format(j) for j in self.adjMat[i]])+"|"
        print "--- Unomralized Transition Matrix ---"
        for i in xrange(len(self.transMat)): print '{:02}'.format(i)+"->|"+",".join(['{:02}'.format(j) for j in self.transMat[i]])+"|"