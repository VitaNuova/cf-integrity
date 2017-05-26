#include "llvm/Pass.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Support/CommandLine.h"
#include <typeinfo>
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"


using namespace llvm;
using namespace std;

static cl::list<string> Param("sf", cl::desc("Sensitive functions"), cl::OneOrMore);

namespace {
   struct WhiteListPass: public ModulePass {
      static char ID;
      WhiteListPass(): ModulePass(ID) {}
      
      unsigned calculate_hash(vector<string> whitelist) {
         unsigned hash = 0;
     	 for(string fname: whitelist) {
	    for(char fchar: fname) {
	       hash += fchar;
	    }
	 }
	 errs() << "Hash value: " << hash << '\n';
	 return hash;
      }

      bool runOnModule(Module &M) override {
         for(string f: Param) {
	    errs() << "Sensitive function: " << f << '\n';	
	 }
  		//CallGraphWrapperPass* pass = getAnalysisIfAvailable<CallGraphWrapperPass>();
		// const CallGraph& graph = (*pass).getCallGraph();
		// const CallGraph& graph = SCC.getCallGraph();
		 /*for(const pair<const llvm::Function* const, unique_ptr<llvm::CallGraphNode>>& node: graph) {
			errs() << "Printing call graph" << '\n';
			if(node.first == NULL) {
				errs() << "Function pointer NULL" << '\n';
			} else {
				(*(node.first)).dump();
			}
			(*(node.second)).dump(); 
		 }*/
		 /*for(const pair<const Function* const, unique_ptr<CallGraphNode>>& node: graph) {
			if(node.first != nullptr) {
				errs() << '\n'<< "Function name: " << (*(node.first)).getName() << '\n';
			} else {
				errs() << "Function is null" << '\n';
			}
			if(node.second != nullptr) {
				errs() << "Size of node: " << (*(node.second)).size() << '\n'; //huge meaningless number
				Function* f = (*(node.second)).getFunction();
				if(f != nullptr) {
					errs() << "Function name (from call graph node):" << (*f).getName() << '\n';
					//errs() << "Function size: " << (*f).size() << '\n'; //segfault	
				}
			}
		 
		}*/
		 
		/*for(CallGraphNode* node: SCC) {
			Function* f = (*node).getFunction();
			if(f != nullptr) {
				errs() << "Function name: " << (*f).getName() << '\n';
			} else {
				errs() << "Function null " << '\n';
			}
			
		}*/
          vector<vector<string>> whitelists;
		
          vector<string> whitelist1;
          whitelist1.push_back("main");
     	  whitelist1.push_back("operation2");
	  whitelist1.push_back("encrypt");
	  whitelist1.push_back("readKey");
          whitelists.push_back(whitelist1);
		
	  vector<string> whitelist2;
	  whitelist2.push_back("main");
	  whitelist2.push_back("operation2");
	  whitelist2.push_back("decrypt");
	  whitelist2.push_back("readKey");
	  whitelists.push_back(whitelist2);

	  for(vector<string> whitelist: whitelists) {
	     unsigned hash = calculate_hash(whitelist);
	  }
      return false;
      }
   };
}


	char WhiteListPass::ID = 0;

	static RegisterPass<WhiteListPass> X("whitelist", "White List Pass", false, false);

	static void registerWhiteListPass(const PassManagerBuilder &,
				 legacy::PassManagerBase &PM) {
	  PM.add(new WhiteListPass());

	}
	static RegisterStandardPasses
	  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
			 registerWhiteListPass);
	
