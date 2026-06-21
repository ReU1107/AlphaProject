#pragma once
#include "Core/Common.h"
#include "Core/Object.h"

namespace Alpha::Animation
{
	struct Snapshot;

	class Node : public Object
	{
	public:
		virtual ~Node() = default;

		// 時間の更新やパラメータの事前計算
		virtual void Update(float delta_time) = 0;

		// 生成（子ノードを再帰的に呼び出す）
		virtual void Evaluate(float elapsed_time, Snapshot* out_pose) = 0;
	};

	class EntryNode : public Node
	{
	private:
		Node* m_source{ nullptr };
	public:
		void SetSource(Node* node);

		// 時間の更新やパラメータの事前計算
		void Update(float delta_time) final;

		// 生成（子ノードを再帰的に呼び出す）
		void Evaluate(float elapsed_time, Snapshot* out_pose) final;

		void Accept(IVisitor* visitor) final;
	};

	class FilterNode : public Node
	{
	public:
		// 時間の更新やパラメータの事前計算
		void Update(float delta_time) final {}

		// 生成（子ノードを再帰的に呼び出す）
		void Evaluate(float elapsed_time, Snapshot* out_pose) final {}

		void Accept(IVisitor* visitor) final;
	};

	class BlendNode : public Node
	{
	private:
		TUnique<Node> m_input_lhs{ nullptr };
		TUnique<Node> m_input_rhs{ nullptr };
		float m_alpha{ 0.0f };
	public:
		BlendNode();
		~BlendNode();

		void SetAlpha(float alpha);

		// 時間の更新やパラメータの事前計算
		void Update(float delta_time) final;

		// 生成（子ノードを再帰的に呼び出す）
		void Evaluate(float elapsed_time, Snapshot* out_pose) final;

		void Accept(IVisitor* visitor) final;
	};

	class Clip;
	class ClipInstance;

	class LeafNode : public Node
	{
	private:
		TUnique<ClipInstance> m_clip_instance{ nullptr };

	public:
		LeafNode();
		~LeafNode();

		void SetClip(TShared<Clip> clip);

		ClipInstance* GetClipInstance() const { return m_clip_instance.get(); }

		// 時間の更新やパラメータの事前計算
		void Update(float delta_time) final;

		// 生成（子ノードを再帰的に呼び出す）
		void Evaluate(float elapsed_time, Snapshot* out_pose) final;

		void Accept(IVisitor* visitor) final;
	};

}