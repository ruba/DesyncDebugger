#pragma once

#include <memory>

namespace CPUProfiler
{
	const uint16_t MAX_CPUPROFILER_SCOPES = 255;
	const uint16_t INVALID_INDEX = uint16_t(-1);

	struct CPUProfilerScope
	{
		uint64_t start_time = 0;
		const char* name = NULL;
		float time = 0.0f;
		uint16_t parent = INVALID_INDEX;
	};

	inline bool IsScopeValid(uint16_t scope_id) 
	{
		return scope_id != INVALID_INDEX;
	}

	uint16_t GetNumAllocatedScopes();

	void Update();
	
	void BeginFrame(uint16_t scope_id);
	void EndFrame(uint16_t scope_id);

	uint16_t AllocateID(const char* name);
	
	CPUProfilerScope* GetProfileScope(uint16_t scope_id = 0);

	struct CPUProfilerStackScope
	{
		CPUProfilerStackScope(uint16_t scope_id) : scope_id_(scope_id)
		{
			CPUProfiler::BeginFrame(scope_id_);
		}
		~CPUProfilerStackScope()
		{
			CPUProfiler::EndFrame(scope_id_);
		}
		uint16_t scope_id_;
	};
};

#define CPU_PROFILER_SCOPE(name) \
	static uint16_t name##__LINE__ = CPUProfiler::AllocateID(#name);\
	CPUProfiler::CPUProfilerStackScope CPUProfilerStackScope_##name(name##__LINE__);

