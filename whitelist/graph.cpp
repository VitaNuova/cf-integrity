#include <stdio.h>
#include <iostream>
#include <queue>

using namespace std;
using namespace llvm;



class Graph {
public:
	class GraphNode {
	public:
		std::string name;
		// vector<GraphNode*> children;
		std::unordered_set<GraphNode*> children;
		std::unordered_set<GraphNode*> callers;
		// vector<GraphNode*> callers;
		GraphNode();
		GraphNode(string name);
		void addChild(string child_name);
		void addChild(GraphNode* new_child);
		void printChildren();
	};
	
	GraphNode root;
	// int size;
	
	Graph();
	Graph(string name);
	void print();
	// void fromLLVMCallGraph(string root_name, const CallGraph& graph, std::unordered_set<string> seen);
	void fromLLVMCallGraph(string root_name, const CallGraph& graph);
	// const pair<const Function* const, unique_ptr<CallGraphNode>>& findinLLVMGraph(string name, const CallGraph&);
	// void insert();
};

Graph::Graph(){
	
}

Graph::Graph(string name){
	root = GraphNode(name);
}

void Graph::print(){
	errs() << "Graph root: " << root.name << '\n';
}



void Graph::fromLLVMCallGraph(string root_name, const CallGraph& graph){
	std::map<string, GraphNode*> funcs;
	
	// Loop over the graph
	for(const pair<const Function* const, unique_ptr<CallGraphNode>>& node: graph) {
		string fname;
		if(node.first != nullptr) {
			fname = (*(node.first)).getName();
			
			GraphNode* gn;
			// If I don't exist in the map, add me
			if(funcs.count(fname) == 0){
				// errs() << "Adding to map: " << fname << '\n';
				GraphNode gno = GraphNode(fname);
				gn = &gno;
				errs() << "Adding to map: " << gn->name << '\n';
				funcs[fname] = gn;
			} else {
				// Use the existing node from the map
				errs() << "Get from map: " << fname << '\n';
				gn = funcs[fname];
			}
			
			if(node.second != nullptr) {
				string calleename;

				// Iterate over each function that this node calls
				for (auto callee: (*(node.second))){
					Function* callee_func = callee.second->getFunction();
					calleename = callee_func->getName();
					
					GraphNode* cgn;
					if(funcs.count(calleename) == 0){
						// Add my child to the map if it isn't in it already
						errs() << "Adding child to map: " << calleename << '\n';
						GraphNode cgno = GraphNode(calleename);
						cgn = &cgno;
						funcs[calleename] = cgn;
					} else {
						// Retrieve the child from the map
						errs() << "Get child from map: " << calleename << '\n';
						cgn = funcs[calleename];
					}	
					
					gn->printChildren();
					
					// Add this child to my children
					// gn->children.insert(cgn);	
					gn->addChild(cgn);  // Adding the children is acting funny.
					
					gn->printChildren();
					
					//for(auto child: gn->children){
					//	errs() << "p2Child: " << child->name << '\n';
					//}
					
					// Add me to my child's callers
					// cgn->callers.insert(gn);
				}
				
				for(auto child: gn->children){
					errs() << "Child: " << child->name << '\n';
				}
			} else {
				// could be non-null and still have no children. Can check this above
				errs() << "node has no children" << '\n';
			}

		} else {
			// Don't deal with null nodes for now
		}
	}

	// Add every function to the map:
	// Create a GNode for it, with its children
	// For each child, add IT to the map
	// If it exists, 
	
	for(auto f: funcs){
		errs() << "Func: " << f.first << '\n';
		for(auto c: f.second->children){
			errs() << c->name << '\n';
		}
		
	}
	
}














Graph::GraphNode::GraphNode(){
	
}

Graph::GraphNode::GraphNode(string name){
	this->name = name;
}

void Graph::GraphNode::addChild(string child_name){
	GraphNode new_child = GraphNode(name);
	// children.push_back(&new_child);
	children.insert(&new_child);
}

void Graph::GraphNode::addChild(GraphNode* new_child){
	children.insert(new_child);
}

void Graph::GraphNode::printChildren(){	
	errs() << "Children of " << name << ":" << '\n';
	for(auto child:this->children){
		errs() << "Child - Name: " << child->name << '\n';
	}	
}











