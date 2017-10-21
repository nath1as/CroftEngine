#pragma once

#include "ShaderProgram.h"

#include <glm/glm.hpp>

#include <boost/optional.hpp>

namespace gameplay
{
    class Node;


    class MaterialParameter
    {
    public:
        explicit MaterialParameter(const std::string& name);

        ~MaterialParameter();

        const std::string& getName() const;

        void set(float value);

        void set(int value);

        void set(const float* values, std::size_t count = 1);

        void set(const int* values, std::size_t count = 1);

        void set(const glm::vec2& value);

        void set(const glm::vec2* values, std::size_t count = 1);

        void set(const glm::vec3& value);

        void set(const glm::vec3* values, std::size_t count = 1);

        void set(const glm::vec4& value);

        void set(const glm::vec4* values, std::size_t count = 1);

        void set(const glm::mat4& value);

        void set(const glm::mat4* values, std::size_t count = 1);

        void set(const std::shared_ptr<gl::Texture>& texture);

        void set(const std::vector<std::shared_ptr<gl::Texture>>& textures);

        template<class ClassType, class ValueType>
        void bind(ClassType* classInstance, ValueType (ClassType::*valueMethod)() const)
        {
            m_valueSetter = [classInstance, valueMethod](const Node& /*node*/, gl::Program::ActiveUniform& uniform) {
                uniform.set( (classInstance->*valueMethod)() );
            };
        }

        using UniformValueSetter = void(const Node& node, gl::Program::ActiveUniform& uniform);

        void bind(std::function<UniformValueSetter>&& setter)
        {
            m_valueSetter = move( setter );
        }

        template<class ClassType, class ValueType>
        void bind(ClassType* classInstance, ValueType (ClassType::*valueMethod)() const,
                  std::size_t (ClassType::*countMethod)() const)
        {
            m_valueSetter = [classInstance, valueMethod, countMethod](const Node& /*node*/,
                                                                      const gl::Program::ActiveUniform& uniform) {
                uniform.set( (classInstance->*valueMethod)(), (classInstance->*countMethod)() );
            };
        }

        void bindModelMatrix();

        void bindViewMatrix();

        void bindModelViewMatrix();

        void bindProjectionMatrix();

        bool bind(const Node& node, const std::shared_ptr<ShaderProgram>& shaderProgram);

    private:

        MaterialParameter& operator=(const MaterialParameter&) = delete;

        gl::Program::ActiveUniform* getUniform(const std::shared_ptr<ShaderProgram>& shaderProgram);

        enum LOGGER_DIRTYBITS
        {
            UNIFORM_NOT_FOUND = 0x01,
            PARAMETER_VALUE_NOT_SET = 0x02
        };

        const std::string m_name;

        boost::optional<std::function<UniformValueSetter>> m_valueSetter;

        uint8_t m_loggerDirtyBits = 0;
    };
}
