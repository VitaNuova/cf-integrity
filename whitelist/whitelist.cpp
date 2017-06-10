#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/Pass.h"
#include "llvm/Analysis/CallGraphSCCPass.h"
#include "llvm/Analysis/CallGraph.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/PassAnalysisSupport.h"
#include "llvm/Support/CommandLine.h"
#include <typeinfo>
#include <unordered_set>
#include "graph.cpp"


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

	void getAnalysisUsage(AnalysisUsage& AU) const override {
		AU.addRequired<CallGraphWrapperPass>();
		AU.setPreservesAll();
	}

	void insertFunc(Module& M, string sens_func, vector<string> whitelist) {
		StringRef name(sens_func);
		Function* f = M.getFunction(name);
		if(f != nullptr) {
			LLVMContext& ctx = (*f).getContext();
			unsigned hash = 0;
			hash = calculate_hash(whitelist);	       
			Constant* calc_hash_func = (*((*f).getParent())).getOrInsertFunction("hash_backtrace", Type::getVoidTy(ctx), Type::getInt32Ty(ctx), NULL);
			IRBuilder<> builder(ctx);
			Instruction* i = (*f).getEntryBlock().getFirstNonPHI();   
			builder.SetInsertPoint(i);
			Constant* arg = ConstantInt::get(Type::getInt32Ty(ctx), hash);
			Value* args[] = {arg}; 
			builder.CreateCall(calc_hash_func, args);
		} else {
			fprintf(stderr, "Specified function not found in this module.\n");
		}
	}

	bool runOnModule(Module &M) override {
		AnalysisUsage analysis;
		getAnalysisUsage(analysis);
		
		// Set of sensitive functions
		std::unordered_set<string> sensitive;
		for (string s: Param){
			sensitive.insert(s);
		}
		
		// 
		
		// moved from below to test
		CallGraphWrapperPass* pass2 = getAnalysisIfAvailable<CallGraphWrapperPass>();
		const CallGraph& graph2 = (*pass2).getCallGraph();
		// ~~~~~~~~~~~~~~~~~
		
		// Graph g = Graph("main");
		std::unordered_set<string> seen;
		Graph g;
		// g.fromLLVMCallGraph("main", graph2, seen);
		g.fromLLVMCallGraph("main", graph2);
		g.print();
		
		
		
		
		
		for(string sens_func: Param) {
			errs() << "Sensitive function: " << sens_func << '\n';	

			CallGraphWrapperPass* pass = getAnalysisIfAvailable<CallGraphWrapperPass>();
			const CallGraph& graph = (*pass).getCallGraph();
			// graph.dump();
			for(const pair<const Function* const, unique_ptr<CallGraphNode>>& node: graph) {
				// Pair: ( current function , graph of nodes I call )
				string fname;
				if(node.first != nullptr) {
					fname = (*(node.first)).getName();
					// errs() << '\n'<< "Function name: " << (*(node.first)).getName() << '\n';
					errs() << '\n'<< "Function name: " << fname << '\n';
				} else {
					fname = "null";
					errs() << "Function is null" << '\n';
				}
				
				
				if(node.second != nullptr) {
					// Size is probably number of callees?
					// errs() << "Size of node: " << (*(node.second)).size() << '\n'; //huge meaningless number
					
					string calledfname;
					
					// Iterate over each function that this node calls
					for (auto callee: (*(node.second))){
						Function* callee_func = callee.second->getFunction();
						calledfname = callee_func->getName();
						errs() << "callee: " << calledfname << '\n';
					}
					
					Function* f = (*(node.second)).getFunction();
					
					if(f != nullptr) {
						calledfname = (*(f)).getName();
						errs() << fname << " calls " << calledfname << '\n';
						// errs() << "Function name (from call graph node):" << (*f).getName() << '\n';
						// errs() << "Function size: " << (*f).size() << '\n'; //segfault	
					}
				}

			}


			vector<string> whitelist;
			whitelist.push_back("operation2");
			whitelist.push_back("encrypt");
			whitelist.push_back("readKey");
			whitelist.push_back("main");

			insertFunc(M, sens_func, whitelist);
		}
		return true;
	}
};
}

	char WhiteListPass::ID = 0;

	static RegisterPass<WhiteListPass> X("whitelist", "White List Pass", false, false);

