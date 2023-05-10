#include "Camera.h"
// #include <glm/gtx/quaternion.hpp>
#include <iostream>

// TODO: fill up the following function properly 
void Camera::set_rotation(const glm::quat& _q)
{
  // std::cout << "_q (" << _q.w << ", " <<_q.x << ", " << _q.y << ", " << _q.z << ")" << std::endl;
  // glm::mat4 viewMatrix = this->get_view_matrix();
  // glm::mat3 rotationMatrix = glm::mat3(_q);
  // std::cout << "rotationMatrix (" << rotationMatrix[0][0] << ", " << rotationMatrix[0][1] << ", " << rotationMatrix[0][2] << ")" << std::endl;

  // viewMatrix[0] = glm::vec4(rotationMatrix[0], 0.f);
  // viewMatrix[1] = glm::vec4(rotationMatrix[1], 0.f);
  // viewMatrix[2] = glm::vec4(rotationMatrix[2], 0.f);

  // this->set_pose(viewMatrix);

  glm::mat4 rotationMatrix = glm::mat4_cast(_q);
  this->set_pose(rotationMatrix);
}

// TODO: re-write the following function properly 
const glm::quat Camera::get_rotation() const
{
  glm::mat4 viewMatrix = this->get_view_matrix();
  // glm::mat3 rotationMatrix = viewMatrix;


  // return glm::quat(1.0f, 0.0f, 0.0f, 0.0f);
  return glm::quat_cast(viewMatrix);
}

// TODO: fill up the following function properly 
void Camera::set_pose(const glm::quat& _q, const glm::vec3& _t)
{
  // std::cout << "tempResult (" << _t[0] << ", " <<_t[1] << ", " << _t[2] << ")" << std::endl;

  glm::mat4 rotationMatrix = glm::mat4_cast(_q);
  // std::cout << "rotationMatrix (" << rotationMatrix[3][0] << ", " <<rotationMatrix[3][1] << ", " << rotationMatrix[3][2] << ", " << rotationMatrix[3][3] << ")" << std::endl;
  
  rotationMatrix[3][0] = -_t[0];
  rotationMatrix[3][1] = -_t[1];
  rotationMatrix[3][2] = _t[2];

  this->set_pose(rotationMatrix);

  // std::cout << "rotationMatrix_2 (" << rotationMatrix[3][0] << ", " <<rotationMatrix[3][1] << ", " << rotationMatrix[3][2] << ", " << rotationMatrix[3][3] << ")" << std::endl;
  // ===
  // needs optimiation
  // std::cout << "_q (" << _q.w << ", " <<_q.x << ", " << _q.y << ", " << _q.z << ")" << std::endl;
  // glm::mat4 viewMatrix = this->get_view_matrix();
  // glm::vec4 transPos = viewMatrix[3];

  // this->set_rotation(_q);
  // viewMatrix = this->get_view_matrix();
  // ==
  // //
  // transPos[0] -= _t.x;
  // transPos[1] -= _t.y;
  // transPos[2] -= _t.z;

  // viewMatrix[3] = transPos;
  // this->set_pose(viewMatrix); 
  // //
  // ===
  
  //glm::translate(this->get_view_matrix(), glm::vec3(0.f)));
  // glm::vec4 tempResult = glm::translate(this->get_view_matrix(), _t)[3];

  // std::cout << "tempResult (" << tempResult[0] << ", " <<tempResult[1] << ", " << tempResult[2] << ", " << tempResult[3] << ")" << std::endl;
  // glm::toMat4(_q);
}

// TODO: fill up the following function properly 
void Camera::get_pose(glm::quat& _q, glm::vec3& _t) const
{

}

// TODO: rewrite the following function properly 
const glm::mat4 Camera::get_pose() const
{
  return  glm::mat4(1.0f);
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

  // glm::vec3 tempRight = glm::vec3(_frame[0][0], _frame[1][0], _frame[2][0]);
  // glm::vec3 tempUp = glm::vec3(_frame[0][1], _frame[1][1], _frame[2][1]);
  // glm::vec3 tempFront = glm::vec3(_frame[0][2], _frame[1][2], _frame[2][2]);

  // std::cout << "=====" << std::endl;
  // std::cout << "tempRight (" << _frame[0][0]<< ", " << _frame[1][0] << ", " << _frame[2][0] << ")" << std::endl;
  // std::cout << "Right (" << right_dir_.x << ", " << right_dir_.y << ", " << right_dir_.z << ")" << std::endl;

  // std::cout << "tempFront (" << _frame[0][2]<< ", " << _frame[1][2] << ", " << _frame[2][2] << ")" << std::endl;
  // std::cout << "front_dir_ (" << front_dir_.x << ", " << front_dir_.y << ", " << front_dir_.z << ")" << std::endl;

  // std::cout << "tempUp (" << _frame[0][1]<< ", " << _frame[1][1] << ", " << _frame[2][1] << ")" << std::endl;
  // std::cout << "up_dir_ (" << up_dir_.x << ", " << up_dir_.y << ", " << up_dir_.z << ")" << std::endl;
}

