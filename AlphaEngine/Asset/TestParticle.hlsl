struct MyData
{
    uint emitt_num;
};

struct ParticleRecord
{
    float3 position;

}



[Shader("node")]
[NodeIsProgramEntry]
[NodeLaunch("broadcasting")]
[NodeDispatchGrid(1, 1, 1)]
[NumThreads(1, 1, 1)]
void Emitter(DispatchNodeInputRecord<MyData> inputData,
[NodeId("Update")]
NodeOutput<ParticleRecord> particleOutput)
{
    ThreadNodeOutputRecords<ParticleRecord> outputRecord = particleOutput.GetThreadNodeOutputRecords(1);
    outputRecord.OutputComplete();
}

[Shader("node")]
[NodeLaunch("thread")]
void Update(ThreadNodeInputRecord<ParticleRecord> input)
{
    ParticleRecord record = input.Get();
    record.position += float3(0.0f,0.1f,0.0f);
    RWStructuredBuffer<ParticleRecord> buffer = ResourceDescriptorHeap[0];

    buffer[0] = record;
}