#pragma once
#include "g2dobject.h"
#include "g2dinput.h"

namespace g2d
{
	constexpr unsigned int DefaultCameraVisibkeMask = 0xFFFFFFFF;

	constexpr int DefaultComponentOrder = 0x5000;

	struct Component;
	struct Camera;
	struct SceneNode;
	struct Scene;

	/** \brief Base Component
	*
	*	Custom component inherit this class to gain the ability of acting different behaviors when different events occurred.
	*
	*	Component is very most important concept in Got2D engine.
	*	Add different components to SceneNode to present varity behaviors.
	*	One SceneNode can mounts a set of componets.
	*/
	struct G2DAPI Component : public Object
	{
	public:
		/**
		*	Get the SceneNode holds the components.
		*	Provided to the custom component to retrieve the node.
		*/
		SceneNode* GetSceneNode() const { return mAttachNode; }

		point2d<float> GetSceneNodePosition() const;

		point2d<float> GetSceneNodeWorldPosition() const;

		float2 GetSceneNodeScale() const;

		radian<float> GetSceneNodeRotation() const;
		/**
		*	It equals to GetSceneNode()->GetVisibleMask();
		*	The mask will collaborate with the camera's visible mask
		*/
		unsigned int GetCameraVisibleMask() const;

	public:
		/** \brief Local AABB
		*
		*	override this function to define the boundry of a
		*	visible object, the result of visibility testing depends
		*	on the whether their aabb is intersection with cameras boundtry.
		*/
		virtual const aabb2d<float>& GetLocalAABB() const { static aabb2d<float> b; return b; }

		/**
		*	AABB in world-space, by defaultm, calculating world-space
		*	AABB is transforming local AABB with their world transform.
		*/
		virtual aabb2d<float> GetWorldAABB() const;

		/**
		*	override this function to redefine the expected execution order.
		*	Less number will be executed first.
		*/
		virtual int GetExecuteOrder() const { return DefaultComponentOrder; }

		/**
		*
		*/
		virtual void Release() = 0;

		/********************************************************************
		* Event Listener
		*********************************************************************/
	public:

		/**
		*	Trigger when SceneNode being created or the
		*	component being added to the node successfully.
		*/
		virtual void OnInitial() { }

		/**
		*	Trigger each frame
		*/
		virtual void OnUpdate(unsigned int deltaTime) { }

		/**
		*	Trigger when the node going to render.
		*	User need to commit a render request to render system.
		*/
		virtual void OnRender() { }

		/**
		*	Trigger when local Position changes by calling SetPosition().
		*/
		virtual void OnPositionChanging(const point2d<float>& newPos) { }

		virtual void OnPositionChanged(const point2d<float>& newPos) { }

		/**
		*	Trigger when local Position changes by calling SetPosition().
		*/
		virtual void OnPivotChanging(const point2d<float>& newPos) { }

		virtual void OnPivotChanged(const point2d<float>& newPos) { }

		/**
		*	Trigger when local Roll rotation changes by calling SetRotation().
		*/
		virtual void OnRotateChanging(radian<float> r) { }

		virtual void OnRotateChanged(radian<float> r) { }

		/**
		*	Trigger when local scale changes by calling SetScale().
		*/
		virtual void OnScaleChanging(const float2& newScaler) { }

		virtual void OnScaleChanged(const float2& newScaler) { }

		/**
		*	First update event after local tranform changes.
		*	It comes after OnUpdate event, do AABB update here.
		*/
		virtual void OnPostUpdateTransformChanged() { }

		/**
		*	Trigger when raw messages comes.
		*	Some messages will be converted to special events list below.
		*/
		virtual void OnMessage(const Message& message) { }

		/**
		*	Trigger when cursor first hit the SceneNode.
		*	Adjacent SceneNode is the last cursor hovering node,
		*	it would be null if cursor hit the node from empty ground.
		*/
		virtual void OnCursorEnterFrom(SceneNode* adjacency, const Mouse&, const Keyboard&) { }

		/**
		*	triggering Repeatly when cursor hovering the node.
		*/
		virtual void OnCursorHovering(const Mouse&, const Keyboard&) { }

		/**
		*	Trigger when cursor leave off the SceneNode.
		*	Adjacent SceneNode is the next cursor hovering node,
		*	it would be null if cursor goes to empty ground.
		*/
		virtual void OnCursorLeaveTo(SceneNode* adjacency, const Mouse&, const Keyboard&) { }

		/**
		*	Trigger when node is clicked.
		*/
		virtual void OnLClick(const Mouse&, const Keyboard&) { }

