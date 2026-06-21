#pragma once
#include "Core/Common.h"
#include "Core/Object.h"

namespace Alpha::Animation
{
	class Node;
	class EntryNode;
	class LeafNode;
	class BlendNode;
	struct Snapshot;

	class Graph : public Object
	{
	private:
		TUnique<EntryNode> m_entry_node{};
		TUniqueArray<Node> m_nodes{};

	public:
		Graph();
		~Graph();

		// システム側が呼ぶ関数群
		void Update(float delta_time);

		void Evaluate(float time, Snapshot* pose);

		LeafNode* MakeLeafNode();

		BlendNode* MakeBlendNode();

		void SetEntry(UInt32 index);

		void Accept(IVisitor* visitor) override;

	};

}