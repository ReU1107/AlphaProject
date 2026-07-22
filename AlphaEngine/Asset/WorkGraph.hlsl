#include "Resource.hlsli"
struct TaskData
{
    uint instance_index;
};


ConstantBuffer<HeapIndices> heap_indices : register(b0);

[Shader("node")]
[NodeLaunch("broadcasting")]
[NodeDispatchGrid(1, 1, 1)]
[NumThreads(1, 1, 1)]
void EntryNode(
    uint tig : SV_GroupIndex,
    uint gid : SV_GroupId,
    DispatchNodeInputRecord< TaskData> InputQueue,
    NodeOutput<TaskData> RenderNode)
{

// カリングは一旦無視

    ThreadNodeOutputRecords<TaskData> output_record = RenderNode.GetThreadNodeOutputRecords(1);

output_record.Get().instance_index = gid;

    output_record.OutputComplete();
}