		virtual void OnRClick(const Mouse&, const Keyboard&) { }

		virtual void OnMClick(const Mouse&, const Keyboard&) { }

		/**
		*	Trigger when node is double clicked.
		*/
		virtual void OnLDoubleClick(const Mouse&, const Keyboard&) { }

		virtual void OnRDoubleClick(const Mouse&, const Keyboard&) { }

		virtual void OnMDoubleClick(const Mouse&, const Keyboard&) { }

		/**
		*	Trigger when the node is dragged at first time.
		*/
		virtual void OnLDragBegin(const Mouse&, const Keyboard&) { }

		virtual void OnRDragBegin(const Mouse&, const Keyboard&) { }

		virtual void OnMDragBegin(const Mouse&, const Keyboard&) { }

		/**
		*	Trigger when node is dragging without hitting other nodes.
		*/
		virtual void OnLDragging(const Mouse&, const Keyboard&) { }

		virtual void OnRDragging(const Mouse&, const Keyboard&) { }

		virtual void OnMDragging(const Mouse&, const Keyboard&) { }

		/**
		*	Trigger when node is drag end without hitting other nodes.
		*/
		virtual void OnLDragEnd(const Mouse&, const Keyboard&) { }

		virtual void OnRDragEnd(const Mouse&, const Keyboard&) { }

		virtual void OnMDragEnd(const Mouse&, const Keyboard&) { }

		/**
		*	Trigger when node is dragging with hitting other nodes.
		*/
		virtual void OnLDropping(SceneNode* dropped, const Mouse&, const Keyboard&) { }

		virtual void OnRDropping(SceneNode* dropped, const Mouse&, const Keyboard&) { }

		virtual void OnMDropping(SceneNode* dropped, const Mouse&, const Keyboard&) { }

		/**
		*	Trigger when node is drag end with hitting other nodes.
		*/
		virtual void OnLDropTo(SceneNode* dropped, const Mouse&, const Keyboard&) { }

		virtual void OnRDropTo(SceneNode* dropped, const Mouse&, const Keyboard&) { }

		virtual void OnMDropTo(SceneNode* dropped, const Mouse&, const Keyboard&) { }

		/**
		*	Trigger when key is pressed, mutex to pressing.
		*/
		virtual void OnKeyPress(KeyCode key, const Mouse&, const Keyboard& keyboard) { }

		/**
		*	Trigger when key is pressing at first time, mutex to pressed.
		*/
		virtual void OnKeyPressingBegin(KeyCode key, const Mouse&, const Keyboard& keyboard) { }

		/**
		*	Trigger when key pressing repeatly.
		*/
		virtual void OnKeyPressing(KeyCode key, const Mouse&, const Keyboard& keyboard) { }

		/**
		*	Trigger when the key is released.
		*/
		virtual void OnKeyPressingEnd(KeyCode key, const Mouse&, const Keyboard& keyboard) { }


		/** Internal use
		*
		*	Please do not use the function below otherwise it will cause undetermin internal-errors.
		*/
	public:
		void _SetSceneNode_Internal(SceneNode* node);

		void _SetRenderingOrder_Internal(unsigned int& order);

		unsigned int _GetRenderingOrder_Internal() { return mRenderingOrder; }

	private:
		SceneNode* mAttachNode = nullptr;

		unsigned int mRenderingOrder = 0xFFFFFFFF;
	};

	/** \brief Image Quad
	*
	*/
	struct G2DAPI Quad : public Component
	{
	public:
		static Quad* Create();

		/**
		*	Resize mesh size of the Quad.
		*/
		virtual Quad* SetSize(const float2& size) = 0;

		/**
		*	Get the mesh size of the Quad.
		*/
		virtual const float2& GetSize() const = 0;
	};

	/**
	*	Camera is used for visibility testing & rendering
	*/
	struct G2DAPI Camera : public Component
	{
	public:
		/**
		*	Index in scene, index of main(default) camera is 0
		*/
		virtual unsigned int GetIndex() const = 0;

		/**
		*	Setting eye's Position.
		*/
		virtual Camera* SetPosition(const point2d<float>& p) = 0;

		/**
		*	Setting camera's boundry.
		*/
		virtual Camera* SetScale(const float2& s) = 0;

		/**
		*	Setting camera's Roll rotation.
		*/
		virtual Camera* SetRotation(radian<float> r) = 0;

		/**
		*	Setting camera's rendering order.
		*	Multiple cameras need to be sort before rendering,
		*	less rendering order will be rendered first
		*/
		virtual void SetRenderingOrder(int order) = 0;

