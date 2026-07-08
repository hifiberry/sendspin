#include <cstdio>
#include <cstring>

int main(int argc, char** argv) {
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--version") == 0) {
            std::printf("sendspind 0.1.0\n");
            return 0;
        }
    }
    std::fprintf(stderr, "sendspind: nothing to do (try --version)\n");
    return 0;
}
