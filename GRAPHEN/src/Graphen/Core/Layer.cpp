#include "gnpch.h"
#include "Graphen/Core/Layer.h"

namespace gn {

	Layer::Layer(const std::string& debugName, bool enable)
		: m_DebugName(debugName), m_Enable(enable)
	{
	}
	
}