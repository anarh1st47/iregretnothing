#pragma once
#include "MinHook.h"
#include <vector>
#include <algorithm>


namespace hooks {
	struct hook_t {
		bool enabled = false;
		void* target = nullptr;
		void* original = nullptr;
		void* custom = nullptr;

		void enable() {
			MH_EnableHook(target);
			enabled = true;
		}

		void disable() {
			MH_DisableHook(target);
			enabled = false;
		}
	};

	class c_hooks {
		std::vector<hook_t> m_hooks;

	public:
		c_hooks()
		{
			MH_Initialize();
		}
		~c_hooks()
		{
			MH_Uninitialize();
		}

		template < typename fn = uintptr_t >
		bool create_hook(fn custom_func, void* o_func) {

			hook_t hook = {};
			hook.target = o_func;
			hook.custom = custom_func;
			if (MH_CreateHook(o_func, custom_func, &hook.original) == MH_OK) {
				m_hooks.push_back(hook);
				return true;
			}
			return false;
		}

		void enable() {
			for (auto& h : m_hooks)
				h.enable();
		}

		void restore() {
			for (auto& h : m_hooks)
				h.disable();
		}

		template < typename fn = uintptr_t, class ret = fn >
		ret original(fn custom_func)
		{
			auto found = std::find_if(m_hooks.begin(), m_hooks.end(), [&](hook_t hook) {
				return hook.custom == custom_func;
				});
			if (found != m_hooks.end())
				return reinterpret_cast<ret>(found->original);

			return nullptr;
		}
	};
	inline c_hooks h;
};
