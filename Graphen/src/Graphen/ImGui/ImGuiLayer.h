#pragma once

#include "Graphen/Core/Layer.h"
#include "Graphen/Events/MouseEvent.h"
#include "Graphen/Render/DescriptorHeap.h"
#include "Graphen/Render/DynamicDescriptorHeap.h"
struct ImDrawList;
struct ImDrawCmd;


namespace gn {

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach() override;
		virtual void OnDetach() override;

		void Begin();
		void End();

      void InvalidateDeviceObjects();
      void Resize(UINT width, UINT height);

      D3D12_GPU_DESCRIPTOR_HANDLE UploadDescHandle(D3D12_CPU_DESCRIPTOR_HANDLE Handle);

	private:
		float m_Time = 0.0f;

      UserDescriptorHeap m_descHeap;
      std::vector<DescriptorHandle> m_allocatedDescHandles;
      uint m_nextAllocHandleIdx;
      const static uint DESC_SIZE = 32;
	};

}
