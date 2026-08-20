#pragma once
#include "rendering/mesh.h"
#include "rendering/material.h"

#include "core/math.h"

#include <memory>

class Model {
public:
    Model(std::shared_ptr<Mesh> mesh, std::shared_ptr<Material> material) : m_mesh(mesh), m_material(material) {}

    std::shared_ptr<Mesh> getMesh() const { return m_mesh; }
    std::shared_ptr<Material> getMaterial() const { return m_material; }

    void setMesh(std::shared_ptr<Mesh> mesh) { m_mesh = mesh; }
    void setMaterial(std::shared_ptr<Material> material) { m_material = material; }

    Vec3 getPosition() const { return m_position; }
    void setPosition(Vec3 position) { m_position = position; }

private:
    std::shared_ptr<Mesh> m_mesh;
    std::shared_ptr<Material> m_material;

    Vec3 m_position;
};