export module RenderLayer;

import std;
import raylib;

export namespace RenderLayer {
    class Layer {
    public:
        Layer(int texture_width, int texture_height, float camera_width, float camera_height);
        ~Layer();
        Layer(const Layer&) = delete;
        Layer& operator=(const Layer&) = delete;
        Layer(Layer&&) = default;
        Layer& operator=(Layer&&) = default;

        void begin() const;
        void end() const;

        raylib::Rectangle fit_rect(const raylib::Rectangle& bounds) const;
        raylib::Rectangle fit_rect_top_left(const raylib::Rectangle& bounds) const;

        void draw(const raylib::Rectangle& dest_rect) const;
        const raylib::RenderTexture2D& target() const { return m_target; }

    private:
        int m_texture_width;
        int m_texture_height;
        raylib::RenderTexture2D m_target;
        raylib::Camera2D m_camera;
    };

}

module :private;

namespace RenderLayer {

Layer::Layer(int texture_width, int texture_height, float camera_width, float camera_height):
    m_texture_width(texture_width),
    m_texture_height(texture_height),
    m_target(raylib::LoadRenderTexture(texture_width, texture_height)),
    m_camera(raylib::Camera2D{
        .offset = {0.0f, 0.0f},
        .target = {0.0f, 0.0f},
        .rotation = 0.0f,
        .zoom = std::min(static_cast<float>(texture_width) / camera_width,
                         static_cast<float>(texture_height) / camera_height),
    })
{}

Layer::~Layer() {
    raylib::UnloadRenderTexture(m_target);
}

void Layer::begin() const {
    raylib::BeginTextureMode(m_target);
    raylib::BeginMode2D(m_camera);
}

void Layer::end() const {
    raylib::EndMode2D();
    raylib::EndTextureMode();
}

raylib::Rectangle Layer::fit_rect(const raylib::Rectangle& bounds) const {
    const auto scale = std::min(bounds.width / static_cast<float>(m_texture_width),
                                bounds.height / static_cast<float>(m_texture_height));

    return raylib::Rectangle{
        bounds.x + (bounds.width - m_texture_width * scale) / 2.0f,
        bounds.y + (bounds.height - m_texture_height * scale) / 2.0f,
        m_texture_width * scale,
        m_texture_height * scale,
    };
}

raylib::Rectangle Layer::fit_rect_top_left(const raylib::Rectangle& bounds) const {
    const auto scale = std::min(bounds.width / static_cast<float>(m_texture_width),
                                bounds.height / static_cast<float>(m_texture_height));

    return raylib::Rectangle{
        bounds.x,
        bounds.y,
        m_texture_width * scale,
        m_texture_height * scale,
    };
}

void Layer::draw(const raylib::Rectangle& dest_rect) const {
    raylib::DrawTexturePro(m_target.texture,
                            {0, 0,
                            static_cast<float>(m_texture_width),
                            -static_cast<float>(m_texture_height)},
                            dest_rect,
                            {0, 0},
                            0.0f,
                            raylib::WHITE);
}

}