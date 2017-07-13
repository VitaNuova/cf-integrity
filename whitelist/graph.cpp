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
		std::unordered_set<GraphNode*> parents;
		std::unordered_set<unsigned> hashes;
		
		GraphNode();
		GraphNode(string name);
		void addChild(string child_name);
		void addChild(GraphNode* new_child);
		void addParent(GraphNode* new_parent);
		void mergeChildren(std::unordered_set<GraphNode*> new_children);
		void mergeParents(std::unordered_set<GraphNode*> new_parents);
		void print();
		void printChildren();
		void printParents();
		void printHashes();
	};
	
	GraphNode* root;
	std::map<string, GraphNode*>* funcNodes;
	
	Graph();
	Graph(string name);
	void print();
	void printBFS();
	void calculateHashes();
	void fromLLVMCallGraph(string root_name, const CallGraph& graph);
	std::map<string, GraphNode*>* getFunctionNodeMap();
	std::unordered_set<unsigned> getValidHashes(string name);

};

Graph::Graph(){
	funcNodes = new map<string, GraphNode*>;
	GraphNode* dummygn = new GraphNode("dummy");
	pair<string, GraphNode*> dummy = pair<string, GraphNode*>("dummy", dummygn);
	funcNodes->insert(dummy);
	funcNodes->erase("dummy");
}

Graph::Graph(string name){
	GraphNode rn = GraphNode(name);
	root = &rn;
	funcNodes = new map<string, GraphNode*>;
}

void Graph::print(){
	errs() << "Graph root: " << root->name << '\n';
	printBFS();
}

void Graph::fromLLVMCallGraph(string root_name, const CallGraph& graph){	
	string fname;	
	
	// Loop over the LLVM graph
	for(const pair<const Function* const, unique_ptr<CallGraphNode>>& node: graph) {
		if(node.first != nullptr && (*(node.first)).getBasicBlockList().size() != 0) {
			fname = (*(node.first)).getName();
			
			errs() << "Function name: " << fname << '\n';
			errs() << "And its tree: \n";
                        (*(node.second)).dump();
			
                        GraphNode* parentNode;
			// If I don't exist in the map, add me			
			if(funcNodes->count(fname) == 0){
				parentNode = new GraphNode(fname);
			} else {
				// Use the existing node from the map, merge the children
				parentNode = funcNodes->at(fname);				
			}
			
			// Build the list of children for this node, if it has any	
			std::unordered_set<GraphNode*> childNodes;			
			if(node.second != nullptr) {
                                node.second->dump();
				string calleename;

				// Iterate over each function that this node calls				
				for (auto callee: (*(node.second))){
					Function* callee_func = callee.second->getFunction();
					if((*callee_func).getBasicBlockList().size() != 0) {
					   calleename = callee_func->getName();				
					
					// errs() << "Callee name: " << calleename << '\n';
					
					   GraphNode* childNode;
					
					   if(funcNodes->count(calleename) == 0){
						// Add my child to the map if it isn't in it already
						childNode = new GraphNode(calleename);
                                                funcNodes->insert(pair<string, GraphNode*>(calleename, childNode));
					   } else {
						// Retrieve the child from the map
						childNode = funcNodes->at(calleename);
					   }	
					   childNode->addParent(parentNode);
					   childNodes.insert(childNode);
					}
					// 				
				}
			} // Otherwise null .second, so no children
			
			// Merge children
			parentNode->mergeChildren(childNodes);
			
			// parentNode->print();
			funcNodes->insert(pair<string, GraphNode*>(fname, parentNode));	
			
			errs() << "FuncNodes:" << '\n';
			for(auto p: *funcNodes){
				errs() << p.first << ":";
				p.second->print();
			}
			errs() << "~~~ End FuncNodes" << '\n';

		} else {
			// Don't deal with null nodes
			errs() << "Skipping null node" << '\n';
		}		
		errs() << '\n';
	}
	
	root = funcNodes->at(root_name);
	if(root == nullptr){
		errs() << "Unable to build graph. The requested root <" << root_name << "> was not found." << '\n';
	}
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
		
		current->print();
		
		seen.insert(current->name);
		// What order should adding to 'seen' and adding children be?
		
		for(GraphNode* child:current->children){
			if(seen.count(child->name) == 0){
				q.push(child);
			}			
		}		
	}
}

