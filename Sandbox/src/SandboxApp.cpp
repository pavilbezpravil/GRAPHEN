#include <Graphen.h>
#include <Graphen/Core/EntryPoint.h>

#include "ExampleLayer.h"

class Sandbox : public gn::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{
	}
};

gn::Application* gn::CreateApplication()
{
	return new Sandbox();
}
