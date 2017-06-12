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
	
	void insertFuncWithHashList(Module& M, string sens_func, std::unordered_set<unsigned> hashes) {
		StringRef name(sens_func);
		Function* f = M.getFunction(name);
		if(f != nullptr) {
			
			unsigned length = hashes.size();
			// unsigned hash_array[length];
			
			
			LLVMContext& ctx = (*f).getContext();
			IntegerType* i32 = IntegerType::get(ctx, 32);
			ArrayType* hash_array = ArrayType::get(i32, length);
			
			int index = 0;
			for(unsigned hash: hashes){
				hash_array[index] = ConstantInt::get(i32, hash);
				// hash_array[index] = hash;
				index++;
			}
			
			
			//unsigned hash = 0;
			//hash = calculate_hash(whitelist);	       
			Constant* calc_hash_func = (*((*f).getParent())).getOrInsertFunction("hash_backtrace", Type::getVoidTy(ctx), Type::getInt32Ty(ctx), NULL);
			IRBuilder<> builder(ctx);
			Instruction* i = (*f).getEntryBlock().getFirstNonPHI();   
			builder.SetInsertPoint(i);
			Constant* arg = ConstantInt::get(Type::getInt32Ty(ctx), length);
			Constant* array = ConstantDataArray::get( ctx, hash_array);
			// Constant* arg2 = ConstantArray::get(Type::getInt32Ty(ctx), hash_array);
			Value* args[] = {arg2, arg}; 
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

		CallGraphWrapperPass* pass2 = getAnalysisIfAvailable<CallGraphWrapperPass>();
		const CallGraph& graph2 = (*pass2).getCallGraph();
		
		Graph g;
		g.fromLLVMCallGraph("main", graph2);
		
		// funcNodes has correct readKey, but tree traversal doesn't  
		std::map<string, Graph::GraphNode*>* funcs = g.getFunctionNodeMap();
		for(auto n: *funcs){
			n.second->print();
		}
		
		g.calculateHashes();
		// g.print();  // Tree is missing pointers for readKey...	
		
		for(string f: Param){
			
			// insertFuncWithHashList(M, f, hashes);
		}
		
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
				} else {
					fname = "null";
				}
				
				
				if(node.second != nullptr) {
					string calledfname;
					
					// Iterate over each function that this node calls
					for (auto callee: (*(node.second))){
						Function* callee_func = callee.second->getFunction();
						calledfname = callee_func->getName();
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

