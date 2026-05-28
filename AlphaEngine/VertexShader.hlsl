[Shader("node")]
[NodeLaunch("broadcasting")]
[NodeDispatchGrid(1, 1, 1)]
[numthreads(32, 1, 1)] // ← これを追加！ (1グループあたりのスレッド数)
void TestNode()
{
    // SM 6.9 の機能を確認するためのダミー処理
}