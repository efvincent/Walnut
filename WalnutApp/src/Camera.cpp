#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include "Walnut/Input/Input.h"

using namespace Walnut;

Camera::Camera(float verticalFOV, float nearClip, float farClip)
	: mVerticalFOV(verticalFOV), mNearClip(nearClip), mFarClip(farClip)
{
	mForwardDirection = glm::vec3(0, 0, -1);
	mPosition = glm::vec3(0, 0, 6);
}

bool Camera::OnUpdate(float timeStepSeconds)
{
	glm::vec2 mousePos = Input::GetMousePosition();
	glm::vec2 delta = (mousePos - mLastMousePosition) * 0.002f;
	mLastMousePosition = mousePos;

	if (!Input::IsMouseButtonDown(MouseButton::Right))
	{
		Input::SetCursorMode(CursorMode::Normal);
		return false;
	}

	Input::SetCursorMode(CursorMode::Locked);

	bool moved = false;

  // NOTES - `constexpr` here tells the compiler that the expression is a constant and can
  // be evaluated by the compiler at compile time if possible. `constexpr` can also not be
  // modified after its initialization

	constexpr glm::vec3 upDirection(0.0f, 1.0f, 0.0f);

  // NOTES - glm::cross
  // the cross product of two vectors, results in a third vector that is perpendicular
  // (orthogonal) to both of the original vectors. The direction is determined by the right-hand
  // rule.
  //
  // right hand rule: Point your right thumb in the direction of the first vector (forward in 
  // this case), and your index finger along the second vector (up). Your palm now describes
  // a plane. Point your middle finger perpendicular to the plane, and it's pointing in the
  // direction of the cross product of the two vectors.
  //
  // why right hand instead of left hand? It depends on the API in use. GLM and by extension
  // OpenGL use the right hand rule for cross product.
	glm::vec3 rightDirection = glm::cross(mForwardDirection, upDirection);

	float unitsPerSecondSpeed = 5.0f;

	// Movement
	if (Input::IsKeyDown(KeyCode::W))
	{
		mPosition += mForwardDirection * unitsPerSecondSpeed * timeStepSeconds;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::S))
	{
		mPosition -= mForwardDirection * unitsPerSecondSpeed * timeStepSeconds;
		moved = true;
	}
	if (Input::IsKeyDown(KeyCode::A))
	{
		mPosition -= rightDirection * unitsPerSecondSpeed * timeStepSeconds;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::D))
	{
		mPosition += rightDirection * unitsPerSecondSpeed * timeStepSeconds;
		moved = true;
	}
	if (Input::IsKeyDown(KeyCode::Q))
	{
		mPosition -= upDirection * unitsPerSecondSpeed * timeStepSeconds;
		moved = true;
	}
	else if (Input::IsKeyDown(KeyCode::E))
	{
		mPosition += upDirection * unitsPerSecondSpeed * timeStepSeconds;
		moved = true;
	}

	// Rotation
	if (delta.x != 0.0f || delta.y != 0.0f)
	{
		float pitchDelta = delta.y * GetRotationSpeed();
		float yawDelta = delta.x * GetRotationSpeed();

		glm::quat rotQuat = 
      glm::normalize(
        glm::cross(
          glm::angleAxis(-pitchDelta, rightDirection),
			    glm::angleAxis(-yawDelta, glm::vec3(0.f, 1.0f, 0.0f))
        )
      );

		mForwardDirection = glm::rotate(rotQuat, mForwardDirection);

		moved = true;
	}

	if (moved)
	{
		RecalculateView();
		RecalculateRayDirections();
	}

	return moved;
}

void Camera::OnResize(uint32_t width, uint32_t height)
{
	if (width == mViewportWidth && height == mViewportHeight)
		return;

	mViewportWidth = width;
	mViewportHeight = height;

	RecalculateProjection();
	RecalculateRayDirections();
}

void Camera::RecalculateProjection()
{
	mProjection = glm::perspectiveFov(glm::radians(mVerticalFOV), (float)mViewportWidth, (float)mViewportHeight, mNearClip, mFarClip);
	mInverseProjection = glm::inverse(mProjection);
}

void Camera::RecalculateView()
{
	mView = glm::lookAt(mPosition, mPosition + mForwardDirection, glm::vec3(0, 1, 0));
	mInverseView = glm::inverse(mView);
}

/// @brief Recalculate all the ray directions into a cache. 
/// see: https://youtu.be/lXlXqUEEJ94?si=mbhiYI6lIgt5-ah5&t=997 for detailed explanation
void Camera::RecalculateRayDirections()
{
	mRayDirections.resize(mViewportWidth * mViewportHeight);

	for (uint32_t y = 0; y < mViewportHeight; y++)
	{
		for (uint32_t x = 0; x < mViewportWidth; x++)
		{
			glm::vec2 coord = { (float)x / (float)mViewportWidth, (float)y / (float)mViewportHeight };
			coord = coord * 2.0f - 1.0f; // -1 -> 1

			glm::vec4 target = mInverseProjection * glm::vec4(coord.x, coord.y, 1, 1);
			glm::vec3 rayDirection = glm::vec3(mInverseView * glm::vec4(glm::normalize(glm::vec3(target) / target.w), 0)); // World space
			mRayDirections[x + y * mViewportWidth] = rayDirection;
		}
	}
}