		virtual int GetRenderingOrder() const = 0;

		/**
		*	Visible matching mask, used for visibility testing.
		*	If scene nodes's mask is overlapped this mask,
		*	we treat it as visible.
		*/
		virtual void SetCameraVisibleMask(unsigned int mask) = 0;

		virtual unsigned int GetCameraVisibleMask() const = 0;

		/**
		*	Wont do the visibility testing and rendering if camera is not activated.
		*/
		virtual void SetActivity(bool activity) = 0;

		/**
		*	Is camera is not activated ?
		*/
		virtual bool IsActivity() const = 0;

		virtual const float2x3& GetViewMatrix() const = 0;

		/**
		*	Check whether an AABB is intersect with the camera boundry.
		*	Point AABB is treat as not visible.
		*/
		virtual bool TestVisible(const aabb2d<float>& bounding) const = 0;

		/**
		*	Check whether a component is visible.
		*	Point AABB, mask is not matching, will be regard to not visible.
		*/
		virtual bool TestVisible(Component* component) const = 0;

		/**
		*	Convert screen-space coordinate to world-space coordinate
		*/
		virtual point2d<float> ScreenToWorld(const point2d<int>& pos) const = 0;

		/**
		*	Convert world-space coordinate to screen-space coordinate
		*/
		virtual point2d<int> WorldToScreen(const point2d<float>& pos) const = 0;
	};

	/**
	*	Node in Scene Tree
	*/
	struct G2DAPI SceneNode : public Object
	{
	public:

		/**
		*	Which scene the node belongs to.
		*/
		virtual Scene* GetScene() = 0;

		/**
		*	Return nullptr if the parent is Scene.
		*/
		virtual SceneNode* GetParentNode() = 0;

		/**
		*	Retrieve the first child.
		*	If scene have no children, return nullptr.
		*/
		virtual SceneNode* GetFirstChild() = 0;

		/**
		*	Return nullptr if the node is the first child.
		*/
		virtual SceneNode* GetPrevSiblingNode() = 0;

		/**
		*	Return nullptr if the node is the last child.
		*/
		virtual SceneNode* GetNextSiblingNode() = 0;

		/**
		*	Retrieve the last child.
		*	If scene have no children,  return nullptr.
		*/
		virtual SceneNode* GetLastChild() = 0;

		virtual SceneNode* GetChildByIndex(unsigned int index) = 0;

		virtual unsigned int GetChildCount() const = 0;

		/**
		*	Create a child node, and add it to the node
		*/
		virtual SceneNode* CreateChild() = 0;

		/**
		*	Swap index with the LAST child, make sure the 
		*	node is to rendering at bottom.
		*	CAUTION: Do nothing when the node is the first.
		*/
		virtual void MoveToFront() = 0;

		/**
		*	Swap index with the FIRST child, make sure the 
		*	node is to rendering at top.
		*	CAUTION: Do nothing when the node is the last.
		*/
		virtual void MoveToBack() = 0;

		/**
		*	Swap index with PREV child, to adjust rendering order.
		*	Do nothing when the node is the first child.
		*/
		virtual void MovePrev() = 0;

		/**
		*	Swap index with NEXT child, to adjust rendering order.
		*	Do nothing when the node is the last child.
		*/
		virtual void MoveNext() = 0;

		virtual bool AddComponent(Component*, bool autoRelease) = 0;

		/**
		*	Remove a component with auto release setting.
		*	Component::Release will be called if auto release is set.
		*/
		virtual bool RemoveComponent(Component*) = 0;

		/**
		*	Remove a component without releasing.
		*/
		virtual bool RemoveComponentWithoutRelease(Component*) = 0;

		virtual bool HasComponent(Component*) const = 0;

		/**
		*	Query auto release state of a certain component.
		*	Return false if component is removed or not exist.
		*/
		virtual bool IsComponentAutoRelease(Component*) const = 0;

		/**
		*	 Get component by index.
		*	CAUTION: Index of component will changes 
		*	due to change the execution order.
		*/
		virtual Component* GetComponentByIndex(unsigned int index) = 0;

		virtual unsigned int GetComponentCount() const = 0;

		virtual const float2x3& GetLocalMatrix() = 0;

		virtual const float2x3& GetWorldMatrix() = 0;

		/**
		*	Setting local-space Position.
		*/
		virtual SceneNode* SetPosition(const point2d<float>& Position) = 0;

