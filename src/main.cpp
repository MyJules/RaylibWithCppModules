import std;
import args;
import raylib;
import RenderLayer;

int main(int argc, const char* argv[]) {
    const auto args = args::to_span(argc, argv);

    constexpr auto virtual_width = 320;
    constexpr auto virtual_height = 240;

    raylib::SetConfigFlags(raylib::FLAG_WINDOW_RESIZABLE); 
    raylib::InitWindow(1280, 720, "raylib modules");

    auto world_layer = RenderLayer::Layer(virtual_width, virtual_height, 1920, 1080);
    raylib::SetTextureFilter(world_layer.target().texture, raylib::TextureFilter::TEXTURE_FILTER_POINT);

    constexpr auto ui_width = 1920;
    constexpr auto ui_height = 1080;

    auto ui_layer = RenderLayer::Layer(ui_width, ui_height, 1920, 1080);

    raylib::Camera3D world_camera{
        .position = {6.0f, 6.0f, 6.0f},
        .target = {0.0f, 1.0f, 0.0f},
        .up = {0.0f, 1.0f, 0.0f},
        .fovy = 45.0f,
        .projection = raylib::CAMERA_PERSPECTIVE,
    };

    raylib::SetTargetFPS(144);

    while (!raylib::WindowShouldClose()) {
    
    {
        raylib::BeginTextureMode(ui_layer.target());
        raylib::ClearBackground(raylib::BLANK);

        raylib::DrawText("UI Layer", 40, 50, 64, raylib::RAYWHITE);
        raylib::DrawText(std::format("FPS: {}", raylib::GetFPS()).c_str(), 5, 5, 32, raylib::GREEN);

        raylib::EndTextureMode();
    }

    {
        raylib::BeginTextureMode(world_layer.target());
        raylib::ClearBackground(raylib::BLACK);
        raylib::BeginMode3D(world_camera);

        raylib::DrawCube({0.0f, 1.0f, 0.0f}, 2.0f, 2.0f, 2.0f, raylib::RED);
        raylib::DrawCubeWires({0.0f, 1.0f, 0.0f}, 2.0f, 2.0f, 2.0f, raylib::MAROON);
        raylib::DrawGrid(20, 1.0f);

        raylib::EndMode3D();
        raylib::EndTextureMode();
    }

    {
        raylib::BeginDrawing();
        raylib::ClearBackground(raylib::RAYWHITE);

        const auto screen_width = raylib::GetScreenWidth();
        const auto screen_height = raylib::GetScreenHeight();

        const auto scale_x = static_cast<float>(screen_width) / virtual_width;
        const auto scale_y = static_cast<float>(screen_height) / virtual_height;
        const auto scale = std::min(scale_x, scale_y);

        const auto offset_x = (screen_width - virtual_width * scale) / 2.0f;
        const auto offset_y = (screen_height - virtual_height * scale) / 2.0f;
        const raylib::Rectangle render_rect{
            offset_x,
            offset_y,
            virtual_width * scale,
            virtual_height * scale,
        };

        world_layer.draw(render_rect);
        ui_layer.draw(ui_layer.fit_rect_top_left(render_rect));

        raylib::EndDrawing();
    }
    }

    raylib::CloseWindow();

    return 0;
}
