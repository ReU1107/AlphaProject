#include "D3D12Shader.h"

#include "RHI/Definitions.h"

#include "Core/Assertion.h"

#pragma comment(lib, "dxcompiler.lib")

#if RHI_D3D12

namespace
{
	inline LPCWSTR GetShaderTarget(Alpha::ERHIShaderType stage)
	{
		switch (stage)
		{
		case Alpha::ERHIShaderType::Vertex			: return L"vs_";
		case Alpha::ERHIShaderType::Pixel			: return L"ps_";
		case Alpha::ERHIShaderType::Geometry		: return L"gs_";
		case Alpha::ERHIShaderType::Hull			: return L"hs_";
		case Alpha::ERHIShaderType::Amplification	: return L"as_";
		case Alpha::ERHIShaderType::Mesh			: return L"ms_";
		case Alpha::ERHIShaderType::Compute			: return L"cs_";
		case Alpha::ERHIShaderType::RayTracing		: return L"lib_";
		case Alpha::ERHIShaderType::Library			: return L"lib_";
		default:
			break;
		}

		return L"";
	}

	bool CompileShader(const Alpha::RHIShaderDesc& desc, IDxcBlob** out_blob)
	{
		HRESULT hr = S_OK;
		IDxcCompiler3* compiler = nullptr;
		IDxcLibrary* library = nullptr;
		IDxcUtils* utils = nullptr;
		IDxcBlobEncoding* source_blob = nullptr;
		
		wchar_t exePath[MAX_PATH];
		GetModuleFileNameW(NULL, exePath, MAX_PATH);
		std::wstring dir = exePath;
		dir = dir.substr(0, dir.find_last_of(L"\\/"));

		std::wstring dllPath = dir + L"\\dxcompiler.dll";

		HMODULE hDxc = LoadLibraryExW(dllPath.c_str(), NULL, LOAD_WITH_ALTERED_SEARCH_PATH);
		if (!hDxc) {
			ALPHA_LOG_ERROR(L"DLL not found at: {}", dllPath.c_str());
			return false;
		}

		DxcCreateInstanceProc pDxcCreateInstance =
			(DxcCreateInstanceProc)GetProcAddress(hDxc, "DxcCreateInstance");


		hr = pDxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&compiler));
		if (FAILED(hr))
			return false;

		hr = pDxcCreateInstance(CLSID_DxcLibrary, IID_PPV_ARGS(&library));
		if (FAILED(hr))
			return false;

		hr = pDxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&utils));
		if (FAILED(hr))
			return false;

		IDxcIncludeHandler* include_handler = nullptr;
		utils->CreateDefaultIncludeHandler(&include_handler);


		hr = library->CreateBlobFromFile(desc.file_directory.c_str(), nullptr, &source_blob);
		if (FAILED(hr))
			return false;

		std::vector<LPCWSTR> arguments = {};
		arguments.push_back(desc.file_directory.c_str());					// シェーダーファイル


		if (desc.type != Alpha::ERHIShaderType::Library)
		{
			arguments.push_back(L"-E");
			arguments.push_back(desc.entry_point.c_str());				// エントリーポイント
		}

		std::wstring target = GetShaderTarget(desc.type);
		target += L"6_9";	// シェーダータイプ + シェーダーモデル

		arguments.push_back(L"-T");						 
		arguments.push_back(target.c_str());	// シェーダータイプ + モデル

		arguments.push_back(L"-O3");
		arguments.push_back(L"-HV");
		arguments.push_back(L"2021"); // これを追加

		if (desc.enable_debug)
		{
			arguments.push_back(L"-Zi");					// デバッグ情報を有効化
			arguments.push_back(L"-Qembed_debug");			// デバッグ情報を埋め込む
		}

		if (!desc.enable_optimize)
		{
			arguments.push_back(L"-Od");					// 最適化を無効化
		}

		for (auto path : desc.includes)
		{

		}

		arguments.push_back(L"-I");
		arguments.push_back(L"Asset");


		DxcBuffer source_buffer = {};
		source_buffer.Ptr = source_blob->GetBufferPointer();
		source_buffer.Size = source_blob->GetBufferSize();
		source_buffer.Encoding = DXC_CP_UTF8;

		IDxcResult* result = nullptr;
		hr = compiler->Compile(&source_buffer, arguments.data(), (UINT32)arguments.size(), include_handler, IID_PPV_ARGS(&result));
		HRESULT hr_status;
		result->GetStatus(&hr_status);
		if (FAILED(hr_status)) {
			// コンパイルエラーがある場合、エラーメッセージを取得して出力
			IDxcBlobUtf8* error_blob = nullptr;
			result->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&error_blob), nullptr);
			if (error_blob && error_blob->GetStringLength() > 0) 
			{
				const char* utf8Str = error_blob->GetStringPointer();
				int utf8Len = static_cast<int>(error_blob->GetStringLength());

				int wlen = MultiByteToWideChar(CP_UTF8, 0, utf8Str, utf8Len, nullptr, 0);

				std::wstring wErrorMsg;
				if (wlen > 0) {
					wErrorMsg.resize(wlen);
					MultiByteToWideChar(CP_UTF8, 0, utf8Str, utf8Len, &wErrorMsg[0], wlen);
				}

				ALPHA_LOG_ERROR(L"Shader Compile Error:\n{}", wErrorMsg.c_str());

				error_blob->Release();

				// 日本語が含まれるとエラーになる
				//ALPHA_LOG_ERROR(L"Shader Compile Error:\n{}", Alpha::Text(error_blob->GetStringPointer()).c_str());
				//error_blob->Release();
			}
			return false; // ここで失敗を返すようにする
		}

		result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(out_blob), nullptr);

		include_handler->Release();
		source_blob->Release();
		library->Release();
		compiler->Release();
		result->Release();

		return true;
	}

}

namespace Alpha
{
	D3D12Shader::D3D12Shader(D3D12Device* device)
		: RHIShader()
		, D3D12DeviceChild(device)
	{
	}

	D3D12Shader::~D3D12Shader()
	{
	}

	bool D3D12Shader::Initialize(const RHIShaderDesc& desc)
	{
		IDxcBlob* shader_blob = nullptr;

		if (!CompileShader(desc, &shader_blob))
		{
			return false;
		}

		m_shader_blob = shader_blob;
		m_entry_point = desc.entry_point;
		return true;
	}
}
#endif