		/**
		*	Local Position.
		*/
		virtual point2d<float> GetPosition() const = 0;

		/**
		*	Setting world-space Position, it will changes local Position.
		*/
		virtual SceneNode* SetWorldPosition(const point2d<float>& Position) = 0;

		/**
		*	World-space Position, it equals to local Position tranfromed by world transform.
		*/
		virtual point2d<float> GetWorldPosition() = 0;

		/**
		*	Setting world-space right direction, it will changes local rotation.
		*/
		virtual SceneNode* SetRightDirection(const nfloat2& right) = 0;

		/**
		*	World-space right direction.
		*/
		virtual const nfloat2 GetRightDirection() = 0;

		/**
		*	Setting world-space up direction, it will changes local rotation.
		*/
		virtual SceneNode* SetUpDirection(const nfloat2& up) = 0;

		/**
		*	World-space up directoin.
		*/
		virtual const nfloat2 GetUpDirection() = 0;

		/**
		*	Setting offset of center.
		*/
		virtual SceneNode* SetPivot(const float2& pivot) = 0;

		/**
		*	Local offset of center.
		*/
		virtual const float2& GetPivot() const = 0;

		/**
		*	Setting local scale.
		*/
		virtual SceneNode* SetScale(const float2& scale) = 0;

		/**
		*	Local scale
		*/
		virtual const float2& GetScale() const = 0;

		/**
		*	Setting local Roll rotation.
		*/
		virtual SceneNode* SetRotation(radian<float> r) = 0;

		/**
		*	Local Roll rotation 
		*/
		virtual radian<float> GetRotation() const = 0;

		/**
		*	Manually setting visibility.
		*	None-seen node will not be rendered.
		*/
		virtual void SetVisible(bool) = 0;

		virtual bool IsVisible() const = 0;

		/**
		*	Static node will be add to spatial graph(quadtree)
		*	to boost efficiency of visibility testing.
		*/
		virtual void SetStatic(bool) = 0;

		virtual bool IsStatic() const = 0;

		/**
		*	Setting visible mask.
		*	The mask will collaborate with camera's visible mask 
		*	to determin whether it will be seen by the camera.
		*/
		virtual void SetCameraVisibleMask(unsigned int mask, bool recursive) = 0;

		/**
		*	Visible Mask will collaborate with camera's visible mask 
		*	to determin whether the node will be seen by the camera.
		*/
		virtual unsigned int GetCameraVisibleMask() const = 0;

		/**
		*	Query the child index of his father.
		*/
		virtual unsigned int GetChildIndex() const = 0;

		/**
		*	Is the node is released, but not being destroyed ?
		*/
		virtual bool IsRemoved() const = 0;

		/**
		*	Convert a world-space coordinate to the local-space coordinate.
		*/
		virtual point2d<float> WorldToLocal(const point2d<float>& pos) = 0;

		/**
		*	Convert a world-space coordinate to the parent-space coordinate.
		*/
		virtual point2d<float> WorldToParent(const point2d<float>& pos) = 0;

		/**
		*	Destroy the node, remove from its parent, and release all components.
		*/
		virtual void Release() = 0;
	};

	/*
	*	Query the first component by a certain type.
	*/
	template<typename T> T* FindComponent(SceneNode* node);

	/**
	*
	*/
	struct G2DAPI Scene : public Object
	{
	public:
		virtual SceneNode* GetRootNode() = 0;
		/**
		*	Create a child node, with a camera component, and add it the scene.
		*/
		virtual Camera* CreateAdditionalCameraNode() = 0;

		/**
		*	Default camera, it return the result of
		*	scene->GetCameraByIndex(0);
		*/
		virtual Camera* GetDefaultCamera() = 0;

		/**
		*	Get camera by ID.
		*
		*	The ID of main(default) camera is 0.
		*/
		virtual Camera* GetCameraByIndex(unsigned int index) = 0;

		/**
		*	 Number of exist cameras.
		*/
		virtual unsigned int GetCameraCount() const = 0;

		/**
		*	Call it manually each frame, to send OnRender event
		*	to scene tree, so that each node can notify its components.
		*/
		virtual void Render() = 0;

		/**
		*
		*/
		virtual void Release() = 0;
	};

	template<typename T>
	T* FindComponent(SceneNode* node)
	{
		auto count = node->GetComponentCount();
		for (unsigned int i = 0; i < count; i++)
		{
			Component* component = node->GetComponentByIndex(i);
			if (Is<T>(component))
			{
				return static_cast<T*>(component);
			}
		}
		return nullptr;
	}
}
