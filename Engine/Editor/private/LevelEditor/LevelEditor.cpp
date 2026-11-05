#include <LevelEditor/LevelEditor.h>
#include <LevelEditor/LevelSerializer.h>
#include <LevelEditor/ECS/ecsLevelEditor.h>

#include <Filesystem.h>
#include <ECS/ecsSystems.h>
#include <Parser/WorldParser.h>

#include <imgui.h>
#include <imgui_stdlib.h>

namespace GameEngine
{
	namespace Editor
	{
		LevelEditor::LevelEditor(flecs::world& world)
		{
			m_Level = LevelSerializer::Deserialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string());

			for (World::LevelObject& levelObject : m_Level->GetLevelObjects())
			{
				flecs::entity entity = world.entity(levelObject.GetName().c_str());

				World::LevelObject::ComponentList& componentList = levelObject.GetComponents();

				World::LevelObject::ComponentList::iterator positionAttribute = std::ranges::find_if(componentList,
					[](World::LevelObject::Component& component)
					{
						return !std::strcmp(component.first.c_str(), "Position");
					}
				);

				World::LevelObject::ComponentList::iterator geometryAttribute = std::ranges::find_if(componentList,
					[](World::LevelObject::Component& component)
					{
						return !std::strcmp(component.first.c_str(), "GeometryPtr");
					}
				);

				if (positionAttribute != componentList.end() &&
					geometryAttribute != componentList.end())
				{
					assert(World::WorldParser::GetCustomComponents().contains(geometryAttribute->second));

					entity.set(EntitySystem::LevelEditorECS::PositionDesc{ &positionAttribute->second });

					// Can be set to 0 since it doesn't matter now, will be updated by the system
					entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
					entity.set(GeometryPtr{
						reinterpret_cast<RenderCore::Geometry*>(
							World::WorldParser::GetCustomComponents()[geometryAttribute->second]
							)
						});
				}
			}

			EntitySystem::LevelEditorECS::RegisterLevelEditorEcsSystems(world);
		}

		void LevelEditor::Draw()
		{
			ImGui::Begin(GetName());

			if (m_Level.has_value()) [[likely]]
			{
				for (World::LevelObject& levelObject : m_Level->GetLevelObjects())
				{
					if (ImGui::TreeNode(levelObject.GetName().c_str()))
					{
						for (World::LevelObject::Component& component : levelObject.GetComponents())
						{
							std::string value_str = component.second;

							std::vector<float> values;
							values.reserve(4);

							size_t pos = value_str.find(',');

							if (pos == std::string::npos)
							{
								ImGui::InputText(component.first.c_str(), &component.second);
							}
							else
							{
								size_t start_pos = 0;
								while (pos != std::string::npos)
								{
									size_t value_len = pos - start_pos;
									std::string current_value_str = value_str.substr(start_pos, value_len);
									values.push_back(stof(current_value_str));

									//start_pos = pos + 1;
									value_str = value_str.substr(pos + 1);
									pos = value_str.find(',');
								}

								values.push_back(stof(value_str));

								ImGui::InputScalarN(component.first.c_str(), ImGuiDataType_Float, values.data(), values.size(), nullptr, nullptr, "%.1f");
							}
						}

						ImGui::TreePop();
					}
				}
			}

			if (ImGui::Button("CreateDefaultObject"))
			{
				GameEngine::toCreate = true;
			}

			if (ImGui::Button("Save"))
			{
				m_SaveButtonMessageTimer.Reset();
				m_SaveButtonPressed = true;

				Save();
			}

			if (m_SaveButtonPressed)
			{
				ImGui::SameLine();
				ImGui::Text("Saved!");
			}

			ImGui::End();
		}

		void LevelEditor::Update(float dt)
		{
			m_SaveButtonMessageTimer.Tick();

			if (m_SaveButtonMessageTimer.GetTotalTime() > m_TimeToShowSaveButtonMessage)
			{
				m_SaveButtonPressed = false;
			}
		}

		void LevelEditor::Save()
		{
			assert(m_Level.has_value());
			LevelSerializer::Serialize(Core::g_FileSystem->GetFilePath("Levels/Main.xml").generic_string(), m_Level.value());
		}

		void LevelEditor::AddLevelObject(const std::string& name, const std::string& position, const std::string& geometry, flecs::world& world) {
			World::LevelObject default_object;

			default_object.SetName(name.c_str());
			default_object.AddComponent("Position", position.c_str());
			default_object.AddComponent("GeometryPtr", geometry.c_str());

			m_Level->AddLevelObject(std::move(default_object));

			World::LevelObject& def_obj_link = m_Level->GetLevelObjects().back();

			flecs::entity entity = world.entity(def_obj_link.GetName().c_str());

			World::LevelObject::ComponentList& componentList = def_obj_link.GetComponents();

			World::LevelObject::ComponentList::iterator positionAttribute = std::ranges::find_if(componentList,
				[](World::LevelObject::Component& component)
				{
					return !std::strcmp(component.first.c_str(), "Position");
				}
			);

			World::LevelObject::ComponentList::iterator geometryAttribute = std::ranges::find_if(componentList,
				[](World::LevelObject::Component& component)
				{
					return !std::strcmp(component.first.c_str(), "GeometryPtr");
				}
			);

			if (positionAttribute != componentList.end() &&
				geometryAttribute != componentList.end())
			{
				assert(World::WorldParser::GetCustomComponents().contains(geometryAttribute->second));

				entity.set(EntitySystem::LevelEditorECS::PositionDesc{ &positionAttribute->second });

				// Can be set to 0 since it doesn't matter now, will be updated by the system
				entity.set(EntitySystem::EditorECS::Position{ 0.0f, 0.0f, 0.0f });
				entity.set(GeometryPtr{
					reinterpret_cast<RenderCore::Geometry*>(
						World::WorldParser::GetCustomComponents()[geometryAttribute->second]
						)
					});
			}
		}

	}
}