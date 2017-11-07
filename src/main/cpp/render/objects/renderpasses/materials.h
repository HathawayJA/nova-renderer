/*!
 * \brief Contains struct to represent .material files
 * \author ddubois 
 * \date 01-Nov-17.
 */

#ifndef RENDERER_MATERIALS_H
#define RENDERER_MATERIALS_H

#include <string>
#include <vector>
#include <json.hpp>

namespace nova {
    /*!
     * \brief Controlls the rasterizer's state
     */
    enum class state_enum {
        /*!
         * \brief Enable blending for this material state
         */
        blending,

        /*!
         * \brief Render backfaces and cull frontfaces
         */
        invert_culing,

        /*!
         * \brief Don't cull backfaces or frontfaces
         */
        disable_culling,

        /*!
         * \brief Don't write to the depth buffer
         */
        disable_depth_write,

        /*!
         * \brief Perform the stencil test
         */
        enable_stencil_test,

        /*!
         * \brief Write to the stencil buffer
         */
        stencil_write,

        /*!
         * \brief Don't write to the color buffer
         */
        disable_color_write,

        /*!
         * \brief Enable alpha to coverage
         */
        enable_alpha_to_coverage
    };

    enum class texture_filter_enum {
        texel_aa,
        bilinear,
        point
    };

    enum class texture_wrap_mode_enum {
        repeat,
        clamp
    };

    /*!
     * \brief Defines a sampler to use for a texture
     *
     * At the time of writing I'm not sure how this is corellated with a texture, but all well
     */
    struct sampler_state {
        /*!
         * \brief The index of the sampler. This might correspond directly with the texture but I hope not cause I don't
         * want to write a number of sampler blocks
         */
        uint32_t sampler_index;

        /*!
         * \brief What kind of texture filter to use
         *
         * texel_aa does something that I don't want to figure out right now. Bilinear is your regular bilinear filter,
         * and point is the point filter. Aniso isn't an option and I kinda hope it stays that way
         */
        texture_filter_enum filter;

        /*!
         * \brief How the texutre should wrap at the edges
         */
        texture_wrap_mode_enum wrap_mode;
    };

    /*!
     * \brief The kind of data in a vertex attribute
     */
    enum class vertex_field {
        /*!
         * \brief The vertex position
         *
         * 12 bytes
         */
        position,

        /*!
         * \brief The vertex color
         *
         * 4 bytes
         */
        color,

        /*!
         * \brief The UV coordinate of this object
         *
         * Except not really, because Nova's virtual textures means that the UVs for a block or entity or whatever
         * could change on the fly, so this is kinda more of a preprocessor define that replaces the UV with a lookup
         * in the UV table
         *
         * 8 bytes (might try 4)
         */
        main_uv,

        /*!
         * \brief The UV coordinate in the lightmap texture
         *
         * This is a real UV and it doesn't change for no good reason
         *
         * 2 bytes
         */
        lightmap_uv,

        /*!
         * \brief Vertex normal
         *
         * 12 bytes
         */
        normal,

        /*!
         * \brief Vertex tangents
         *
         * 12 bytes
         */
        tangent,

        /*!
         * \brief The texture coordinate of the middle of the quad
         *
         * 8 bytes
         */
        mid_tex_coord,

        /*!
         * \brief A uint32_t that's a unique identifier for the texture that this vertex uses
         *
         * This is generated at runtime by Nova, so it may change a lot depending on what resourcepacks are loaded and
         * if they use CTM or random detail textures or whatever
         *
         * 4 bytes
         */
        virtual_texture_id,

        /*!
         * \brief Some information about the current block/entity/whatever
         *
         * 12 bytes
         */
        mc_entity_id,

        /*!
         * \brief Useful if you want to skip a vertex attribute
         */
        empty
    };

    enum class texture_location_enum {
        dynamic,
        in_user_package
    };

    /*!
     * \brief A texture definition in a material file
     *
     * This class simple describes where to get the texture data from.
     */
    struct texture {
        /*!
         * \brief The index of the texture
         *
         * In pure Bedrock mode this is part of the texture name, e.g. setting the index to 0 means the texture will be
         * bound to texture name TEXTURE0. If you don't name your textures according to this format, then the index is
         * the binding point of the texture, so an index of 0 would put this texture at binding point 0
         */
        uint32_t index;

        /*!
         * \brief Where Nova should look for the texture at
         *
         * The texture location currently has two values: Dynamic and InUserPackage.
         *
         * A Dynamic texture is generated at runtime, often as part of the rendering pipeline. There are a number of
         * dynamic textures defined by Nova, which I don't feel like listing out here.
         *
         * A texture that's InUserPackage is not generated at runtime. Rather, it's supplied by the resourcepack. A
         * InUserPackage texture can have a name that's the path, relative to the resourcepack, of where to find the
         * texture, or it can refer to an atlas. Because of the way Nova handles megatextures there's actually always
         * three atlases: color, normals, and data. Something like `atlas.terrain` or `atlas.gui` refer to the color
         * atlas. Think of the texture name as more of a guideline then an actual rule
         */
        texture_location_enum texture_location;

