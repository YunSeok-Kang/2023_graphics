#include "Object.h"

void Object::init_buffer_objects() 
{
    // std::cout<<sizeof(this->pmesh_->mVertices[0])<<std::endl;
    // std::cout<<this->pmesh_->mNumVertices<<std::endl;

    this->positions.resize(pmesh_->mNumVertices);
    memcpy(&this->positions[0], &this->pmesh_->mVertices[0], sizeof(this->pmesh_->mVertices[0]) * this->pmesh_->mNumVertices);

    this->colors.resize(pmesh_->mNumVertices);
    memcpy(&this->colors[0], &this->pmesh_->mColors[0][0], sizeof(this->pmesh_->mColors[0][0]) * this->pmesh_->mNumVertices);

    this->tv_indices.clear();

    for (unsigned int i = 0; i < this->pmesh_->mNumFaces; ++i)
    {
        aiFace& face = this->pmesh_->mFaces[i];

        for (unsigned int idx = 0; idx < face.mNumIndices - 2; ++idx)
        {
            tv_indices.push_back(face.mIndices[0]);
            tv_indices.push_back(face.mIndices[idx + 1]);
            tv_indices.push_back(face.mIndices[idx + 2]);
        }
    }

    tv_positions.resize(this->tv_indices.size());
    tv_colors.resize(this->tv_indices.size());

    // std::cout << tv_indices.size() << std::endl;
    // for (std::size_t i = 0; i < tv_indices.size(); i+=3)
    // {
    //     // set tv_positions and tv_flat_normals as each face normal
    //     tv_positions[i]   = positions[this->tv_indices[i]];
    //     tv_positions[i+1] = positions[this->tv_indices[i+1]];
    //     tv_positions[i+2] = positions[this->tv_indices[i+2]];

    //     tv_colors[i]   = colors[this->tv_indices[i]];
    //     tv_colors[i+1] = colors[this->tv_indices[i+1]];
    //     tv_colors[i+2] = colors[this->tv_indices[i+2]];

    //     // std::cout << tv_positions[i].x << " " << tv_positions[i].y << " " << tv_positions[i].z << std::endl;
    //     // std::cout << tv_colors[i].r << tv_colors[i].g << " " << tv_colors[i].b << " " << tv_colors[i].a << std::endl;
    // }
    for (std::size_t i = 0; i < tv_indices.size(); i+=3)
    {
        // set tv_positions and tv_flat_normals as each face normal
        tv_positions[i]   = positions[tv_indices[i]];
        tv_positions[i+1] = positions[tv_indices[i+1]];
        tv_positions[i+2] = positions[tv_indices[i+2]];

        tv_colors[i]   = colors[tv_indices[i]];
        tv_colors[i+1] = colors[tv_indices[i+1]];
        tv_colors[i+2] = colors[tv_indices[i+2]];
    }

    // generate GPU buffers
    glGenBuffers(1, &position_buffer);
    glGenBuffers(1, &color_buffer);

    // std::cout << "tv_positions: " << tv_positions.size() << std::endl;
    // std::cout << "tv_colors: " << tv_colors.size() << std::endl;

    // per-vertex position and normal data
    glBindBuffer(GL_ARRAY_BUFFER, position_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3)*tv_positions.size(), &tv_positions[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, this->color_buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4)*tv_colors.size(), &tv_colors[0], GL_STATIC_DRAW);
}

glm::mat4 Object::get_model_matrix() const
{
    glm::mat4 mat_model = glm::mat4(1.0f);

    glm::mat4 rot_mat;
    this->get_rotate(rot_mat);
    
    // TODO
    mat_model = mat_model * glm::translate(glm::mat4(1.0f), this->get_translate());
    mat_model = mat_model * rot_mat;
    mat_model = mat_model * glm::scale(glm::mat4(1.0f), this->get_scale());

    return mat_model;
}

void Object::draw(int loc_a_position, int loc_a_color)
{
    // TODO
    glBindBuffer(GL_ARRAY_BUFFER, this->position_buffer);
    glEnableVertexAttribArray(loc_a_position);
    glVertexAttribPointer(loc_a_position, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    // TODO : bind color buffer
    glBindBuffer(GL_ARRAY_BUFFER, this->color_buffer); 
    glEnableVertexAttribArray(loc_a_color);
    glVertexAttribPointer(loc_a_color, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

    glDrawArrays(GL_TRIANGLES, 0, (GLsizei) this->tv_positions.size());
    
    glDisableVertexAttribArray(loc_a_position);
    glDisableVertexAttribArray(loc_a_color);
}
    
void Object::print_info()
{
    std::cout << "print mesh info" << std::endl;

    std::cout << "num vertices " << pmesh_->mNumVertices << std::endl;
    for (unsigned int i = 0; i < pmesh_->mNumVertices; ++i)
    {
        aiVector3D vertex = pmesh_->mVertices[i];
        std::cout << "  vertex  (" << vertex.x << ", " << vertex.y << ", " << vertex.z << ")" << std::endl;

        if (pmesh_->mColors[0] != NULL)
        {
            aiColor4D color = pmesh_->mColors[0][i];
            std::cout << "  color  (" << color.r << ", " << color.g << ", " << color.b << ", " << color.a << ")" << std::endl;
        }
    }
}