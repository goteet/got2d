#pragma once
#include "g2dobject.h"
#include "g2dmessage.h"

namespace g2d
{
	struct RenderSystem;
	struct Scene;
	
	/** \brief Got2D starts here
	*
	*	This is the main entrance of the entire engine.
	*/
	struct G2DAPI Engine : public Object
	{
		/** \brief
		*
		*	Fill this sturct to initialize the engine.
		*/
		struct CreationConfig
		{
			/** Native rendering window handle
			*
			*	eg. HWND, EGLWindowType, etc.
			*/
			void* NativeWindow;

			/** \brief
			*
			*	 Engine will prefix this path to all relative resource-loading paths using in the engine, turning them to absolute paths.
			*/
			const char* ResourceFolderPath;
		};

		enum class InitialResult
		{
			Success,
			AlreadyInitialized,
			Error,
		};

		/** \brief Create an Engine Instance;
		*
		*/
		static InitialResult Initialize(const CreationConfig& config);

		static void Uninitialize();

		static Engine* GetInstance();

		/**
		*
		*/
		virtual RenderSystem* GetRenderSystem() = 0;

		/**
		*
		*/
		virtual Scene* CreateNewScene(float sceneBoundSize) = 0;

		/**
		*
		*	Call update each frame
		*/
		virtual void Update(unsigned int deltaTime) = 0;

		/**
		*
		*	Notity Engine when OS message comes.
		*/
		virtual void OnMessage(const Message& message) = 0;

		/** \brief Create an Engine Instance;
		*
		*	Call it when the size of the native render window changed.
		*/
		virtual bool OnResize(unsigned int width, unsigned int height) = 0;

		/**
		*
		*/
		virtual void Release() = 0;
	};
}


