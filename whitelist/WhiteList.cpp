#include "llvm/Pass.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/raw_ostream.h"
#include <typeinfo>
/*#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"*/


using namespace llvm;
using namespace std;

namespace {
   struct WhiteListPass: public CallGraphSCCPass {
      static char ID;
      WhiteListPass(): CallGraphSCCPass(ID) {}

      bool runOnSCC(CallGraphSCC &SCC) override {

	 //This is a way to get a call graph if we switch to ModulePass
	 /*CallGraphAnalysis analysis;
         ModuleAnalysisManager manager;
	 CallGraph graph = analysis.run(M, manager);*/

         const CallGraph& graph = SCC.getCallGraph();
	 for(const pair<const llvm::Function* const, unique_ptr<llvm::CallGraphNode>>& node: graph) {
		errs() << "Printing call graph" << '\n';
                if(node.first == NULL) {
			errs() << "Function pointer NULL" << '\n';
                } else {
			(*(node.first)).dump();
		}
        	(*(node.second)).dump(); 
	 }
         
 	 return false;
      }
   };
}

char WhiteListPass::ID = 0;

//TODO Registering the way described in assignment works for ModulePass but not SCC pass. Should we use ModulePass?
static RegisterPass<WhiteListPass> X("whitelist", "White List Pass", false, false);

/*static void registerWhiteListPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new WhiteListPass());

}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerWhiteListPass);
*/
