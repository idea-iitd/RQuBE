#include "Node.h"

Node::Node(int _NodeId)
{
	nodeId = _NodeId;
	labels.push_back(-1);
}

void Node::addEdge(int _destId, bool _direction, int _label = 0)
{
	if (_direction)
	{
		fwd_labelled_edges[_label].push_back(_destId);
		numFwdEdges[_label] += 1;
		fwdEdge[_destId]=_label;
		allFwdEdges.push_back(_destId);
	}
	else
	{
		bwd_labelled_edges[_label].push_back(_destId);
		numBwdEdges[_label] += 1;
		bwdEdge[_destId]=_label;
		allBwdEdges.push_back(_destId);
	}
}

void Node::add_label(int l, int sort = 0) 
{
	labels.push_back(l);
	if (sort) sort_labels();
}

void Node::sort_labels()
{
	sort(labels.begin(),labels.end());
}

void Node::add_attr(string s, int value)  
{
	attributes[s] = value;
}
