import std;
import args;
import raylib;
import RenderLayer;

int main(int argc, const char* argv[]) {
    const auto args = args::to_span(argc, argv);

    constexpr auto virtual_width = 1280;
    constexpr auto virtual_height = 720;

    raylib::SetConfigFlags(raylib::FLAG_WINDOW_RESIZABLE); 
    raylib::InitWindow(1280, 720, "raylib modules");

    auto world_layer = RenderLayer::Layer(virtual_width, virtual_height, 1920, 1080);
    raylib::SetTextureFilter(world_layer.target().texture, raylib::TextureFilter::TEXTURE_FILTER_POINT);

    constexpr auto ui_width = 1920;
    constexpr auto ui_height = 1080;

    auto ui_layer = RenderLayer::Layer(ui_width, ui_height, 1920, 1080);

    raylib::SetTargetFPS(144);

    while (!raylib::WindowShouldClose()) {
    
    {
        ui_layer.begin();
        raylib::ClearBackground(raylib::BLANK);

        raylib::DrawText("UI Layer", 40, 50, 64, raylib::RAYWHITE);
        raylib::DrawText(std::format("FPS: {}", raylib::GetFPS()).c_str(), 5, 5, 32, raylib::GREEN);

        ui_layer.end();
    }

    {
        world_layer.begin();
        raylib::ClearBackground(raylib::BLACK);

        raylib::DrawRectangle(10, 150, 100, 100, raylib::RED);
        raylib::DrawCircle(200, 200, 50, raylib::GREEN);
        raylib::DrawLine(300, 150, 400, 250, raylib::BLUE);
        raylib::DrawTriangle({500, 150}, {450, 250}, {550, 250}, raylib::YELLOW);
        raylib::DrawEllipse(700, 200, 50, 25, raylib::PURPLE);
        raylib::DrawRing({800, 200}, 30, 50, 0, 360, 32, raylib::ORANGE);
        raylib::DrawRectangleRounded({10, 300, 100, 50}, 0.2f, 4, raylib::PINK);
        raylib::DrawRectangleRoundedLines({10, 400, 100, 50}, 0.2f, 10, raylib::MAROON);
        raylib::DrawRectangleRoundedLinesEx({10, 500, 100, 50}, 0.2f, 10, 5.0f, raylib::SKYBLUE);
        raylib::DrawCircleSector({200, 400}, 50, 0, 270, 32, raylib::LIME);

        world_layer.end();
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
