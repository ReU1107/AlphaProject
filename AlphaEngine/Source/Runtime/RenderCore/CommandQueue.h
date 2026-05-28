#pragma once
#include "Core/Common.h"
#include "RHI/Types.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class RHICommandQueue;

	class Timeline;
	class CommandList;

	class CommandQueue
	{
	private:
		TUnique<RHICommandQueue> m_rhi{ nullptr };

	public:
		// コマンドリスト
		CommandQueue();
		// デストラクタ
		~CommandQueue();

		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			ERHICommandType type{ ERHICommandType::Direct };
			// 追加の初期化パラメーターをここに記述できます
		};

		void Initialize(const Args& args);

		RHICommandQueue* GetRHI() const;

		void Execute(CommandList* command_list);

		void Execute(CommandList** command_lists, UInt32 count);

		// 
		UInt64 Signal(Timeline* timeline);

		void Wait(Timeline* timeline);
		void Wait(Timeline* timeline, UInt64 wait_value);

	};
}
/*
* SignalとWaitの関係
* SignalはGPU側での処理完了を示すためのもので、Waitはその完了を待つためのものです。
* CommandQueueがSignalを発行すると、その時点でのGPUの処理が完了したことを示す信号が発行されます。
* Timelineはこの信号を受け取り、GPUの処理が完了したことを認識します。
* 一方、CommandQueueがWaitを呼び出すと、その時点でTimelineが保持している信号を待機します。
* これにより、GPUの処理が完了するまでCommandQueueの処理が停止します。
* この仕組みにより、CPUとGPUの処理を適切に同期させることができます。
*/