// Should make the BFS its own function/iterator
void Graph::calculateHashes(){
	
	// List of seen nodes
	std::unordered_set<string> seen;
	
	// Queue to store nodes to explore
	std::queue<GraphNode*> q;
	
	// Start at root
	unsigned roothash = 0;
	for(char fchar: root->name){
		roothash += fchar;
	}
	root->hashes.insert(roothash);
	q.push(root);
	
	// Go!
	while(!q.empty()){

		GraphNode* current = q.front();
		q.pop();
                bool all_parents_seen = true; 		
		// For each of my parents,
		// For each of hash of the parent,
		// Add a hash to my hash list, hash = pHash + meHash;
		for(auto parent:current->parents){
                        if(seen.count(parent->name) == 0) {
                           all_parents_seen = false;
			   break;
                        }
			for(auto hash: parent->hashes){
				unsigned newhash = hash;
				for(char fchar: current->name){
					newhash += fchar;
				}
				current->hashes.insert(newhash);
                                current->print();
			}
		}
		if(all_parents_seen) {
               // additional check to prevent the situation where the hashes for a child are calculated earlier than for some of the parents
                //if(current->hashes.size() >= current->parents.size()) {
		   seen.insert(current->name);
		   
               // }
		// What order should adding to 'seen' and adding children be?
		
		   for(GraphNode* child:current->children){
			if(seen.count(child->name) == 0){
				q.push(child);
			}			
		   }
                }		
	}
}

// Should make the BFS its own function/iterator
std::unordered_set<unsigned> Graph::getValidHashes(string name){
	
	// List of seen nodes
	std::unordered_set<string> seen;
	
	// Queue to store nodes to explore
	std::queue<GraphNode*> q;
	
	// Start at root
	unsigned roothash = 0;
	for(char fchar: root->name){
		roothash += fchar;
	}
	root->hashes.insert(roothash);
	q.push(root);
	
	// Go!
	while(!q.empty()){
		GraphNode* current = q.front();
		q.pop();
		
		if(current->name == name){
			return current->hashes;
		}
		
		seen.insert(current->name);
		// What order should adding to 'seen' and adding children be?
		
		for(GraphNode* child:current->children){
			if(seen.count(child->name) == 0){
				q.push(child);
			}			
		}		
	}
	std::unordered_set<unsigned> empty;
	return empty;
}

std::map<string, Graph::GraphNode*>* Graph::getFunctionNodeMap(){
	return funcNodes;
}







Graph::GraphNode::GraphNode(){

}

Graph::GraphNode::GraphNode(string name){
	this->name = name;
}

void Graph::GraphNode::addChild(string child_name){
	GraphNode new_child = GraphNode(name);
	children.insert(&new_child);
}

void Graph::GraphNode::addChild(GraphNode* new_child){
	// errs() << " + Adding child " << new_child->name << " to " << this->name << '\n';
	children.insert(new_child);
}

void Graph::GraphNode::addParent(GraphNode* new_parent){
	// errs() << " + Adding parent " << new_parent->name << " to " << this->name << '\n';
	parents.insert(new_parent);
}

void Graph::GraphNode::mergeChildren(std::unordered_set<GraphNode*> new_children){
	children.insert(new_children.begin(), new_children.end());
}

void Graph::GraphNode::mergeParents(std::unordered_set<GraphNode*> new_parents){
	parents.insert(new_parents.begin(), new_parents.end());
}

void Graph::GraphNode::printChildren(){	
	for(auto child:this->children){
		errs() << "   - Child: " << child->name << '\n';
	}	
}

void Graph::GraphNode::printParents(){	
	for(auto parent:this->parents){
		errs() << "   - Parent: " << parent->name << '\n';
	}	
}

void Graph::GraphNode::printHashes(){	
	for(auto hash:this->hashes){
		errs() << "      - Hash: " << hash << '\n';
	}	
}

void Graph::GraphNode::print(){
	errs() << "GraphNode: " << name << ":" << '\n';
	if (this->children.size() > 0){
		this->printChildren();
	} else {
		errs()<< "   " << name << " has no children. " << '\n';
	}
	
	if (this->parents.size() > 0){
		this->printParents();
	} else {
		errs()<< "   " << name << " has no parents. " << '\n';
	}
	
	if (this->hashes.size() > 0){
		this->printHashes();
	} else {
		errs()<< "   " << name << " has no hashes. " << '\n';
	}
}

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

/*
 * 
 * 
 * 
 * 	if(funcNodes->count("readKey") == 0){
		errs() << "readKey: " << "null" << '\n';
	} else {
		errs() << "readKey: " << funcNodes->at("readKey")->name << '\n';
		for(auto p: funcNodes->at("readKey")->parents){
			errs() << p->name << '\n';
		}
	}
	
	if(funcNodes->count("decrypt") != 0){
		errs() << "Size: " << funcNodes->at("decrypt")->children.size() << '\n';
	}
	
	
	
	
	
 * 
 * 
 */



