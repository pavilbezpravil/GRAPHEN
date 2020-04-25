#pragma once

#include "Graphen/Core/Core.h"
#include "Graphen/Core/Timestep.h"
#include "Graphen/Events/Event.h"

namespace gn {

	class Layer
	{
	public:
		Layer(const std::string& name = "Layer", bool enable = true);
		virtual ~Layer() = default;

		virtual void OnAttach() {}
		virtual void OnDetach() {}
		virtual void OnUpdate(Timestep ts) {}
		virtual void OnImGuiRender() {}
		virtual void OnEvent(Event& event) {}

      void SetEnable(bool enable) { m_Enable = enable; }
      bool IsEnable() const { return m_Enable; }

		const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
      bool m_Enable;
	};

}