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
		GraphNode();
		GraphNode(string name);
		void addChild(string child_name);
		void addChild(GraphNode* new_child);
		void mergeChildren(std::unordered_set<GraphNode*> new_children);
		void print();
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
	// - std::map<string, GraphNode*> funcs;
	std::map<string, std::unordered_set<string>>* funcs;
	std::map<string, GraphNode*>* funcNodes;
};

Graph::Graph(){
	// - std::map<string, GraphNode*>* funcMap = new map<string, GraphNode*>(); //  = new map<string, GraphNode>()
	// std::map<string, GraphNode*> &funcs = *funcMap;
	// - funcs = *funcMap;
	funcs = new map<string, std::unordered_set<string>>;
	funcNodes = new map<string, GraphNode*>;
}

Graph::Graph(string name){
	root = GraphNode(name);
	// - std::map<string, GraphNode*>* funcMap = new map<string, GraphNode*>(); //  = new map<string, GraphNode>()
	// std::map<string, GraphNode*> &funcs = *funcMap;
	// - funcs = *funcMap;
	funcs = new map<string, std::unordered_set<string>>;
	funcNodes = new map<string, GraphNode*>;
}

void Graph::print(){
	errs() << "Graph root: " << root.name << '\n';
}



void Graph::fromLLVMCallGraph(string root_name, const CallGraph& graph){	
	string fname;	
	
	// Loop over the graph
	for(const pair<const Function* const, unique_ptr<CallGraphNode>>& node: graph) {
		if(node.first != nullptr) {
			fname = (*(node.first)).getName();
			
			// Build the list of children for this node, if it has any
			std::unordered_set<string> children;		
			std::unordered_set<GraphNode*> childNodes;			
			if(node.second != nullptr) {
				string calleename;

				// Iterate over each function that this node calls				
				for (auto callee: (*(node.second))){
					Function* callee_func = callee.second->getFunction();
					calleename = callee_func->getName();				
					
					errs() << "Callee name: " << calleename << '\n';
					
					GraphNode* childNode;
					
					if(funcNodes->count(calleename) == 0){
						// Add my child to the map if it isn't in it already
						errs() << "Adding cNode to map: " << calleename << '\n';
						childNode = new GraphNode(calleename);
					} else {
						// Retrieve the child from the map
						errs() << "Get cNode from map: " << calleename << '\n';
						childNode = funcNodes->at(calleename);
					}	
					
					childNode->print();
					
					/*
					if(funcs->count(calleename) == 0){
						// Add my child to the map if it isn't in it already
						errs() << "Adding cString to map: " << calleename << '\n';
						
					} else {
						// Retrieve the child from the map
						errs() << "Get cString from map: " << calleename << '\n';
					}	*/
					
					children.insert(calleename);
					childNodes.insert(childNode);
					// 				
				}
			} // Otherwise null .second
			
			GraphNode* parentNode;
			// If I don't exist in the map, add me			
			if(funcs->count(fname) == 0){
				errs() << "Adding pNode to map: " << fname << '\n';
				parentNode = new GraphNode(fname);
			} else {
				// Use the existing node from the map, merge the children
				errs() << "Get pNode from map: " << fname << '\n';
				parentNode = funcNodes->at(fname);				
			}
			
			/*
			if(funcs->count(fname) == 0){
				errs() << "Adding to map: " << fname << '\n';
			} else {
				// Use the existing node from the map
				errs() << "Get from map: " << fname << '\n';
			}*/		
			
			// Merge children
			parentNode->mergeChildren(childNodes);
			funcNodes->insert(pair<string, GraphNode*>(fname, parentNode));
			
			//funcs->insert(pair<string, std::unordered_set<string>>(fname, children));			

		} else {
			// Don't deal with null nodes for now
		}		
		errs() << '\n';
	}
	
	for(auto f: *funcs){
		errs() << "Func: " << f.first << '\n';
		for(auto c: f.second){
			errs() << "  - child: " << c << '\n';
		}
		
	}

	// Add every function to the map:
	// Create a GNode for it, with its children
	// For each child, add IT to the map
	// If it exists, 	
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
	errs() << " + Adding child " << new_child->name << " to " << this->name << '\n';
	children.insert(new_child);
}

