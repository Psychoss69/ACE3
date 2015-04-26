#include "lod_info.hpp"

#include "read_helpers.hpp"

namespace ace {
    namespace p3d {


        lod::lod() {}
        lod::lod(std::istream & stream_, uint32_t id_) : id(id_) {
            uint32_t temp_count;

            // proxies
            stream_.read((char *)&temp_count, sizeof(uint32_t)); assert(temp_count < 4096 * 10);
            for (int x = 0; x < temp_count; x++) {
                proxies.push_back(std::make_shared<proxy>(stream_));
            }
            LOG(DEBUG) << "Found Proxies:";
            for (auto & proxy : proxies) {
                LOG(DEBUG) << "\t" << proxy->name;
            }
            
            compressed<uint32_t> item(stream_, false, false);
            items = item.data;

            // bone links
            stream_.read((char *)&temp_count, sizeof(uint32_t)); assert(temp_count < 4096 * 10);
            for (int x = 0; x < temp_count; x++) {
                // @ TODO: BoneLInks
                uint32_t link_count;
                std::vector<uint32_t> links;
                stream_.read((char *)&link_count, sizeof(uint32_t));
                for (int x = 0; x < link_count; x++) {
                    uint32_t val;
                    stream_.read((char *)&val, sizeof(uint32_t));
                    links.push_back(val);
                }
                bone_links.push_back(links);
            }

            stream_.read((char *)&point_count, sizeof(uint32_t));
            stream_.read((char *)&u_float_1, sizeof(float));

            // Derp, this was only TOH apparently!?
            //point_flags = compressed<uint32_t>(stream_, true, true);

            stream_.read((char *)&u_float_2, sizeof(float));
            stream_.read((char *)&u_float_3, sizeof(float));
            
            min_pos = ace::vector3<float>(stream_);
            max_pos = ace::vector3<float>(stream_);
            autocenter_pos = ace::vector3<float>(stream_);

            stream_.read((char *)&sphere, sizeof(float));

            // Texture strings
            stream_.read((char *)&temp_count, sizeof(uint32_t));
            for (int x = 0; x < temp_count; x++) {
                std::string temp;

                READ_STRING(temp);
                textures.push_back(temp);
                LOG(DEBUG) << "Found texture: " << temp << " pos: " << stream_.tellg();
            }
            
            //Materials
            stream_.read((char *)&temp_count, sizeof(uint32_t));
            for (int x = 0; x < temp_count; x++) {
                 materials.push_back(std::make_shared<material>(stream_));
            }

            edges.mlod = compressed<uint16_t>(stream_, true, false).data;
            edges.vertex = compressed<uint16_t>(stream_, true, false).data;

            // @TODO: THIS IS OFF WTF?!
            // The first face is coming up null, so we missed something
            // Faces magic
            stream_.read((char *)&temp_count, sizeof(uint32_t));
            stream_.read((char *)&faces_allocation_size, sizeof(uint32_t));
            
            // WTF IS GOING ON!?
            stream_.seekg(2, stream_.cur);

            for (int x = 0; x < temp_count; x++) {
                faces.push_back(std::make_shared<face>(stream_));
            }

            // Sections
            stream_.read((char *)&temp_count, sizeof(uint32_t));
            for (int x = 0; x < temp_count; x++) {
                sections.push_back(std::make_shared<section>(stream_));
            }

            stream_.read((char *)&temp_count, sizeof(uint32_t));
            for (int x = 0; x < temp_count; x++) {
                selections.push_back(std::make_shared<named_selection>(stream_));
            }
            
            // named properties
            LOG(DEBUG) << "Loaded properties";
            stream_.read((char *)&temp_count, sizeof(uint32_t));
            for (int x = 0; x < temp_count; x++) {
                std::string key, value;
                READ_STRING(key);
                READ_STRING(value);
                named_properties[key] = value;
                LOG(DEBUG) << "\t\t" << key << "->" << value;
            }

            stream_.read((char *)&temp_count, sizeof(uint32_t));
            for (int x = 0; x < temp_count; x++) {
                frames.push_back(std::make_shared<frame>(stream_));
            }

            stream_.read((char *)&icon_color, sizeof(uint32_t));
            stream_.read((char *)&selected_color, sizeof(uint32_t));
            stream_.read((char *)&u_residue, sizeof(uint32_t));
            stream_.read((char *)&u_byte_1, sizeof(uint8_t));
            stream_.read((char *)&temp_count, sizeof(uint32_t));

            // Vertex Table starts here
            vertices = std::make_shared<c_vertex_table>(stream_, temp_count);
        }

        lod::~lod() {}

        uv::uv() {}
        uv::uv(std::istream &stream_) {
            stream_.read((char *)&uv_scale, sizeof(float) * 4);
            data = compressed<float>(stream_, true, true);
        }

        c_vertex_table::c_vertex_table() {}
        c_vertex_table::c_vertex_table(std::istream &stream_, uint32_t size_) : size(size_) {
            uint32_t temp_count;

            point_flags = compressed<uint32_t>(stream_, true, true);

            uvsets.push_back(uv(stream_));

            // UV optionala additional sets
            stream_.read((char *)&temp_count, sizeof(uint32_t));
            for (int x = 0; x < temp_count-1; x++) {
                uvsets.push_back(uv(stream_));
            }

            // Points
            stream_.read((char *)&temp_count, sizeof(uint32_t));
            for (int x = 0; x < temp_count; x++) {
                points.push_back(ace::vector3<float>(stream_));
            }
        }

