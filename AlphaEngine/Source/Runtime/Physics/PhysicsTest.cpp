
#define _SILENCE_CXX20_CISO646_REMOVED_WARNING
#include <PxPhysicsAPI.h>
#ifdef _DEBUG
#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#else
#pragma comment(lib, "PhysX_64.lib")
#pragma comment(lib, "PhysXCommon_64.lib")
#pragma comment(lib, "PhysXFoundation_64.lib")
#pragma comment(lib, "PhysXExtensions_static_64.lib")
#endif

using namespace physx;

namespace Alpha
{
	void A()
	{
		physx::PxDefaultAllocator m_defaultAllocator;
		// エラー時用のコールバックでエラー内容が入ってる
		physx::PxDefaultErrorCallback m_defaultErrorCallback;
		// 上位レベルのSDK(PxPhysicsなど)をインスタンス化する際に必要
		physx::PxFoundation* m_pFoundation = nullptr;
		// 実際に物理演算を行う
		physx::PxPhysics* m_pPhysics = nullptr;
		// シミュレーションをどう処理するかの設定でマルチスレッドの設定もできる
		physx::PxDefaultCpuDispatcher* m_pDispatcher = nullptr;
		// シミュレーションする空間の単位でActorの追加などもここで行う
		physx::PxScene* m_pScene = nullptr;
		// PVDと通信する際に必要
		physx::PxPvd* m_pPvd = nullptr;
		if (m_pFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, m_defaultAllocator, m_defaultErrorCallback); !m_pFoundation) {
			return;
		}
	}
}