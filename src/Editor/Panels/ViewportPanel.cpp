#include "Editor/Panels/ViewportPanel.h"

#include "imgui.h"

#include <cstdint>

void ViewportPanel::Draw(Renderer& renderer, EditorCamera& editorCamera)
{
	ImGui::Begin("Viewport");

	// Gets avaialbale space within imgui viewport window
	const ImVec2 viewportSize = ImGui::GetContentRegionAvail();

	const int viewportWidth = static_cast<int>(viewportSize.x);
	const int viewportHeight = static_cast<int>(viewportSize.y);

	if (viewportWidth > 0 && viewportHeight > 0)
	{
		// Resize off-screen render target
		renderer.ResizeViewport(viewportWidth, viewportHeight);

		// Update camera aspect ratio to match the editor viewport
		editorCamera.SetViewportSize(viewportWidth, viewportHeight);

		ID3D11ShaderResourceView* viewportTexture = renderer.GetViewportShaderResourceView();

		if (viewportTexture != nullptr)
		{
			ImTextureID textureID = static_cast<ImTextureID>(reinterpret_cast<std::uintptr_t>(viewportTexture));

			ImGui::Image(ImTextureRef(textureID), viewportSize);
		}
	}

	// Store viewport interaction state
	m_isHovered = ImGui::IsWindowHovered();
	m_isFocused = ImGui::IsWindowFocused();

	ImGui::End();
}

bool ViewportPanel::IsHovered() const
{
	return m_isHovered;
}

bool ViewportPanel::IsFocused() const
{
	return m_isFocused;
}