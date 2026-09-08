#include "Editor/Panels/HierarchyPanel.h"

#include "imgui.h"

void HierarchyPanel::Draw(Scene& scene, std::uint32_t& selectedEntityID)
{
	ImGui::Begin("Hierarchy");
	ImGui::Text("Scene");

	// Adding Entities
	if (ImGui::Button("+ Add Entity"))
	{
		Entity& newEntity = scene.CreateEntity("New Entity");
		selectedEntityID = newEntity.GetID();
	}

	ImGui::Separator();

	// Entity List
	for (const Entity& entity : scene.GetEntities())
	{
		const bool isSelected = selectedEntityID == entity.GetID();

		ImGui::PushID(static_cast<int>(entity.GetID()));

		if (ImGui::Selectable(entity.GetName().c_str(), isSelected))
		{
			selectedEntityID = entity.GetID();
		}

		ImGui::PopID();
	}

	ImGui::End();
}