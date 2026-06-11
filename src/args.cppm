export module args;

import std;

export namespace args {
    std::span<const char* const> to_span(int argc, const char* argv[]) {
        return std::span(argv, argc);
    }
}
