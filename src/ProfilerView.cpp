#include "ProfilerView.h"

#include "CPUProfiler.h"
#include "imgui.h"

#include <vector>

namespace ProfilerView
{
	uint32_t frame_counter = 0;
	uint32_t scope_history_id = 0;

	const	uint32_t history_length = 255;
	float	cpu_perf_history[history_length];

	void ShowTreeNode(uint16_t scope_id)
	{
		CPUProfiler::CPUProfilerScope* scope = CPUProfiler::GetProfileScope(scope_id);
		
		std::vector<uint16_t> childs;
		
		for (uint16_t idx = 0; idx < CPUProfiler::MAX_CPUPROFILER_SCOPES; idx++)
		{
			CPUProfiler::CPUProfilerScope* scope = CPUProfiler::GetProfileScope(idx);

			if (!scope->name)
			{
				break;
			}

			if (scope->parent == scope_id)
			{
				childs.push_back(idx);
			}
		}

		ImGuiTreeNodeFlags nodeFlags = childs.empty() ? ImGuiTreeNodeFlags_Leaf : ImGuiTreeNodeFlags_OpenOnArrow;

		char text[80];
		std::snprintf(text, sizeof(text), "%s: %.2f ###%s", scope->name, scope->time, scope->name);

		if (ImGui::TreeNodeEx(text, nodeFlags))
		{
			if (ImGui::IsMouseDoubleClicked(0) && ImGui::IsItemHovered())
			{
				scope_history_id = scope_id;
				memset(cpu_perf_history, 0, sizeof(cpu_perf_history));
				frame_counter = 0;
			}

			for (auto idx : childs)
			{
				ShowTreeNode(idx);
			}

			ImGui::TreePop();
		}
	}

	void Show(bool& show)
	{
		CPUProfiler::CPUProfilerScope* scopes = CPUProfiler::GetProfileScope();

		ImGui::SetNextWindowSize(ImVec2(650, 200), ImGuiCond_FirstUseEver);

		uint32_t current_sample = frame_counter++ % CPUProfiler::MAX_CPUPROFILER_SCOPES;
		cpu_perf_history[current_sample] = scopes[scope_history_id].time;

		float avg = 0.f;
		float max = 0.f;

		for (uint16_t i = 0; i < history_length; i++)
		{
			avg += cpu_perf_history[i];
			max = max < cpu_perf_history[i] ? cpu_perf_history[i] : max;
		}
		avg /= static_cast<float>(history_length);

		char text[80];
		std::snprintf(text, sizeof(text), "avg: %.2f, max: %.2f", avg, max);

		char plotText[80];
		std::snprintf(plotText, sizeof(plotText), "CPU: %s", scopes[scope_history_id].name);
		
		if (ImGui::Begin("Profiler", &show))
		{
			ImGui::PlotLines(plotText, cpu_perf_history, CPUProfiler::MAX_CPUPROFILER_SCOPES,
				0, text, 0.f, 2000.0f, ImVec2(0, 80));

			ImGui::Separator();

			for (uint16_t scope_id = 0; scope_id < CPUProfiler::MAX_CPUPROFILER_SCOPES; scope_id++)
			{
				if (scopes[scope_id].name && !CPUProfiler::IsScopeValid(scopes[scope_id].parent))
				{
					ShowTreeNode(scope_id);
				}
			}
		}

		ImGui::End();
	}
};
