#include "GUISystem.h"
#include "RenderCore/Module.h"

#include "RenderContext.h"

#include "Core/Math/MathF.h"
#include "Core/Window/WindowSystem.h"

#include <imgui/imgui.h>
#include <windowsx.h>

namespace
{
	ImGuiKey VirtualKeyToImGuiKey(WPARAM wParam) 
	{
		switch (wParam) 
		{
		case VK_TAB: return ImGuiKey_Tab;
		case VK_LEFT: return ImGuiKey_LeftArrow;
		case VK_RIGHT: return ImGuiKey_RightArrow;
		case VK_UP: return ImGuiKey_UpArrow;
		case VK_DOWN: return ImGuiKey_DownArrow;
		case VK_PRIOR: return ImGuiKey_PageUp;
		case VK_NEXT: return ImGuiKey_PageDown;
		case VK_HOME: return ImGuiKey_Home;
		case VK_END: return ImGuiKey_End;
		case VK_INSERT: return ImGuiKey_Insert;
		case VK_DELETE: return ImGuiKey_Delete;
		case VK_BACK: return ImGuiKey_Backspace;
		case VK_SPACE: return ImGuiKey_Space;
		case VK_RETURN: return ImGuiKey_Enter;
		case VK_ESCAPE: return ImGuiKey_Escape;
		case VK_OEM_7: return ImGuiKey_Apostrophe;
		case VK_OEM_COMMA: return ImGuiKey_Comma;
		case VK_OEM_MINUS: return ImGuiKey_Minus;
		case VK_OEM_PERIOD: return ImGuiKey_Period;
		case VK_OEM_2: return ImGuiKey_Slash;
		case VK_OEM_1: return ImGuiKey_Semicolon;
		case VK_OEM_PLUS: return ImGuiKey_Equal;
		case VK_OEM_4: return ImGuiKey_LeftBracket;
		case VK_OEM_5: return ImGuiKey_Backslash;
		case VK_OEM_6: return ImGuiKey_RightBracket;
		case VK_OEM_3: return ImGuiKey_GraveAccent;
		case VK_CAPITAL: return ImGuiKey_CapsLock;
		case VK_SCROLL: return ImGuiKey_ScrollLock;
		case VK_NUMLOCK: return ImGuiKey_NumLock;
		case VK_SNAPSHOT: return ImGuiKey_PrintScreen;
		case VK_PAUSE: return ImGuiKey_Pause;
		case VK_NUMPAD0: return ImGuiKey_Keypad0;
		case VK_NUMPAD1: return ImGuiKey_Keypad1;
		case VK_NUMPAD2: return ImGuiKey_Keypad2;
		case VK_NUMPAD3: return ImGuiKey_Keypad3;
		case VK_NUMPAD4: return ImGuiKey_Keypad4;
		case VK_NUMPAD5: return ImGuiKey_Keypad5;
		case VK_NUMPAD6: return ImGuiKey_Keypad6;
		case VK_NUMPAD7: return ImGuiKey_Keypad7;
		case VK_NUMPAD8: return ImGuiKey_Keypad8;
		case VK_NUMPAD9: return ImGuiKey_Keypad9;
		case VK_DECIMAL: return ImGuiKey_KeypadDecimal;
		case VK_DIVIDE: return ImGuiKey_KeypadDivide;
		case VK_MULTIPLY: return ImGuiKey_KeypadMultiply;
		case VK_SUBTRACT: return ImGuiKey_KeypadSubtract;
		case VK_ADD: return ImGuiKey_KeypadAdd;
		case VK_LSHIFT: return ImGuiKey_LeftShift;
		case VK_RSHIFT: return ImGuiKey_RightShift;
		case VK_LCONTROL: return ImGuiKey_LeftCtrl;
		case VK_RCONTROL: return ImGuiKey_RightCtrl;
		case VK_LMENU: return ImGuiKey_LeftAlt;
		case VK_RMENU: return ImGuiKey_RightAlt;
		case VK_LWIN: return ImGuiKey_LeftSuper;
		case VK_RWIN: return ImGuiKey_RightSuper;
		case '0': return ImGuiKey_0;
		case '1': return ImGuiKey_1;
		case '2': return ImGuiKey_2;
		case '3': return ImGuiKey_3;
		case '4': return ImGuiKey_4;
		case '5': return ImGuiKey_5;
		case '6': return ImGuiKey_6;
		case '7': return ImGuiKey_7;
		case '8': return ImGuiKey_8;
		case '9': return ImGuiKey_9;
		case 'A': return ImGuiKey_A;
		case 'B': return ImGuiKey_B;
		case 'C': return ImGuiKey_C;
		case 'D': return ImGuiKey_D;
		case 'E': return ImGuiKey_E;
		case 'F': return ImGuiKey_F;
		case 'G': return ImGuiKey_G;
		case 'H': return ImGuiKey_H;
		case 'I': return ImGuiKey_I;
		case 'J': return ImGuiKey_J;
		case 'K': return ImGuiKey_K;
		case 'L': return ImGuiKey_L;
		case 'M': return ImGuiKey_M;
		case 'N': return ImGuiKey_N;
		case 'O': return ImGuiKey_O;
		case 'P': return ImGuiKey_P;
		case 'Q': return ImGuiKey_Q;
		case 'R': return ImGuiKey_R;
		case 'S': return ImGuiKey_S;
		case 'T': return ImGuiKey_T;
		case 'U': return ImGuiKey_U;
		case 'V': return ImGuiKey_V;
		case 'W': return ImGuiKey_W;
		case 'X': return ImGuiKey_X;
		case 'Y': return ImGuiKey_Y;
		case 'Z': return ImGuiKey_Z;
		case VK_F1: return ImGuiKey_F1;
		case VK_F2: return ImGuiKey_F2;
		case VK_F3: return ImGuiKey_F3;
		case VK_F4: return ImGuiKey_F4;
		case VK_F5: return ImGuiKey_F5;
		case VK_F6: return ImGuiKey_F6;
		case VK_F7: return ImGuiKey_F7;
		case VK_F8: return ImGuiKey_F8;
		case VK_F9: return ImGuiKey_F9;
		case VK_F10: return ImGuiKey_F10;
		case VK_F11: return ImGuiKey_F11;
		case VK_F12: return ImGuiKey_F12;
		default: return ImGuiKey_None;
		}
	}
}