        named_selection::named_selection() {}
        named_selection::named_selection(std::istream &stream_) {
            uint32_t count;

            READ_STRING(name);

            stream_.read((char *)&count, sizeof(uint32_t));
            for (int x = 0; x < count; x++) {
                uint16_t temp;
                stream_.read((char *)&temp, sizeof(uint16_t));
                faces.push_back(temp);
            }

            stream_.read((char *)&Always0Count, sizeof(uint32_t));
            assert(Always0Count == 0);

            READ_BOOL(is_selectional);

            stream_.read((char *)&count, sizeof(uint32_t));
            for (int x = 0; x < count; x++) {
                uint32_t temp;
                stream_.read((char *)&temp, sizeof(uint32_t));
                sections.push_back(temp);
            }

            stream_.read((char *)&count, sizeof(uint32_t));
            for (int x = 0; x < count; x++) {
                uint16_t temp;
                stream_.read((char *)&temp, sizeof(uint16_t));
                vertex_table.push_back(temp);
            }

            stream_.read((char *)&count, sizeof(uint32_t));
            for (int x = 0; x < count; x++) {
                uint8_t temp;
                stream_.read((char *)&temp, sizeof(uint8_t));
                vertices_weights.push_back(temp);
            }
        }


        section::section() {}
        section::section(std::istream &stream_) {
            stream_.read((char *)&face_offsets, sizeof(uint32_t) * 2);
            stream_.read((char *)&material_offsets, sizeof(uint32_t) * 2);

            stream_.read((char *)&common_points_user_value, sizeof(uint32_t));
            stream_.read((char *)&common_texture, sizeof(uint16_t));
            stream_.read((char *)&common_face_flags, sizeof(uint32_t));

            stream_.read((char *)&material_index, sizeof(int32_t));
            if (material_index == -1) {
                stream_.read((char *)&extra, sizeof(uint8_t));
            }

            stream_.read((char *)&u_long_1, sizeof(uint32_t));
            stream_.read((char *)&u_float_resolution_1, sizeof(float));
            stream_.read((char *)&u_float_resolution_2, sizeof(float));
        }

        face::face() { }
        face::face(std::istream & stream_) {
            stream_.read((char *)&type, sizeof(uint8_t));
            assert(type == 3 || type == 4);
            for (int x = 0; x < type; x++) {
                uint16_t val;
                stream_.read((char *)&val, sizeof(uint16_t));
                vertex_table.push_back(val);
            }
        }

        uint32_t                         flags;            //ODOL7 ONLY see P3D Point and Face Flags
        uint16_t                         texture;         //ODOL7 ONLY
        uint8_t                          type;             // 3==Triangle or 4==Box
        std::vector<uint16_t>            vertex_table;

        stage_texture::stage_texture() : file(""), filter(0), transform_id(0) { }
        stage_texture::stage_texture(std::istream &stream_, uint32_t type_) {
            stream_.read((char *)&filter, sizeof(uint32_t));
            READ_STRING(file);
            stream_.read((char *)&transform_id, sizeof(uint32_t));
            if (type_ == 11) {
                READ_BOOL(wtf);
            }
        }

        material::material() { }
        material::material(std::istream &stream_) {
            uint32_t textures_count, transforms_count;

            READ_STRING(name);

            stream_.read((char *)&type, sizeof(uint32_t));

            stream_.read((char *)&emissive, sizeof(float) * 4);
            stream_.read((char *)&ambient, sizeof(float) * 4);
            stream_.read((char *)&diffuse, sizeof(float) * 4);
            stream_.read((char *)&forced_diffuse, sizeof(float) * 4);
            stream_.read((char *)&specular, sizeof(float) * 4);
            stream_.read((char *)&specular_2, sizeof(float) * 4);
            
            stream_.read((char *)&specular_power, sizeof(float));
            stream_.read((char *)&pixel_shader, sizeof(uint32_t));
            stream_.read((char *)&vertex_shader, sizeof(uint32_t));
            stream_.read((char *)&u_long_1, sizeof(uint32_t));
            stream_.read((char *)&an_index, sizeof(uint32_t));
            stream_.read((char *)&u_long_2, sizeof(uint32_t));

            READ_STRING(surface);

            stream_.read((char *)&u_long_3, sizeof(uint32_t));
            stream_.read((char *)&render_flags, sizeof(uint32_t));

            stream_.read((char *)&textures_count, sizeof(uint32_t));
            stream_.read((char *)&transforms_count, sizeof(uint32_t));

            for (int x = 0; x < textures_count; x++) {
                texture_stages.push_back(std::make_shared<stage_texture>(stream_, type));
            }

            for (int x = 0; x < textures_count; x++) {
                uint32_t uv_source;
                stream_.read((char *)&uv_source, sizeof(uint32_t));
                transform_stages.push_back(std::pair<uint32_t, transform_matrix>(uv_source, transform_matrix(stream_)));
            }
            if (type >= 10) {
                texture_stages.push_back(std::make_shared<stage_texture>(stream_, type));
            }
        }

        frame::frame() {}
        frame::frame(std::istream &stream_) {
            uint32_t count;

            stream_.read((char *)&time, sizeof(float));

            stream_.read((char *)&count, sizeof(uint32_t));
            for (int x = 0; x < count; x++) {
                bone_positions.push_back(ace::vector3<float>(stream_));
            }
        }

        proxy::proxy() { }
        proxy::proxy(std::istream &stream_) {
            READ_STRING(name);
            transform = ace::transform_matrix(stream_);
            stream_.read((char *)&sequence_id, sizeof(uint32_t));
            stream_.read((char *)&named_selection_id, sizeof(uint32_t));
            stream_.read((char *)&bone_id, sizeof(uint32_t));
            stream_.read((char *)&section_id, sizeof(uint32_t));

            LOG(DEBUG) << "Proxy: [" << name << "]";

        }
    }
}