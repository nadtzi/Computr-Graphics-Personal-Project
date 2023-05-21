#ifndef CAMERA_H
#define CAMERA_H

#include "glm/glm.hpp"
#include "glm/gtc/constants.hpp"

class Camera
{
public:
	glm::mat4 getViewMatrix() const;

	virtual void setPosition(const glm::vec3& position){}
	virtual void rotate(float yaw, float pitch) {}
	virtual void move(const glm::vec3& offsetPos) {}

	const glm::vec3& getLook() const;
	const glm::vec3& getRight() const;
	const glm::vec3& getUp() const;
	const glm::vec3& getPosition() const;

	float getFOV() const { return mFOV; }
	void setFOV(float fov) { mFOV = fov; }

protected:
	Camera();

	virtual void updateCameraVectors() {}
	glm::vec3 mPosition;
	glm::vec3 mTargetPos;
	glm::vec3 mLook;
	glm::vec3 mUp;
	glm::vec3 mRight;
	const glm::vec3 World_Up;

	// camera parameters 
	float mYaw;
	float mPitch;
	float mFOV;

};



//Implementation of FPS Camera
class FPSCamera : public Camera
{
public:
	FPSCamera(glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f), float yaw = glm::pi<float>(),
		float pitch = 0.0f);
	FPSCamera(glm::vec3 position, glm::vec3 target);
	
	virtual void setPosition(const glm::vec3& position);
	virtual void rotate(float yaw, float pitch); // not doing rolling!!!!
	virtual void move(const glm::vec3& offsetPos);

private:
	void updateCameraVectors();
};

#endif