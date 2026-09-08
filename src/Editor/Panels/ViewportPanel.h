#pragma once

#include "Renderer/Renderer.h"
#include "Editor/EditorCamera.h"

class ViewportPanel
{
public: 
	void Draw(Renderer& renderer, EditorCamera& editorCamera);

	bool IsHovered() const;
	bool IsFocused() const;

private:
	bool m_isHovered = false;
	bool m_isFocused = false;
};