#include <MiniFB.h>
#include <stdio.h>
#include <stdint.h>

#define WIDTH 800
#define HEIGHT 600

int main()
{
    // Initialize buffer for pixels (32-bit RGB)
    uint32_t* buffer = malloc(WIDTH * HEIGHT * sizeof(uint32_t));
    
    // Create window
    struct mfb_window* window = mfb_open("My First Window", WIDTH, HEIGHT);
    if (!window) {
        fprintf(stderr, "Failed to create window\n");
        free(buffer);
        return 1;
    }

    // Fill buffer with a simple pattern
    for (int i = 0; i < WIDTH * HEIGHT; ++i) {
        buffer[i] = MFB_RGB(i % 255, (i / 255) % 255, (i * 123) % 255);
    }

    // Main loop
    while (mfb_wait_sync(window)) {
        int state = mfb_update(window, buffer);
        if (state < 0) {
            break;  // Window closed
        }
    }

    // Cleanup
    free(buffer);
    mfb_close(window);
    return 0;
}
