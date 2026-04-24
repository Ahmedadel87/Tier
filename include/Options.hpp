#pragma once

#include <string_view>

struct Options {
    bool dump_ast = false;
    bool dump_ir = false;
    bool dump_tokens = false;
    bool debug_status = false;
    bool debug_no_error = false;
    int optimization_level = 0;

    static Options& get() {
        static Options instance;
        return instance;
    }

    static void init(int argc, char** argv) {
        Options& o = get();
        for (int i = 1; i < argc; i++) {
            std::string_view arg = argv[i];
            if (arg == "--dump-ast") o.dump_ast = true;
            else if (arg == "--dump-ir") o.dump_ir = true;
            else if (arg == "--dump-tokens") o.dump_tokens = true;
            else if (arg == "--debug-status") o.debug_status = true;
            else if (arg == "--debug-no-errors") o.debug_no_error = true;
            else if (arg == "-O1") o.optimization_level = 1;
            else if (arg == "-O2") o.optimization_level = 2;
        }
    }

private:
    Options() = default;
    Options(const Options&) = delete;
    Options& operator=(const Options&) = delete;
};