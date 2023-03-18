#include "CPUProfiler.h"

#include <memory.h>

#include "SDL.h"

namespace CPUProfiler
{
	using namespace std;

	static CPUProfilerScope g_prev_scopes[MAX_CPUPROFILER_SCOPES];
	static CPUProfilerScope g_scopes[MAX_CPUPROFILER_SCOPES];

	static uint16_t g_parrent_scope = INVALID_INDEX;
	static uint16_t g_num_allocated_scopes = 0;

	/////////////////////////////////////////////////////////////////////////
	void BeginFrame(uint16_t scope_id)
	{
		g_scopes[scope_id].start_time = SDL_GetPerformanceCounter();
		g_scopes[scope_id].parent = g_parrent_scope;
		g_parrent_scope = scope_id;
	}

	//////////////////////////////////////////////////////////////////////////
	void EndFrame(uint16_t scope_id)
	{
		uint64_t frequency = SDL_GetPerformanceFrequency();
		uint64_t end_time = SDL_GetPerformanceCounter();

		CPUProfilerScope& scope = g_scopes[scope_id];
		
		float dt = (static_cast<float>(end_time - scope.start_time) * 1000000.0f / frequency);

		scope.time += dt;
		g_parrent_scope = scope.parent;
	}

	//////////////////////////////////////////////////////////////////////////
	uint16_t GetNumAllocatedScopes()
	{
		return g_num_allocated_scopes;
	}

	//////////////////////////////////////////////////////////////////////////
	uint16_t AllocateID(const char* name)
	{
		g_scopes[g_num_allocated_scopes].name = name;
		g_scopes[g_num_allocated_scopes].parent = g_parrent_scope;

		return g_num_allocated_scopes++;
	}

	//////////////////////////////////////////////////////////////////////////
	void Update()
	{
		memcpy(g_prev_scopes, g_scopes, g_num_allocated_scopes * sizeof(CPUProfilerScope));

		for (uint32_t i = 0; i < g_num_allocated_scopes; i++)
		{
			g_scopes[i].time = 0.0;
		}
	}

	//////////////////////////////////////////////////////////////////////////
	CPUProfilerScope* GetProfileScope(uint16_t scope_id)
	{
		return &g_prev_scopes[scope_id];
	}
}
