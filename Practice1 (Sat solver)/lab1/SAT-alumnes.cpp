#include <iostream>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <map>
#include <set>
using namespace std;

#define UNDEF -1
#define TRUE 1
#define FALSE 0



//struct occurList
struct dades {
   
    vector<int>clauses;
};
struct dades1 {
	
	int rank;

};

map<int, dades>occurList;
vector<int> ranking;


uint numVars;                  
uint numClauses;                
vector<vector<int> > clauses;                              
vector<int> model;              
vector<int> modelStack;         
uint indexOfNextLitToPropagate; 
uint decisionLevel;


void readClauses( ){
    // Skip comments
    char c = cin.get(); 
    
    while (c == 'c') { 
        while (c != '\n') c = cin.get();
        c = cin.get();
    }  
    
    string aux;
    cin >> aux >> numVars >> numClauses; 
    
    clauses.resize(numClauses); 
    ranking.resize(numVars + 1);
    for (uint i = 0; i < numClauses; ++i) {
        int lit;
        
		while (cin >> lit and lit != 0) {
            clauses[i].push_back(lit); 
            occurList[lit].clauses.push_back(i);
            if (lit > 0) ++ranking[lit];
            else ++ranking[-lit];
            
        } 
    } 
   
}
int currentValueInModel(int lit){
  if (lit >= 0) return model[lit];
  else {
    if (model[-lit] == UNDEF) return UNDEF;
    else return 1 - model[-lit];
  }
}
void setLiteralToTrue(int lit){
  modelStack.push_back(lit);
  if (lit > 0) model[lit] = TRUE;
  else model[-lit] = FALSE;		
}
bool propagateGivesConflict() {   
    while(indexOfNextLitToPropagate < modelStack.size()) { 
       
        if(modelStack[indexOfNextLitToPropagate] > 0) {
            auto it = occurList.find(-modelStack[indexOfNextLitToPropagate]);
            if(it != occurList.end()) {
                
                for(auto indexClause : it->second.clauses) {
                     
                    bool someLitTrue = false;
                    int numUndefs = 0;
                    int lastLitUndef = 0;
                 
                    for(uint k = 0; not someLitTrue and k < clauses[indexClause].size(); k++) {            
                        int val = currentValueInModel(clauses[indexClause][k]);

                        if(val == TRUE) {
                            someLitTrue = true;
                            
                        } else if(val == UNDEF) {
                            ++numUndefs; 
                            lastLitUndef = clauses[indexClause][k];
                        }
                    } 
                    
                    if (not someLitTrue and numUndefs == 0) {
                        ranking[modelStack[indexOfNextLitToPropagate]] +=3;
                        return true; // conflict! all lits false 
                    } 
                    else if (not someLitTrue and numUndefs == 1)  {
                        setLiteralToTrue(lastLitUndef);  
                    } 
                }
            }
        } else {
            auto it1 = occurList.find(-modelStack[indexOfNextLitToPropagate]);
            if(it1 != occurList.end()) {
            
                for(auto indexClause : it1->second.clauses) {
                     
                    bool someLitTrue = false;
                    int numUndefs = 0;
                    int lastLitUndef = 0;
                    
                    for(uint k = 0; not someLitTrue and k < clauses[indexClause].size(); k++) {
                       
                        int val = currentValueInModel(clauses[indexClause][k]);
                    
                        if(val == TRUE) {
                            someLitTrue = true;
                            
                        } else if(val == UNDEF) {
                            ++numUndefs; 
                            lastLitUndef = clauses[indexClause][k];
                        }
                        
                    }  
                    if (not someLitTrue and numUndefs == 0) {
                        ranking[-modelStack[indexOfNextLitToPropagate]] += 3;
                        return true; // conflict! all lits false (detecta un conflicto)
                    } 
                    else if (not someLitTrue and numUndefs == 1) { 
                        setLiteralToTrue(lastLitUndef);      
                    } 
                }
            }
        }
        ++indexOfNextLitToPropagate;
       
    }
     
    return false;
}
void backtrack(){
  uint i = modelStack.size() -1;
  int lit = 0;
  while (modelStack[i] != 0){ // 0 is the DL mark
    lit = modelStack[i];
    model[abs(lit)] = UNDEF;
    modelStack.pop_back();
    --i;
  }
  // at this point, lit is the last decision
  modelStack.pop_back(); // remove the DL mark
  --decisionLevel;
  indexOfNextLitToPropagate = modelStack.size();
  setLiteralToTrue(-lit);  // reverse last decision
}
// Heuristic for finding the next decision literal:
int getNextDecisionLiteral() {
   int aux_score = -1;
    int aux_lit = 0;
    for (uint i = 1; i <= numVars; ++i) {
        if (ranking[i] > aux_score && model[i] == UNDEF) {
            aux_score = ranking[i];
            aux_lit = i;
        }
    }
    return aux_lit; // returns 0 when all literals are defined 
}

void checkmodel(){
  for (uint i = 0; i < numClauses; ++i){
    bool someTrue = false;
    for (uint j = 0; not someTrue and j < clauses[i].size(); ++j)
      someTrue = (currentValueInModel(clauses[i][j]) == TRUE);
    if (not someTrue) {
      cout << "Error in model, clause is not satisfied:";
      for (uint j = 0; j < clauses[i].size(); ++j) cout << clauses[i][j] << " ";
      cout << endl;
      exit(1);
    }
  }  
}
int main(){ 
  readClauses(); // reads numVars, numClauses and clauses
  model.resize(numVars+1,UNDEF);
  indexOfNextLitToPropagate = 0;  
  decisionLevel = 0;
  
  // Take care of initial unit clauses, if any
  for (uint i = 0; i < numClauses; ++i)
    if (clauses[i].size() == 1) {
      int lit = clauses[i][0];
      int val = currentValueInModel(lit);
      if (val == FALSE) {cout << "UNSATISFIABLE" << endl; return 10;}
      else if (val == UNDEF) setLiteralToTrue(lit);
    }
  
  // DPLL algorithm
  while (true) {
    while ( propagateGivesConflict() ) {
      if ( decisionLevel == 0) { cout << "UNSATISFIABLE" << endl; return 10; }
      backtrack();
    }
    int decisionLit = getNextDecisionLiteral();
    if (decisionLit == 0) { checkmodel(); cout << "SATISFIABLE" << endl; return 20; }
    // start new decision level:
    modelStack.push_back(0);  // push mark indicating new DL
    ++indexOfNextLitToPropagate;
    ++decisionLevel;
    setLiteralToTrue(decisionLit);    // now push decisionLit on top of the mark
  }
}  