        /*!
         * \brief The name of the texture
         *
         * If the texture name starts with `atlas` then the texture is one of the atlases. Nova sticks all the textures
         * it can into the virtual texture atlas, so it doesn't really care what atlas you request.
         */
        std::string texture_name;

        /*!
         * \brief If true, calculates mipmaps for this texture before the shaders is drawn
         */
        bool calculate_mipmaps;
    };

    /*!
     * \brief Tells Nova what framebuffer attachments you output to, and what format you expect them to be in
     */
    struct framebuffer_output {
        /*!
         * \brief The index of the framebuffer attachment you're writing to. You don't get more than 8, no matter how
         * loud you yell
         */
        uint8_t index;

        /*!
         * \brief The format of the framebuffer attachment that you want to write to. This should be one of the formats
         * that Vulkan supports, and it should almost always be RGBA, but I don't feel like writing them all out
         */
        std::string format;
    };

    /*!
     * \brief Represents the configuration for a single pipeline
     */
    struct material_state {
        /*!
         * \brief The name of this material_state
         */
        std::string name;

        /*!
         * \brief The material_state that this material_state inherits from
         *
         * I may or may not make this a pointer to another material_state. Depends on how the code ends up being
         */
        std::string parent;

        /*!
         * \brief All of the symbols in the shader that are defined by this state
         */
        std::vector<std::string> defines;

        /*!
         * \brief Defines the rasterizer state that's active for this material state
         */
        std::vector<state_enum> states;

        /*!
         * \brief The path from the resourcepack or shaderpack root to the vertex shader
         *
         * Except not really, cause if you leave off the extension then Nova will try using the `.vert` and `.vsh`
         * extensions. This is kinda just a hint
         */
        std::string vertex_shader;

        /*!
         * \brief The path from the resourcepack or shaderpack root to the fragment shader
         *
         * Except not really, cause if you leave off the extension then Nova will try using the `.frag` and `.fsh`
         * extensions. This is kinda just a hint
         */
        std::string fragment_shader;

        /*!
         * \brief The path from the resourcepack or shaderpack root to the geometry shader
         *
         * Except not really, cause if you leave off the extension then Nova will try using the `.geom` and `.geo`
         * extensions. This is kinda just a hint
         */
        std::string geometry_shader;

        /*!
         * \brief The path from the resourcepack or shaderpack root to the tessellation evaluation shader
         *
         * Except not really, cause if you leave off the extension then Nova will try using the `.test` and `.tse`
         * extensions. This is kinda just a hint
         */
        std::string tessellation_evaluation_shader;

        /*!
         * \brief The path from the resourcepack or shaderpack root to the tessellation control shader
         *
         * Except not really, cause if you leave off the extension then Nova will try using the `.tesc` and `.tsc`
         * extensions. This is kinda just a hint
         */
        std::string tessellation_control_shader;

        /*!
         * \brief Sets up the vertex fields that Nova will bind to this shader
         *
         * The index in the array is the attribute index that the vertex field is bound to
         */
        std::vector<vertex_field> vertex_fields;

        /*!
         * \brief All the sampler states that are defined for this material_state. Still not sure how they work though
         */
        std::vector<sampler_state> sampler_states;

        /*!
         * \brief All the textures that this material state uses
         */
        std::vector<texture> textures;

        /*!
         * \brief The filter string used to get data for this material_state
         */
        std::string filters;

        /*!
         * \brief The material_state to use if this one's shaders can't be found
         */
        std::string fallback;

        /*!
         * \brief When this material state will be drawn
         *
         * Lower pass indices are drawn earlier, and larger pass indices are drawn later. If multiple material states
         * have the same pass index then Nova makes no guarantees about when they will be drawn relative to each other.
         * Pass indices to not have to be continuous
         */
        uint32_t pass_index;

        /*!
         * \brief The framebuffer attachments that this material pass outputs to
         *
         * The index in this array is the location of the output in the shader, and the index member of the
         * frameuffer_output struct is the index in the framebuffer. For example, a framebuffer_output at index 2 in
         * this array with an index member of 4 tells Nova that when the shader associated with this material state
         * outputs to location 2, that data should be written to colortex4. Alteriately, you can think of it as telling
         * Nocva to bind colortex4 to shader output 2
         */
        std::vector<framebuffer_output> outputs;

        /*!
         * \brief The width of the output texture we're rendering to
         *
         * If this is not set by the .material file, then its value comes from the framebuffer that it renders to. I
         * mostly put this member in this struct as a convenient way to pass it into a shader creation
         */
        uint32_t output_width;

        /*!
         * \brief The height of the output texture we're rendering to
         *
         * If this is not set by the .material file, then its value comes from the framebuffer that it renders to. I
         * mostly put this member in this struct as a convenient way to pass it into a shader creation
         */
        uint32_t output_height;
    };

    material_state create_material_from_json(const std::string& material_state_name, const std::string& parent_state_name, const nlohmann::json& material_json);

    /*!
     * \brief Translates a string from a material file to a state_enum value
     * \param state_to_decode The string to translate to a state
     * \return The decoded state
     */
    state_enum decode_state(const std::string& state_to_decode);
}

#endif //RENDERER_MATERIALS_H