#pragma once
#include "Renderer.h"

class Rectangle
{
public:
    Rectangle();
	~Rectangle();
    void Draw(Renderer& renderer);
private:
    float mWidth;
    float mHeight;
};

