#include "Camera.h"

// TODO: fill up the following function properly 
void Camera::set_rotation(const glm::quat& _q)
{
  glm::mat4 rotationMatrix = glm::mat4_cast(_q);
  this->set_pose(rotationMatrix);
}

// TODO: re-write the following function properly 
const glm::quat Camera::get_rotation() const
{
  glm::mat4 viewMatrix = this->get_view_matrix();
  return glm::quat_cast(viewMatrix);
}

// TODO: fill up the following function properly 
void Camera::set_pose(const glm::quat& _q, const glm::vec3& _t)
{
  // glm::mat4 rotationMatrix = glm::mat4_cast(_q);

  // rotationMatrix[3][0] = -_t[0];
  // rotationMatrix[3][1] = -_t[1];
  // rotationMatrix[3][2] = _t[2];

  // this->set_pose(rotationMatrix);

  this->set_rotation(_q);
  this->set_position(_t);
}

// TODO: fill up the following function properly 
void Camera::get_pose(glm::quat& _q, glm::vec3& _t) const
{
  // // this->set_pose(_q, _t);
  // glm::mat4 rotationMatrix = glm::mat4_cast(_q);
  // rotationMatrix[3][0] = -_t[0];
  // rotationMatrix[3][1] = -_t[1];
  // rotationMatrix[3][2] = _t[2];

  _q = this->get_rotation();
  _t = this->position();
}

// TODO: rewrite the following function properly 
const glm::mat4 Camera::get_pose() const
{
  return this->get_view_matrix();
}

// TODO: fill up the following function properly 
void Camera::set_pose(const glm::mat4& _frame)
{
  // +x_cam: right direction of the camera    (it should be a unit vector whose length is 1)
  right_dir_ = glm::vec3(_frame[0][0], _frame[1][0], _frame[2][0]); // _frame[0]; // +x
  // +y_cam: up direction of the camera       (it should be a unit vector whose length is 1)   
  up_dir_    = glm::vec3(_frame[0][1], _frame[1][1], _frame[2][1]);// _frame[1];    // +y
  // -z_cam: front direction of the camera    (it should be a unit vector whose length is 1)
  front_dir_ = glm::vec3(-_frame[0][2], -_frame[1][2], -_frame[2][2]);//_frame[2];    // -z
  // pos_cam: position of the camera
  position_  = glm::vec3(-_frame[3][0], -_frame[3][1], -_frame[3][2]);    // pos
}

// TODO: fill up the following function properly 
void Camera::set_pose(const glm::vec3& _pos, const glm::vec3& _at, const glm::vec3& _up_dir)
{
  glm::vec3 camDirection = glm::normalize(_at - _pos); // Opposite of the camera's actual orientation
  
  up_dir_    = _up_dir;
  front_dir_ = -camDirection;    // -z_cam direction
  right_dir_ = glm::normalize(glm::cross(up_dir_, front_dir_));    // +x_cam direction
  up_dir_    = glm::cross(front_dir_, right_dir_);    // +y_cam direction

  position_  = _at;      // pos
}

// TODO: rewrite the following function properly 
const glm::mat4 Camera::get_view_matrix() const
{
  glm::mat4 lookAt = glm::lookAt(this->position(), this->position() + front_dir_, up_dir_);
  return lookAt;
}

// TODO: rewrite the following function properly 
const glm::mat4 Camera::get_projection_matrix() const
{
  // TODO: Considering the followings,
  //       You must return a proper projection matrix
  //       i) camera mode: it can be either kOrtho or kPerspective
  //       ii) zoom-in/-out: if the camera mode is kOrtho, 
  //                         utilize ortho_scale_ for zoom-in/-out
  //                         if the camera mode is kPerspective,
  //                         utilize fovy_ for zoom-in/-out
  //       iii) aspect ratio: utilize aspect_ in the both camera modes
  //       iv) near/far clipping planes: utilize near_, far_

  if (mode_ == kPerspective)
  {
    return glm::perspective(glm::radians(fovy_), aspect_, near_, far_);
  }
  else
  {
    return glm::ortho(-ortho_scale_ * this->aspect_, ortho_scale_ * this->aspect_, -ortho_scale_, ortho_scale_, near_, far_);
  }
}

// TODO: fill up the following functions properly 
void Camera::move_forward(float delta)
{
  this->set_position(this->position() + front_dir_ * delta);
}

// TODO: fill up the following functions properly 
void Camera::move_backward(float delta)
{
  this->set_position(this->position() - front_dir_ * delta);
}

// TODO: fill up the following functions properly 
void Camera::move_left(float delta)
{
  // glm::vec3 dir = glm::normalize(glm::cross(front_dir_, up_dir_)) * delta;
  this->set_position(this->position() - (right_dir_ * delta));
}

// TODO: fill up the following functions properly 
void Camera::move_right(float delta)
{
  this->set_position(this->position() + (right_dir_ * delta));
}

// TODO: fill up the following functions properly 
void Camera::move_up(float delta)
{
  this->set_position(this->position() + (up_dir_ * delta));
}

// TODO: fill up the following functions properly 
void Camera::move_down(float delta)
{
  this->set_position(this->position() - (up_dir_ * delta));
}