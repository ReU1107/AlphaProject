

struct Particle
{
    float3 pos;
    float3 vel;
};

struct SpawnCountRecord
{
    uint count;
};

struct MyData
{
    uint value;
};

[Shader("node")]
[NodeId("MyParentNode")] // ← これが C++ の getProgramIdentifier(L"MyParentNode") と一致
[NodeIsProgramEntry] // ← これが DispatchGraph を受け付ける印
[NodeLaunch("broadcasting")]
[NodeDispatchGrid(1, 1, 1)]
[NumThreads(1, 1, 1)]
void MyFunc(DispatchNodeInputRecord< MyData> inputData) // NodeIdがあれば関数名は何でもOK
{
RWStructuredBuffer<MyData> myBuffer = ResourceDescriptorHeap[0];
myBuffer[0].value = inputData.Get().
value;
}


//[Shader("node")]
//[NodeIsProgramEntry]
//[NodeId("MyParentNode")]
//[NodeLaunch("thread")] // ← 起動タイプを明示
//void Entry(
//    [MaxRecords(1)] [NodeId("Emitter")] 
//    NodeOutput<SpawnCountRecord> outputRecord
//) {

//    //// スレッド単位のレコード取得
//    //ThreadNodeOutputRecords<SpawnCountRecord> outRecs = outputRecord.GetThreadNodeOutputRecords(1);
//    //outRecs.Get().count = 10;
//    //outRecs.OutputComplete();
//}

//[Shader("node")]
//[NodeId("Emitter")]
//[NodeLaunch("thread")]
//void Emitter(
//    ThreadNodeInputRecord< SpawnCountRecord> input,
//    [MaxRecords(4)] [NodeId("Update")] 
//    NodeOutput<Particle> updateOutput // ← 名前を変更
//) {
//    // 最小限のテストコード（中身は空でもOK）
//}

//[Shader("node")]
//[NodeId("Update")] // これが必要
//[NodeLaunch("thread")]
//void Update(
//    ThreadNodeInputRecord< Particle> input
//) 
//{
//    // 粒子の更新処理などをここに記述
//}