#include "Editor/Panels/InspectorPanel.h"

#include "imgui.h"

#include <string>
#include <vector>

void InspectorPanel::Draw(Scene& scene, AssetManager& assetManager, std::uint32_t& selectedEntityID)
{
	ImGui::Begin("Inspector");

	bool deletedSelectedEntity = false;

	Entity* selectedEntity = scene.FindEntity(selectedEntityID);

	if (selectedEntity != nullptr)
	{
		ImGui::Text("%s", selectedEntity->GetName().c_str());
		ImGui::Separator();

		// Transform Component
		TransformComponent& transform = selectedEntity->GetTransform();

		ImGui::Text("Transform");
		ImGui::DragFloat3("Position", &transform.position.x, 0.05f);
		ImGui::DragFloat3("Rotation", &transform.position.x, 0.05f);
		ImGui::DragFloat3("Scale", &transform.position.x, 0.05f);

		// Mesh Component
		if (selectedEntity->HasMeshComponent())
		{
			MeshComponent* meshComponent = selectedEntity->GetMeshComponent();

			ImGui::Separator();
			ImGui::Text("Mesh");

			// Drag and Drop Target
			ImGui::Button("Drop Mesh Asset Here", ImVec2(-1.0f, 40.0f));
			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESH_ASSET");
				if (payload != nullptr)
				{
					const char* assetName = static_cast<const char*>(payload->Data);

					meshComponent->assetName = assetName;
					meshComponent->mesh = assetManager.GetMesh(assetName);
				}
				ImGui::EndDragDropTarget();
			}

			// Asset Drop
			const char* currentAsset = meshComponent->assetName.empty() ? "None" : meshComponent->assetName.c_str();
			if (ImGui::BeginCombo("Asset", currentAsset))
			{
				const std::vector<std::string> assetNames = assetManager.GetMeshAssetNames();

				for (const std::string& assetName : assetNames)
				{
					const bool selected = meshComponent->assetName == assetName;

					if (ImGui::Selectable(assetName.c_str(), selected))
					{
						meshComponent->assetName = assetName;
						meshComponent->mesh = assetManager.GetMesh(assetName);
					}

					if (selected)
					{
						ImGui::SetItemDefaultFocus();
					}
				}

				ImGui::EndCombo();
			}

			// Remove Mesh Button
			if (ImGui::Button("Remove Mesh Component"))
			{
				selectedEntity->RemoveMeshComponent();
			}
		}
		else
		{
			ImGui::Separator();
			ImGui::Text("Mesh");

			// Add Mesh Component Button
			if (ImGui::Button("Add Mesh Component"))
			{
				MeshComponent& meshComponent = selectedEntity->AddMeshComponent();

				meshComponent.assetName = "Cube";
				meshComponent.mesh = assetManager.GetMesh("Cube");
			}

			ImGui::TextDisabled("Drag a mesh asset here");

			// Add Component withh drag and drop
			ImGui::Button("Drop Mesh Asset Here", ImVec2(-1.0f, 40.0f));

			if (ImGui::BeginDragDropTarget())
			{
				const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("MESH_ASSET");

				if (payload != nullptr)
				{
					const char* assetName = static_cast<const char*>(payload->Data);

					MeshComponent& meshComponent = selectedEntity->AddMeshComponent();
					meshComponent.assetName = assetName;
					meshComponent.mesh = assetManager.GetMesh(assetName);
				}

				ImGui::EndDragDropTarget();
			}
		}

		// Delete Entity
		ImGui::Separator();
		if (ImGui::Button("Deleted Entity"))
		{
			deletedSelectedEntity = true;
		}
	}
	else
	{
		ImGui::TextDisabled("No Entity Selected");
	}

	ImGui::End();

	if (deletedSelectedEntity)
	{
		scene.DestroyEntity(selectedEntityID);
		selectedEntityID = 0;
	}
}