void Graph::GraphNode::mergeChildren(std::unordered_set<GraphNode*> new_children){
	children.insert(new_children.begin(), new_children.end());
}

void Graph::GraphNode::printChildren(){	
	errs() << "Children of " << name << ":" << '\n';
	for(auto child:this->children){
		errs() << "   - Child: " << child->name << '\n';
	}	
}

void Graph::GraphNode::print(){
	errs() << "GraphNode: " << name << ":" << '\n';
	if (this->children.size() > 0){
		this->printChildren();
	} else {
		errs() << name << " has no children. " << '\n';
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




// Almost working. Bug where cgn = gn for some reason. Scoping? pointers? wtf
void Graph::fromLLVMCallGraph(string root_name, const CallGraph& graph){
	//std::map<string, GraphNode*>* funcMap = new map<string, GraphNode*>(); //  = new map<string, GraphNode>()
	// std::map<string, GraphNode*> &funcs = *funcMap;
	//&funcs = *funcMap;
	
	string fname;
	
	GraphNode gno;
	GraphNode* gn;
	
	// GraphNode cgno;
	GraphNode* cgno;
	// GraphNode cgno = new GraphNode();
	GraphNode* cgn;
	
	
	// Loop over the graph
	for(const pair<const Function* const, unique_ptr<CallGraphNode>>& node: graph) {
		// string fname;
		if(node.first != nullptr) {
			fname = (*(node.first)).getName();
			
			// GraphNode* gn;
			// If I don't exist in the map, add me
			if(funcs.count(fname) == 0){
				// GraphNode gno = GraphNode(fname);
				gno = GraphNode(fname);
				gn = &gno;
				errs() << "Adding to map: " << gn->name << '\n';
				funcs[fname] = gn;
				
				// gno.print(); // As expected
				// funcs[fname]->print(); // As expected
			} else {
				// Use the existing node from the map
				errs() << "Get from map: " << fname << '\n';
				gn = funcs[fname];
			}				
			
			// funcs[fname]->print();
			
			if(node.second != nullptr) {
				string calleename;

				// Iterate over each function that this node calls
				for (auto callee: (*(node.second))){
					Function* callee_func = callee.second->getFunction();
					calleename = callee_func->getName();
					
					funcs[fname]->print();
					
					errs() << "Callee name: " << calleename << '\n';
					
					// GraphNode* cgn;
					if(funcs.count(calleename) == 0){
						// Add my child to the map if it isn't in it already
						errs() << "Adding child to map: " << calleename << '\n';
						// GraphNode cgno = GraphNode(calleename);
						cgno = new GraphNode(calleename);
						// cgn = &cgno;
						cgn = cgno;
						// funcs[calleename] = cgn;
						funcs.insert(pair<string, GraphNode*>(calleename, cgn));
					} else {
						// Retrieve the child from the map
						errs() << "Get child from map: " << calleename << '\n';
						// cgn = funcs[calleename];
						cgn = funcs.at(calleename);
						cgn->print();  // THIS PRINTS THE OUTER ONE. WHY
						errs() << "~~~~~~~~~~~~~~~" << '\n';
					}	
					
					gn->print();
					
					// Add this child to my children
					// gn->children.insert(cgn);	
					gn->addChild(cgn);  // Adding the children is acting funny.
					
					gn->print();				
					
					// Add me to my child's callers
					// cgn->callers.insert(gn);					
				}
				

			} else {
				// could be non-null and still have no children. Can check this above
				errs() << "node has no children" << '\n';
			}

		} else {
			// Don't deal with null nodes for now
		}
		
		errs() << '\n';
	}

	// Add every function to the map:
	// Create a GNode for it, with its children
	// For each child, add IT to the map
	// If it exists, 
	
	for(auto f: funcs){
		errs() << "Func: " << f.first << '\n';
		for(auto c: f.second->children){
			errs() << "  - child: " << c->name << '\n';
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


