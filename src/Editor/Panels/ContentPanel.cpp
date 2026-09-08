#include "Editor/Panels/ContentPanel.h"

#include "imgui.h"

#include <string>
#include <vector>

void ContentPanel::Draw(AssetManager& assetManager)
{
	ImGui::Begin("Content Browser");

	ImGui::Text("Mesh Asset");
	ImGui::Separator();

	const std::vector<std::string> meshAssetNames = assetManager.GetMeshAssetNames();

	for (const std::string& assetName : meshAssetNames)
	{
		ImGui::Selectable(assetName.c_str(), false);

		// Drag Source
		if (ImGui::BeginDragDropSource())
		{
			ImGui::SetDragDropPayload("MESH_ASSET", assetName.c_str(), assetName.size() + 1);
			ImGui::Text("Mesh %s", assetName.c_str());
			ImGui::EndDragDropSource();
		}
	}

	ImGui::End();
}