/*
 * 
 * std::vector<GraphNode> getChildren(){
	return children;
}

void Graph::insert(){

}


void Graph::xfromLLVMCallGraph(string root_name, const CallGraph& graph, std::unordered_set<string> seen){
	Graph g;
	
	for(const pair<const Function* const, unique_ptr<CallGraphNode>>& node: graph) {
		// Pair: ( current function , graph of nodes I call )
		string fname;
		if(node.first != nullptr) {
			fname = (*(node.first)).getName();
			// errs() << '\n'<< "Function name: " << (*(node.first)).getName() << '\n';
			// errs() << '\n'<< "Function name: " << fname << '\n';

			if (fname == root_name){
				// Found the intended root node, make it our root and add its children
				errs() << "New root will be " << fname << '\n';
				g = Graph(fname);
				seen.insert(fname);

				if(node.second != nullptr) {
					string calledfname;

					// Iterate over each function that this node calls
					// and add a subgraph for each of them
					for (auto callee: (*(node.second))){
						Function* callee_func = callee.second->getFunction();
						calledfname = callee_func->getName();

						if(seen.count(calledfname) > 0){
							// This child has already been seen
							// Don't handle cycles right now. Skip this child.
						} else {
							Graph childGraph;
							childGraph.fromLLVMCallGraph(calledfname, graph, seen);
							// Graph childGraph = Graph(calledfname);
							// Graph::GraphNode childRoot = childGraph.root;
							// g.root.addChild(*childRoot);
						}

						//errs() << "callee: " << calledfname << '\n';
					}

					// Function* f = (*(node.second)).getFunction();

					//if(f != nullptr) {
					//calledfname = (*(f)).getName();
					// errs() << fname << " calls " << calledfname << '\n';
					// errs() << "Function name (from call graph node):" << (*f).getName() << '\n';
					// errs() << "Function size: " << (*f).size() << '\n'; //segfault	
					//}
				}				
			}						
		} else {
			// Don't deal with null nodes for now
		}
	}
	this->root = g.root;
	// return g;
}







const pair<const Function* const, unique_ptr<CallGraphNode>>& findinLLVMGraph(string name, const CallGraph& graph){
	pair<const Function* const, unique_ptr<CallGraphNode>>& result = std::make_pair();
	for(const pair<const Function* const, unique_ptr<CallGraphNode>>& node: graph) {
		if(node.first != nullptr) {
			string fname = (*(node.first)).getName();

			if (fname == name){
				return node;
			}						
		} 
	}
	return result;
}





void Graph::fromLLVMCallGraph(string root_name, const CallGraph& graph){
	Graph g;
	std::unordered_set<string> seen;
	std::queue<GraphNode*> q;
	std::map<string, GraphNode*> funcs;
	
	// Add start node to queue
	bool found = false;
	for(const pair<const Function* const, unique_ptr<CallGraphNode>>& node: graph) {
		if(node.first != nullptr) {
			string fname = (*(node.first)).getName();

			if (fname == root_name){				
				g = Graph(fname);
				GraphNode gn = GraphNode(fname);
				q.push(&gn);

				found = true;
			}						
		} 
	}
	
	if(!found){
		// The expected root was not found.
		errs() << "The expected root " << root_name << " was not found." << '\n';
		return;
	}
	
	while(!q.empty()){
		// while queue isn't empty, get the next element, get the children.
		// if the child hasn't been seen, add it to the queue
		// - Create a new node for the child
		// - Add the node as my child 
		
		// If the node has been seen, add a parent link for it to this node
		
		GraphNode* current = q.front();
		q.pop();
		errs() << "Current: " << current->name << '\n';
		
		// If current hasn't been seen
		// Get children of current
		if(seen.count(current->name) == 0){
			// Node has not been processed
			
			// Find it in the LLVM Graph
			
			
			// Get its children
			
			// 
			
			// For every child of this node
			for(auto child: current->children){
				
				// Find the child in the LLVM callgraph
				bool child_found = false;
				for(const pair<const Function* const, unique_ptr<CallGraphNode>>& node: graph) {
					if(node.first != nullptr) {
						string fname = (*(node.first)).getName();

						if (fname == child->name){		
							
							// Get the 
							
							g = Graph(fname);
							GraphNode gn = GraphNode(fname);
							q.push(&gn);
							child_found = true;
						}						
					} 
				}
				
			}
			
			seen.insert(current->name);
		} else {
			// Node has been seen already
			// current->callers.insert()
		}
		
	}	
}





















		// Get graph
		// - CallGraphWrapperPass* pass2 = getAnalysisIfAvailable<CallGraphWrapperPass>();
		// - const CallGraph& graph2 = (*pass2).getCallGraph(); // CallGraph&
		// - errs() << "First function: " << graph2.getRoot();
		
		// Start at root of graph
		// const pair<const Function* const, unique_ptr<CallGraphNode>>& node2 = graph2.begin();
		// errs() << "First function: " << (*(node2.first)).getName();
		// CallGraphNode *root = graph2.getRoot();
		// - CallGraphNode *root = graph2.getExternalCallingNode();
		// errs() << "First function: " << root->getIntrinsicID();

*/


