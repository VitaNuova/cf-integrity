#include "llvm/Pass.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"


using namespace llvm;
namespace {
   struct WhiteListPass: public CallGraphSCCPass {
      static char ID;
      WhiteListPass(): CallGraphSCCPass(ID) {}

      bool runOnSCC(CallGraphSCC &SCC) override {
         errs() << "Hello: " << '\n';
         
 	 return false;
      }
   };
}

char WhiteListPass::ID = 0;

//TODO which registration is most necessary?
static RegisterPass<WhiteListPass> X("whitelist", "White List Pass", false, true);

static void registerWhiteListPass(const PassManagerBuilder &,
                         legacy::PassManagerBase &PM) {
  PM.add(new WhiteListPass());

}
static RegisterStandardPasses
  RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
                 registerWhiteListPass);

