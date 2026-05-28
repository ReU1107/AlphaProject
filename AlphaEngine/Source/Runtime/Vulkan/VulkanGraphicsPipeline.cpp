#include "VulkanGraphicsPipeline.h"
#include <vector>
#include <array>
#if RHI_VK

namespace Alpha
{
	VulkanGraphicsPipeline::VulkanGraphicsPipeline(VulkanDevice* device)
		: VulkanDeviceChild(device)
	{

	}
	void VulkanGraphicsPipeline::Create()
	{
		//VkPipelineRenderingCreateInfoKHR dynamic_rendering_info = {};
		//dynamic_rendering_info.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO_KHR;
		//dynamic_rendering_info.colorAttachmentCount = 1;
		//dynamic_rendering_info.pColorAttachmentFormats = &swapChain.colorFormat;
		////dynamic_rendering_info.depthAttachmentFormat = depthFormat;
		////dynamic_rendering_info.stencilAttachmentFormat = depthFormat;
		//// Chain into the pipeline creat einfo

		//VkGraphicsPipelineCreateInfo pipelineCI{};
		//pipelineCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		//// The layout used for this pipeline (can be shared among multiple pipelines using the same layout)
		//pipelineCI.layout = pipelineLayout;
		//pipelineCI.pNext = &dynamic_rendering_info;


		//VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateCI{};
		//inputAssemblyStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		//inputAssemblyStateCI.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

		//// Rasterization state
		//VkPipelineRasterizationStateCreateInfo rasterizationStateCI{};
		//rasterizationStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		//rasterizationStateCI.polygonMode = VK_POLYGON_MODE_FILL;
		//rasterizationStateCI.cullMode = VK_CULL_MODE_NONE;
		//rasterizationStateCI.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		//rasterizationStateCI.depthClampEnable = VK_FALSE;
		//rasterizationStateCI.rasterizerDiscardEnable = VK_FALSE;
		//rasterizationStateCI.depthBiasEnable = VK_FALSE;
		//rasterizationStateCI.lineWidth = 1.0f;

		//// Color blend state describes how blend factors are calculated (if used)
		//// We need one blend attachment state per color attachment (even if blending is not used)
		//VkPipelineColorBlendAttachmentState blendAttachmentState{};
		//blendAttachmentState.colorWriteMask = 0xf;
		//blendAttachmentState.blendEnable = VK_FALSE;
		//VkPipelineColorBlendStateCreateInfo colorBlendStateCI{};
		//colorBlendStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		//colorBlendStateCI.attachmentCount = 1;
		//colorBlendStateCI.pAttachments = &blendAttachmentState;

		//// Viewport state sets the number of viewports and scissor used in this pipeline
		//// Note: This is actually overridden by the dynamic states (see below)
		//VkPipelineViewportStateCreateInfo viewportStateCI{};
		//viewportStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		//viewportStateCI.viewportCount = 1;
		//viewportStateCI.scissorCount = 1;

		//std::vector<VkDynamicState> dynamicStateEnables;
		//dynamicStateEnables.push_back(VK_DYNAMIC_STATE_VIEWPORT);
		//dynamicStateEnables.push_back(VK_DYNAMIC_STATE_SCISSOR);
		//VkPipelineDynamicStateCreateInfo dynamicStateCI{};
		//dynamicStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		//dynamicStateCI.pDynamicStates = dynamicStateEnables.data();
		//dynamicStateCI.dynamicStateCount = static_cast<uint32_t>(dynamicStateEnables.size());


		//VkPipelineDepthStencilStateCreateInfo depthStencilStateCI{};
		//depthStencilStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		//depthStencilStateCI.depthTestEnable = VK_FALSE;
		//depthStencilStateCI.depthWriteEnable = VK_FALSE;
		//depthStencilStateCI.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		//depthStencilStateCI.depthBoundsTestEnable = VK_FALSE;
		//depthStencilStateCI.back.failOp = VK_STENCIL_OP_KEEP;
		//depthStencilStateCI.back.passOp = VK_STENCIL_OP_KEEP;
		//depthStencilStateCI.back.compareOp = VK_COMPARE_OP_ALWAYS;
		//depthStencilStateCI.stencilTestEnable = VK_FALSE;
		//depthStencilStateCI.front = depthStencilStateCI.back;

		//VkPipelineMultisampleStateCreateInfo multisampleStateCI{};
		//multisampleStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		//multisampleStateCI.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
		//multisampleStateCI.pSampleMask = nullptr;

		//// Vertex input descriptions
		//// Specifies the vertex input parameters for a pipeline

		//// Vertex input binding
		//// This example uses a single vertex input binding at binding point 0 (see vkCmdBindVertexBuffers)
		//VkVertexInputBindingDescription vertexInputBinding{};
		//vertexInputBinding.binding = 0;
		//vertexInputBinding.stride = sizeof(Vertex);
		//vertexInputBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		//// Input attribute bindings describe shader attribute locations and memory layouts
		//std::array<VkVertexInputAttributeDescription, 2> vertexInputAttributs;
		//// These match the following shader layout (see triangle.vert):
		////	layout (location = 0) in vec3 inPos;
		////	layout (location = 1) in vec3 inColor;
		//// Attribute location 0: Position
		//vertexInputAttributs[0].binding = 0;
		//vertexInputAttributs[0].location = 0;
		//// Position attribute is three 32 bit signed (SFLOAT) floats (R32 G32 B32)
		//vertexInputAttributs[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		//vertexInputAttributs[0].offset = offsetof(Vertex, position);
		//// Attribute location 1: Color
		//vertexInputAttributs[1].binding = 0;
		//vertexInputAttributs[1].location = 1;
		//// Color attribute is three 32 bit signed (SFLOAT) floats (R32 G32 B32)
		//vertexInputAttributs[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		//vertexInputAttributs[1].offset = offsetof(Vertex, color);

		//// Vertex input state used for pipeline creation
		//VkPipelineVertexInputStateCreateInfo vertexInputStateCI{};
		//vertexInputStateCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		//vertexInputStateCI.vertexBindingDescriptionCount = 1;
		//vertexInputStateCI.pVertexBindingDescriptions = &vertexInputBinding;
		//vertexInputStateCI.vertexAttributeDescriptionCount = 2;
		//vertexInputStateCI.pVertexAttributeDescriptions = vertexInputAttributs.data();

		//// Shaders
		//std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

		//// Vertex shader
		//shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		//// Set pipeline stage for this shader
		//shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		//// Load binary SPIR-V shader
		//shaderStages[0].module = loadSPIRVShader(getShadersPath() + "triangle/triangle.vert.spv");
		//// Main entry point for the shader
		//shaderStages[0].pName = "main";
		//assert(shaderStages[0].module != VK_NULL_HANDLE);

		//// Fragment shader
		//shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		//// Set pipeline stage for this shader
		//shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		//// Load binary SPIR-V shader
		//shaderStages[1].module = loadSPIRVShader(getShadersPath() + "triangle/triangle.frag.spv");
		//// Main entry point for the shader
		//shaderStages[1].pName = "main";
		//assert(shaderStages[1].module != VK_NULL_HANDLE);

		//// Set pipeline shader stage info
		//pipelineCI.stageCount = static_cast<uint32_t>(shaderStages.size());
		//pipelineCI.pStages = shaderStages.data();

		//// Assign the pipeline states to the pipeline creation info structure
		//pipelineCI.pVertexInputState = &vertexInputStateCI;
		//pipelineCI.pInputAssemblyState = &inputAssemblyStateCI;
		//pipelineCI.pRasterizationState = &rasterizationStateCI;
		//pipelineCI.pColorBlendState = &colorBlendStateCI;
		//pipelineCI.pMultisampleState = &multisampleStateCI;
		//pipelineCI.pViewportState = &viewportStateCI;
		//pipelineCI.pDepthStencilState = &depthStencilStateCI;
		//pipelineCI.pDynamicState = &dynamicStateCI;

		//// Create rendering pipeline using the specified states
		//VK_CHECK_RESULT(vkCreateGraphicsPipelines(device, pipelineCache, 1, &pipelineCI, nullptr, &pipeline));
	}
}
#endif