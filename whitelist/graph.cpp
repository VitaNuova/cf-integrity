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
	
	GraphNode* root;
	
	Graph();
	Graph(string name);
	void print();
	void printBFS();
	void fromLLVMCallGraph(string root_name, const CallGraph& graph);
	std::map<string, std::unordered_set<string>>* funcs;
	std::map<string, GraphNode*>* funcNodes;
};

Graph::Graph(){
	funcs = new map<string, std::unordered_set<string>>;
	funcNodes = new map<string, GraphNode*>;
}

Graph::Graph(string name){
	GraphNode rn = GraphNode(name);
	root = &rn;
	funcs = new map<string, std::unordered_set<string>>;
	funcNodes = new map<string, GraphNode*>;
}

void Graph::print(){
	errs() << "Graph root: " << root->name << '\n';
	printBFS();
}

void Graph::fromLLVMCallGraph(string root_name, const CallGraph& graph){	
	string fname;	
	
	// Loop over the graph
	for(const pair<const Function* const, unique_ptr<CallGraphNode>>& node: graph) {
		if(node.first != nullptr) {
			fname = (*(node.first)).getName();
			
			// Build the list of children for this node, if it has any	
			std::unordered_set<GraphNode*> childNodes;			
			if(node.second != nullptr) {
				string calleename;

				// Iterate over each function that this node calls				
				for (auto callee: (*(node.second))){
					Function* callee_func = callee.second->getFunction();
					calleename = callee_func->getName();				
					
					// errs() << "Callee name: " << calleename << '\n';
					
					GraphNode* childNode;
					
					if(funcNodes->count(calleename) == 0){
						// Add my child to the map if it isn't in it already
						childNode = new GraphNode(calleename);
					} else {
						// Retrieve the child from the map
						childNode = funcNodes->at(calleename);
					}	
					
					childNodes.insert(childNode);
					// 				
				}
			} // Otherwise null .second
			
			GraphNode* parentNode;
			// If I don't exist in the map, add me			
			if(funcs->count(fname) == 0){
				parentNode = new GraphNode(fname);
			} else {
				// Use the existing node from the map, merge the children
				parentNode = funcNodes->at(fname);				
			}
			
			// Merge children
			parentNode->mergeChildren(childNodes);
			parentNode->print();
			funcNodes->insert(pair<string, GraphNode*>(fname, parentNode));		

		} else {
			// Don't deal with null nodes for now
		}		
		errs() << '\n';
	}
	
	root = funcNodes->at(root_name);
	if(root == nullptr){
		errs() << "Unable to build graph. The requested root <" << root_name << "> was not found." << '\n';
	}

	// Add every function to the map:
	// Create a GNode for it, with its children
	// For each child, add IT to the map
	// If it exists, 	
}

void Graph::printBFS(){
	// List of seen nodes
	std::unordered_set<string> seen;
	
	// Queue to store nodes to explore
	std::queue<GraphNode*> q;
	
	// Start at root
	q.push(root);
	
	// Go!
	while(!q.empty()){
		GraphNode* current = q.front();
		q.pop();
		
		errs() << "Current node: " << current->name << '\n';
		
		seen.insert(current->name);
		// What order should adding to 'seen' and adding children be?
		
		for(GraphNode* child:current->children){
			if(seen.count(child->name) == 0){
				q.push(child);
			}			
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
	errs() << " + Adding child " << new_child->name << " to " << this->name << '\n';
	children.insert(new_child);
}

void Graph::GraphNode::mergeChildren(std::unordered_set<GraphNode*> new_children){
	children.insert(new_children.begin(), new_children.end());
}

void Graph::GraphNode::printChildren(){	
	// errs() << "Children of " << name << ":" << '\n';
	for(auto child:this->children){
		errs() << "   - Child: " << child->name << '\n';
	}	
}

void Graph::GraphNode::print(){
	errs() << "GraphNode: " << name << ":" << '\n';
	if (this->children.size() > 0){
		this->printChildren();
	} else {
		errs()<< "   " << name << " has no children. " << '\n';
	}
}




