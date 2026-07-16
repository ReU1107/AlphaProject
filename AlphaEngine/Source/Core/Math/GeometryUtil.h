#pragma once
#include "Vector.h"
#include "Core/Container/Array.h"

namespace Alpha
{
    void CreateUnitCube(TArray<Vector3>* positions, TArray<Vector3>* normals, TArray<Vector2>* texcoords, TArray<UInt32>* indices);

    void CreateUnitSphere(UInt32 stackCount, UInt32 sectorCount, TArray<Vector3>* positions, TArray<Vector3>* normals, TArray<Vector2>* texcoords, TArray<UInt32>* indices);


}