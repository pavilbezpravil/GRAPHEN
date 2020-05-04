#pragma once

#include "Graphen/Core/Layer.h"
#include "Graphen/Events/MouseEvent.h"
#include "Graphen/Render/DescriptorHeap.h"


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

      // todo: remove handle
      DescriptorHandle AllocDescHandle();

	private:
		float m_Time = 0.0f;

      UserDescriptorHeap m_descHeap;
	};

}
