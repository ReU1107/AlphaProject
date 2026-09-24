#include "Graph.h"
#include "Node.h"

namespace Alpha::Animation
{
	Graph::Graph()
	{
		m_entry_node = std::make_unique<EntryNode>();
	}

	Graph::~Graph() = default;

	void Graph::Update(float delta_time)
	{
		m_entry_node->Update(delta_time);
	}

	void Graph::Evaluate(float time, Snapshot* pose)
	{
		m_entry_node->Evaluate(time, pose);
	}

	LeafNode* Graph::MakeLeafNode()
	{
		m_nodes.push_back(std::make_unique<LeafNode>());
		return static_cast<LeafNode*>(m_nodes.back().get());
	}

	BlendNode* Graph::MakeBlendNode()
	{
		m_nodes.push_back(std::make_unique<BlendNode>());
		return static_cast<BlendNode*>(m_nodes.back().get());
	}

	void Graph::SetEntry(UInt32 index)
	{
		if (index >= m_nodes.size())
			return;
		m_entry_node->SetSource(m_nodes[index].get());
	}

	void Graph::Accept(IVisitor* visitor)
	{
		if (visitor->BeginTreeNode(L"Graph"))
		{
			if (visitor->BeginTreeNode(L"Nodes"))
			{
				for (const auto& node : m_nodes)
					node->Accept(visitor);

				visitor->EndTreeNode();
			}

			visitor->EndTreeNode();
		}
	}
}