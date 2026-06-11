import std;
import args;
import raylib;

int main(int argc, const char* argv[]) {
    const auto args = args::to_span(argc, argv);

    constexpr auto window_width = 960;
    constexpr auto window_height = 540;

    raylib::InitWindow(window_width, window_height, "raylib modules");
    raylib::SetTargetFPS(144);

    while (!raylib::WindowShouldClose()) {
        raylib::BeginDrawing();
        raylib::ClearBackground(raylib::BLACK);

        raylib::DrawText("Hello raylib", 10, 50, 22, raylib::RAYWHITE);
        raylib::DrawText(std::format("argc = {}", args.size()).c_str(), 10, 80, 22, raylib::RAYWHITE);
        raylib::DrawText(std::format("argv[0] = {}", args[0]).c_str(), 10, 110, 22, raylib::RAYWHITE);

        raylib::DrawRectangle(10, 150, 100, 100, raylib::RED);
        raylib::DrawCircle(200, 200, 50, raylib::GREEN);
        raylib::DrawLine(300, 150, 400, 250, raylib::BLUE);

        raylib::DrawFPS(10, 10);
        raylib::EndDrawing();
    }

    raylib::CloseWindow();

    return 0;
}
