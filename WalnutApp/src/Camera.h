#pragma once

#include <glm/glm.hpp>
#include <vector>

class Camera
{
public:

	/// @brief Create an instance of camera. The parameters are used to calculate
  /// the camera's frustum (a truncated pyramid, a pyramid cut by a plane that
  /// is parallel to the base and the apical part is removed)
	/// @param verticalFOV vertical field of view in degrees. Will be computed as 
  /// radians when used in GLM functions which typically work in radians.
	/// @param nearClip near clipping distance
	/// @param farClip far clipping distance
	Camera(float verticalFOV, float nearClip, float farClip);

	bool OnUpdate(float ts);
	void OnResize(uint32_t width, uint32_t height);

  // NOTES - about the const use in the following 4 functions

  // the first const means that value of the function (the return value) is a reference
  // to a constant glm::mat4 object. The `const` here assures that the function will not
  // modify the member variable.

  // The const after the function parameters (the second const) is called "const member function"
  // or "const method". It indicates that this member function does not modify any non-static
  // data members of the class. IOW it promises not to modify the state of the object on which
  // it is called. This is a good practice to ensure that calling this function doesn't have
  // unintended side effects.

  // By using `const` in both places, the code enforces immutability of the returned
  // object and guarantees that calling the GetProjection() function will not modify 
  // the state of the object.

	/// @brief Gets the projection matrix, which is used to transform 3D world coordinates
  /// into 2D screen coordinates, taking into account the camera's frustum based on the
  /// aspect ratio, near and far clipping planes, and field of view. This type of projection
  /// simulates the depth perception of the human eye, where things appear smaller when
  /// they're farther away.
	const glm::mat4& GetProjection() const { return mProjection; }

  
	const glm::mat4& GetInverseProjection() const { return mInverseProjection; }
	const glm::mat4& GetView() const { return mView; }
	const glm::mat4& GetInverseView() const { return mInverseView; }
	
	const glm::vec3& GetPosition() const { return mPosition; }
	const glm::vec3& GetDirection() const { return mForwardDirection; }

	const std::vector<glm::vec3>& GetRayDirections() const { return mRayDirections; }

	constexpr float GetRotationSpeed() const { return 3.0f; }

private:
	void RecalculateProjection();
	void RecalculateView();
	void RecalculateRayDirections();
private:
	glm::mat4 mProjection{ 1.0f };
	glm::mat4 mView{ 1.0f };
	glm::mat4 mInverseProjection{ 1.0f };
	glm::mat4 mInverseView{ 1.0f };

	float mVerticalFOV = 45.0f;
	float mNearClip = 0.1f;
	float mFarClip = 100.0f;

	glm::vec3 mPosition{0.0f, 0.0f, 0.0f};
	glm::vec3 mForwardDirection{0.0f, 0.0f, 0.0f};

	// Cached ray directions
	std::vector<glm::vec3> mRayDirections;

	glm::vec2 mLastMousePosition{ 0.0f, 0.0f };

	uint32_t mViewportWidth = 0, mViewportHeight = 0;
};