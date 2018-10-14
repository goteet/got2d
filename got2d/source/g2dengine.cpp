#include "g2dengine.h"
#include "engine.h"
#include "cxx_scope.h"

namespace g2d
{
	Engine::InitialResult Engine::Initialize(const Engine::CreationConfig & config)
	{
		::Engine*& pEngine = ::Engine::Instance;

		if (pEngine == nullptr)
		{
			auto failGuard = cxx::make_scope_guard([&] {
				cxx::safe_delete(pEngine);
			});

			pEngine = new ::Engine();

			if (pEngine->Initialize(config))
			{
				failGuard.dismiss();
				return InitialResult::Success;
			}

			return InitialResult::Error;
		}
		else
		{
			return InitialResult::AlreadyInitialized;
		}
	}

	void Engine::Uninitialize()
	{
		cxx::safe_delete(::Engine::Instance);
	}

	Engine* Engine::GetInstance()
	{
		return ::Engine::Instance;
	}
	bool Engine::IsInitialized()
	{
		return GetInstance() != nullptr;
	}
}