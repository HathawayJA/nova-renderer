/*!
 * \brief Defines a shader program ("program" im OpenGL parlace) and a number of exceptions it can throw
 *
 * \author David
 * \date 17-May-16.
 */

#ifndef RENDERER_GL_SHADER_H
#define RENDERER_GL_SHADER_H

#include <istream>
#include <unordered_map>
#include <vector>

#include <glad/glad.h>
#include "../../../utils/export.h"
#include "../../../data_loading/loaders/shader_source_structs.h"


namespace nova {
    /*!
     * \brief Represents an error in compiling a shader
     */
    class compilation_error : public std::runtime_error {
    public:
        /*!
         * \brief Constructs a compilation_error with the provided message, using the given list of shader_lines to
         * map from line number in the error message to line number and shader file on disk
         *
         * \param error_message The compilation message, straight from the driver
         * \param source_lines The list of source_line objects that maps from line in the shader sent to the driver
         * to the line number and shader file on disk
         */
        compilation_error(const std::string &error_message, const std::vector<shader_line> source_lines);

    private:

        std::string
        get_original_line_message(const std::string &error_message, const std::vector<shader_line> source_lines);
    };

    class wrong_shader_version : public std::runtime_error {
    public:
        wrong_shader_version(const std::string &version_line);
    };

    class program_linking_failure : public std::runtime_error {
    public:
        program_linking_failure(const std::string name) : std::runtime_error("Program " + name + " failed to link") {};
    };

    /*!
     * \brief Represents an OpenGL shader program
     *
     * Shader programs can include between two and five shaders. At the bare minimum, a shader program needs a vertex shader
     * and a fragment shader. A shader program can also have a geometry shader, a tessellation control shader, and a
     * tessellation evaluation shader. Note that if a shader program has one of the tessellation shaders, it must also have
     * the other tessellation shader.
     *
     * A gl_shader_program does a couple of things. First, it holds a reference to the OpenGL object. Second, it holds all
     * the configuration options declared in the shader. Third, it possibly holds the uniforms and attributes defined in
     * this shader. There's a good chance that I won't end up with uniform and attribute information. This class will also
     * hold the map from line in the shader sent to the compiler and the line number and shader file that the line came from
     * on disk
     */
    class gl_shader_program {
    public:
        GLuint gl_name;

        /*!
         * \brief Constructs a gl_shader_program
         */
        explicit gl_shader_program(const shader_definition &source);

        /*!
         * \brief Default copy constructor
         *
         * \param other The thing to copygit add -A :/
         */
		gl_shader_program(const gl_shader_program &other) = default;

        /**
         * \brief Move constructor
         *
         * I expect that this constructor will only be called when returning a fully linked shader from a builder function.
         * If this is not the case, this will throw an error. Be watchful.
         */
        gl_shader_program(gl_shader_program &&other) noexcept;

        gl_shader_program() = default;

        /*!
         * \brief Deletes this shader and all it holds dear
         */
        ~gl_shader_program();

        /*!
         * \brief Sets this shader as the currently active shader
         */
        void bind() noexcept;

        std::string& get_filter() noexcept;

        std::string& get_name() noexcept;

        /*!
         * \brief Finds the uniform location of the given uniform variable
         *
         * The first time this method is called for a given string, it calls glGetUniformLocation to get the uniform
         * location. The result of that function is then cached so that glGetUniformLocation only needs to be called
         * once for every uniform variable, no matter how many times you upload data to that variable
         *
         * \param uniform_name The name of the uniform variable to get the location of
         * \return The location of the desired uniform variable
         */
        GLint get_uniform_location(std::string uniform_name);

    private:
        std::string name;

        std::vector<GLuint> added_shaders;

        std::unordered_map<std::string, GLint> uniform_locations;

        /*!
         * \brief The filter that the renderer should use to get the geometry for this shader
         *
         * Since there's a one-to-one correlation between shaders and filters, I thought it'd be best to put the
         * filter with the shader
         */
        std::string filter;

        void create_shader(const std::vector<shader_line>& shader_source, GLenum shader_type);

        void check_for_shader_errors(GLuint shader_to_check, const std::vector<shader_line>& line_map);

        void link();

        void check_for_linking_errors();
    };
}

#endif //RENDERER_GL_SHADER_H
