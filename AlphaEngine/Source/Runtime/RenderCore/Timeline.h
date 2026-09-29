#pragma once
#include "Core/Common.h"

namespace Alpha
{
	class RenderHardwareInterface;
	class RHITimeline;

	class Timeline
	{
	private:
		TUnique<RHITimeline> m_rhi{ nullptr };
		// 各フレーム（バックバッファ）が最後に発行したフェンス値を記録する
		TArray<UInt64> m_frame_values;

		// 現在GPUに「これから発行する」最先端のグローバルなフェンス値
		UInt64 m_global_value{ 0 };

		UInt32 m_current_buffer_index{ 0 };

	public:
		Timeline();
		~Timeline();
		struct Args
		{
			RenderHardwareInterface* rhi{ nullptr };
			UInt32 buffer_num{ 3 };
		};

		void Initialize(const Args& args);
		RHITimeline* GetRHI() const;

		UInt64 GetNextValue();
		UInt64 GetCurrentValue() const;

		void WaitCPUCurrentFrame(UInt32 next_swapchain_index);

		void WaitCPU(UInt64 value);
		void WaitCPULatest();


	};

}