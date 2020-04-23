#pragma once


class TestApp
{
public:
   virtual ~TestApp() {};

   virtual void Init() = 0;
   virtual void Update(float dt) = 0;
   virtual void Render() = 0;
   virtual void Shutdown() {};
};