// TODO: fill up the following function properly 
void Camera::set_pose(const glm::vec3& _pos, const glm::vec3& _at, const glm::vec3& _up_dir)
{
  glm::vec3 camDirection = glm::normalize(_at - _pos); // Opposite of the camera's actual orientation
  std::cout << "dir (" << camDirection.x << ", " << camDirection.y << ", " << camDirection.z << ")" << std::endl;

  up_dir_    = _up_dir;
  front_dir_ = -camDirection;    // -z_cam direction
  right_dir_ = glm::normalize(glm::cross(up_dir_, front_dir_));    // +x_cam direction
  up_dir_    = glm::cross(front_dir_, right_dir_);    // +y_cam direction

  std::cout << "front (" << front_dir_.x << ", " << front_dir_.y << ", " << front_dir_.z << ")" << std::endl;
  std::cout << "right (" << right_dir_.x << ", " << right_dir_.y << ", " << right_dir_.z << ")" << std::endl;
  std::cout << "up (" << up_dir_.x << ", " << up_dir_.y << ", " << up_dir_.z << ")" << std::endl;

  position_  = _at;      // pos
  this->set_position(_at);
  std::cout << "_at (" << _at.x << ", " << _at.y << ", " << _at.z << ")" << std::endl;
  std::cout << "position_ (" << position_.x << ", " << position_.y << ", " << position_.z << ")" << std::endl;

}

// TODO: rewrite the following function properly 
const glm::mat4 Camera::get_view_matrix() const
{
  // std::cout << this->position().x << this->position().y << this->position().z << std::endl;
  // std::cout << front_dir_.x << front_dir_.y << front_dir_.z << std::endl;
  // return glm::mat4(1.0f);
  // glm::vec3 tempForward = glm::vec3(0.f, 0.f, -1.f);
  glm::mat4 tempLookAt = glm::lookAt(this->position(), this->position() + front_dir_, up_dir_);
  // std::cout << "tempLookAt (" << tempLookAt[0][2]<< ", " << tempLookAt[1][2] << ", " << tempLookAt[2][2] << ")" << std::endl;

  return tempLookAt;
  // return glm::lookAt(this->position(), this->position() + front_dir_, up_dir_);
  // return glm::lookAt(this->position(), this->position() + front_dir_, up_dir_);
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
    // return glm::ortho()
    // return glm::mat4(1.0f);
  }
  // return glm::mat4(1.0f);
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
  // glm::vec3 currentPos = this->position();
  // currentPos.x -= delta;
  glm::vec3 dir = glm::normalize(glm::cross(front_dir_, up_dir_)) * delta;

  this->set_position(this->position() - (dir * delta));
}

// TODO: fill up the following functions properly 
void Camera::move_right(float delta)
{
  // glm::vec3 currentPos = this->position();
  // currentPos.x += delta;
  glm::vec3 dir = glm::normalize(glm::cross(front_dir_, up_dir_)) * delta;

  this->set_position(this->position() + (dir * delta));
}

// TODO: fill up the following functions properly 
void Camera::move_up(float delta)
{
  // std::cout << "origin (" << this->position().x << ", " << this->position().y << ", " << this->position().z << ")" << std::endl;
  
  glm::vec3 tempDir = up_dir_ * delta;
  // std::cout << "tempDir (" << tempDir.x << ", " << tempDir.y << ", " << tempDir.z << ")" << std::endl;

  glm::vec3 tempSum = this->position() + tempDir;
  // std::cout << "tempSum (" << tempSum.x << ", " << tempSum.y << ", " << tempSum.z << ")" << std::endl;
  this->set_position(this->position() + (up_dir_ * delta));
}

// TODO: fill up the following functions properly 
void Camera::move_down(float delta)
{
  this->set_position(this->position() - (up_dir_ * delta));
}
