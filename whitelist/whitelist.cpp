#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"
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

	/*unsigned calculate_hash(vector<string> whitelist) {
		unsigned hash = 0;
		for(string fname: whitelist) {
			for(char fchar: fname) {
				hash += fchar;
			}
		}
		errs() << "Hash value: " << hash << '\n';
		return hash;
	}*/

	void getAnalysisUsage(AnalysisUsage& AU) const override {
		AU.addRequired<CallGraphWrapperPass>();
		AU.setPreservesAll();
	}

	/*void insertFunc(Module& M, string sens_func, vector<string> whitelist) {
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
	}*/
	
	void insertFuncWithHashList(Module& M, string sens_func, std::unordered_set<unsigned> hashes) {
		StringRef name(sens_func);
		Function* f = M.getFunction(name);
                std::vector<unsigned> hashes_vect(hashes.begin(), hashes.end());
		if(f != nullptr) {
			
			unsigned length = hashes_vect.size();
			
			LLVMContext& ctx = (*f).getContext();	
			IRBuilder<> builder(ctx);
			Instruction* i = (*f).getEntryBlock().getFirstNonPHI();   
			builder.SetInsertPoint(i);

			IntegerType* i32 = IntegerType::get(ctx, 32);
			ArrayType* hash_array = ArrayType::get(i32, length);
			AllocaInst* al_inst = builder.CreateAlloca(hash_array);
                        Value* index_list[2];			
                        Value* gep;
                        for(unsigned i = 0; i < hashes_vect.size(); i++) {
			   index_list[0] = ConstantInt::get(Type::getInt32Ty(ctx), 0);
                           index_list[1] = ConstantInt::get(Type::getInt32Ty(ctx), i);
                           gep = builder.CreateGEP(al_inst, index_list);
                           builder.CreateStore(ConstantInt::get(Type::getInt32Ty(ctx), hashes_vect[i]), gep);
		        }

			 Constant* calc_hash_func = (*((*f).getParent())).getOrInsertFunction("hash_backtrace", Type::getVoidTy(ctx), Type::getInt32PtrTy(ctx), Type::getInt32Ty(ctx), NULL);
			index_list[0] = ConstantInt::get(Type::getInt32Ty(ctx), 0);
                        index_list[1] = ConstantInt::get(Type::getInt32Ty(ctx), 0);
                        Value* arg1 = builder.CreateGEP(al_inst, index_list);
			Constant* arg2 = ConstantInt::get(Type::getInt32Ty(ctx), length);
			Value* args[] = {arg1, arg2}; 
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
		errs() << "\nHash calculation\n";
		g.calculateHashes();
                g.print();  // Tree is missing pointers for readKey...	
		
		for(string f: Param){
                   std::unordered_set<unsigned> hashes = g.getValidHashes(f);
		   insertFuncWithHashList(M, f, hashes);
		}
		
		return true;
	}
};
}

	char WhiteListPass::ID = 0;

	static RegisterPass<WhiteListPass> X("whitelist", "White List Pass", false, false);

