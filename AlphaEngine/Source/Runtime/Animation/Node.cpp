#include "Node.h"
#include "Definitions.h"
#include "Clip.h"
#include "ClipInstance.h"

#include "Core/Math/MathUtil.h"

namespace Alpha::Animation
{
	void EntryNode::SetSource(Node* node)
	{
		m_source = node;
	}

	void EntryNode::Update(float delta_time)
	{
		if (m_source)
			m_source->Update(delta_time);
	}

	void EntryNode::Evaluate(float elapsed_time, Snapshot* out_pose)
	{
		if (m_source)
			m_source->Evaluate(elapsed_time, out_pose);
	}

	void EntryNode::Accept(IVisitor* visitor)
	{
		if (m_source)
			m_source->Accept(visitor);
	}

	BlendNode::BlendNode() = default;
	BlendNode::~BlendNode() = default;

	void BlendNode::SetAlpha(float alpha)
	{
		m_alpha = Clamp(alpha, 0.0f, 1.0f);
	}

	void BlendNode::SetInputs(Node* lhs, Node* rhs)
	{
	}

	void BlendNode::Update(float delta_time)
	{
		if (m_input_lhs)
			m_input_lhs->Update(delta_time);

		if (m_input_rhs)
			m_input_rhs->Update(delta_time);

	}

	void BlendNode::Evaluate(float elapsed_time, Snapshot* out_pose)
	{
		
		Snapshot lhs_pose = {};
		Snapshot rhs_pose = {};

		m_input_lhs->Evaluate(elapsed_time, &lhs_pose);
		m_input_rhs->Evaluate(elapsed_time, &rhs_pose);

		
		// lhs,rhsの補完
		out_pose->Blend(lhs_pose, rhs_pose, m_alpha);
	}

	void BlendNode::Accept(IVisitor* visitor)
	{
	}

	LeafNode::LeafNode() = default;
	LeafNode::~LeafNode() = default;

	void LeafNode::SetClip(TShared<Clip> clip)
	{
		if (clip)
		{
			m_clip_instance = clip->CreateInstance();
			m_clip_instance->SetClip(clip);
		}
	}

	void LeafNode::Update(float delta_time)
	{
		
	}

	void LeafNode::Evaluate(float elapsed_time, Snapshot* out_pose)
	{
		m_clip_instance->OnEvaluate(elapsed_time, out_pose);
	}

	void LeafNode::Accept(IVisitor* visitor)
	{
		if (visitor->BeginTreeNode(L"LeafNode"))
		{
			if (m_clip_instance)
				m_clip_instance->Accept(visitor);

			visitor->EndTreeNode();
		}
	}

	void FilterNode::Accept(IVisitor* visitor)
	{
	}

}