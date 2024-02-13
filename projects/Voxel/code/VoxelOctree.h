#pragma once

namespace SVO
{
	struct Node
	{
		bool isLeaf = false;
		Node* children[8];
		
	};

}