namespace Alpha
{
	GUISystem::GUISystem()
	{
	}

	GUISystem::~GUISystem()
	{
	}

	bool GUISystem::Initialize(const Args& args)
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		auto& io = ImGui::GetIO();

		io.BackendRendererName = "";
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
		io.Fonts->AddFontFromFileTTF("Asset/Font/Noto_Serif_JP/NotoSerifJP-VariableFont_wght.ttf", 18.0f, nullptr, ImGui::GetIO().Fonts->GetGlyphRangesJapanese());
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);

		m_font_texture = args.render_core->CreateTexture(width, height);

		args.render_core->UploadTexture(m_font_texture.get(), pixels);

		io.Fonts->SetTexID(m_font_texture.get());

		m_vertex_buffer = std::make_unique<TransientBufferAllocator>();
		{
			TransientBufferAllocator::Args desc = {
				.render_core = args.render_core,
				.stride = sizeof(ImDrawVert),
				.size = sizeof(ImDrawVert) * args.max_vertex_num,
				.alignment = 4,
			};
			m_vertex_buffer->Initialize(desc);
		}

		m_index_buffer = std::make_unique<TransientBufferAllocator>();
		{
			TransientBufferAllocator::Args desc = {
				.render_core = args.render_core,
				.stride = sizeof(ImDrawIdx),
				.size = sizeof(ImDrawIdx) * args.max_vertex_num * 6,
				.alignment = 4,
			};
			m_index_buffer->Initialize(desc);
		}

		m_vs = RenderCore::Get().CreateShader(L"Asset/GUIVS.hlsl", ERHIShaderType::Vertex);
		m_ps = RenderCore::Get().CreateShader(L"Asset/GUIPS.hlsl", ERHIShaderType::Pixel);

		m_pipeline_layout = RenderCore::Get().CreatePipelineLayout(
			[](PipelineLayout& layout)
			{
				layout.AddRootCBV(1, ERHIShaderFlags::Vertex);
				layout.AddSampler(1, ERHIShaderFlags::Pixel);

				layout.BeginDescriptorTable();
				layout.AddDescriptorSRV(1, ERHIShaderFlags::Pixel);
				layout.EndDescriptorTable();
				layout.SetFlags(ERHIPipelineLayoutFlags::InputAssembler);
			}
		);

		m_pipeline = RenderCore::Get().CreateGraphicsPipeline(
			{
				.vertex_layout = VertexLayout()
					.AddElement(L"POSITION", EVertexElementType::Float2, 0)
					.AddElement(L"TEXCOORD", EVertexElementType::Float2, 0)
					.AddElement(L"COLOR", EVertexElementType::Color, 0),
				.pipeline_layout = m_pipeline_layout.get(),
				.vertex_shader = m_vs.get(),
				.pixel_shader = m_ps.get(),
				.render_target_count = 1,
				.render_targets = {
					RenderCore::Get().GetBackBufferFormat()
				},
				.blend_states = {
					DefaultAlphaBlend()
				},
				.cull_mode = ERHICullMode::None,
	
			}
		);
	
		WindowSystem::Get().SetEventCallback([&](uint32_t msg, intptr_t wParam, intptr_t lParam)
			{
				auto& io = ImGui::GetIO();

				switch (msg)
				{
				case WM_MOUSEMOVE:
				{
					float x = (float)GET_X_LPARAM(lParam);
					float y = (float)GET_Y_LPARAM(lParam);
					io.AddMousePosEvent(x, y);

					return 0;
				}
				case WM_LBUTTONDOWN: case WM_LBUTTONDBLCLK:
				case WM_RBUTTONDOWN: case WM_RBUTTONDBLCLK:
				case WM_MBUTTONDOWN: case WM_MBUTTONDBLCLK:
				case WM_LBUTTONUP:
				case WM_RBUTTONUP:
				case WM_MBUTTONUP:
				{
					int button = 0;
					if (msg == WM_LBUTTONDOWN || msg == WM_LBUTTONDBLCLK || msg == WM_LBUTTONUP) button = 0;
					if (msg == WM_RBUTTONDOWN || msg == WM_RBUTTONDBLCLK || msg == WM_RBUTTONUP) button = 1;
					if (msg == WM_MBUTTONDOWN || msg == WM_MBUTTONDBLCLK || msg == WM_MBUTTONUP) button = 2;

					bool is_down = (msg == WM_LBUTTONDOWN || msg == WM_RBUTTONDOWN || msg == WM_MBUTTONDOWN ||
						msg == WM_LBUTTONDBLCLK || msg == WM_RBUTTONDBLCLK || msg == WM_MBUTTONDBLCLK);

					// ボタン状態をイベントとして追加
					io.AddMouseButtonEvent(button, is_down);
					return 0;
				}
				case WM_MOUSEWHEEL:
				{
					// 垂直ホイール (120単位を1.0に変換)
					float wheel = (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
					io.AddMouseWheelEvent(0.0f, wheel);
					return 0;
				}
				case WM_MOUSEHWHEEL:
				{
					// 水平ホイール
					float wheel = (float)GET_WHEEL_DELTA_WPARAM(wParam) / (float)WHEEL_DELTA;
					io.AddMouseWheelEvent(-wheel, 0.0f);
					return 0;
				}
				case WM_CHAR:
					if (wParam > 0 && wParam < 0x10000)
						io.AddInputCharacter((unsigned short)wParam);
					return 0;

				case WM_KEYDOWN: 
				case WM_KEYUP: 
				case WM_SYSKEYUP:
				case WM_SYSKEYDOWN:
					const bool is_down = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
					ImGuiKey key = VirtualKeyToImGuiKey(wParam);
					if (key != ImGuiKey_None)
					{
						// 配列への書き込みではなく、関数を呼ぶ
						io.AddKeyEvent(key, is_down);
					}
					return 0;
				}

				return 0;
			}
		);


		return true;
	}

	void GUISystem::Finalize()
	{
		ImGui::DestroyContext();

		m_pipeline.reset();
		m_pipeline_layout.reset();
		m_vs.reset();
		m_ps.reset();
		m_vertex_buffer.reset();
		m_index_buffer.reset();
		m_font_texture.reset();
	}

	void GUISystem::BeginFrame()
	{
		auto& io = ImGui::GetIO();

		io.DisplaySize = ImVec2((float)1920, (float)1080);

		ImGui::NewFrame();
	}

	void GUISystem::EndFrame()
	{
		ImGui::Render();
	}

	void GUISystem::Flush(RenderContext& ctx)
	{
		auto draw_data = ImGui::GetDrawData();
		
		if (!draw_data) 
			return;

		m_vertex_buffer->Reset();
		m_index_buffer->Reset();

		ctx.command_list->SetGraphicsPipeline(m_pipeline.get());
		ctx.command_list->SetDescriptorHeap(RenderCore::Get().GetResourceHeap(), RenderCore::Get().GetSamplerHeap());


		float L = draw_data->DisplayPos.x;
		float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
		float T = draw_data->DisplayPos.y;
		float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;

		// プロジェクション行列
		Matrix4x4 projection = Matrix4x4OrthographicLH(L, R, T, B, 0.0f, 1.0f);

		auto constant_offset = ctx.transient_buffer->WriteData<matrix4x4>(&projection);
		ctx.command_list->SetGraphicsCBV(ctx.transient_buffer->GetBuffer(), constant_offset, 0);

		auto* sampler_sate = RenderCore::Get().GetPresetSampler(ESamplerPresetType::LinearClamp);

		ctx.command_list->SetGraphicsSamplerState(sampler_sate, 1);

		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{
			const ImDrawList* cmd_list = draw_data->CmdLists[n];

			uint64_t vertex_offset = 0;
			uint64_t vertex_byte_size = cmd_list->VtxBuffer.Size * sizeof(ImDrawVert);
			auto vertex_ptr = m_vertex_buffer->Allocate(vertex_byte_size, &vertex_offset);
			memcpy(vertex_ptr, cmd_list->VtxBuffer.Data, vertex_byte_size);

			uint64_t index_offset = 0;
			uint64_t index_byte_size = cmd_list->IdxBuffer.Size * sizeof(ImDrawIdx);
			auto index_ptr = m_index_buffer->Allocate(index_byte_size, &index_offset);
			memcpy(index_ptr, cmd_list->IdxBuffer.Data, index_byte_size);

			ImVec2 clip_off = draw_data->DisplayPos;

			for (int i = 0; i < cmd_list->CmdBuffer.Size; i++)
			{
				const ImDrawCmd* cmd = &cmd_list->CmdBuffer[i];
				ImTextureID tex_id = cmd->GetTexID();
				auto tex = (Texture*)(uintptr_t)tex_id;

				ctx.command_list->SetGraphicsTexture(tex, 2);

				ctx.command_list->SetVertexBuffer(m_vertex_buffer->GetBuffer(), 0);
				ctx.command_list->SetIndexBuffer(m_index_buffer->GetBuffer(), ERHIIndexFormat::UInt16);

				float clip_min_x = cmd->ClipRect.x - clip_off.x;
				float clip_min_y = cmd->ClipRect.y - clip_off.y;
				float clip_max_x = cmd->ClipRect.z - clip_off.x;
				float clip_max_y = cmd->ClipRect.w - clip_off.y;

				ctx.command_list->SetScissorRect(clip_min_x, clip_min_y, clip_max_x, clip_max_y);

				ctx.command_list->DrawIndexed(cmd->ElemCount, (index_offset / sizeof(ImDrawIdx)) + cmd->IdxOffset, (vertex_offset / sizeof(ImDrawVert)) + cmd->VtxOffset);
			}
		}

	}
}