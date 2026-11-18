#ifndef R4_HUD_PACKETS_H
#define R4_HUD_PACKETS_H
#include <stdint.h>

/* Remove a frame-local producer's packets without skipping its timer/audio
 * logic. Validate the complete DMA chain before changing any links. */
static inline int r4_hud_unlink_range(uint32_t root, uint32_t first, uint32_t end,
                                    uint32_t (*read32)(uint32_t),
                                    void (*write32)(uint32_t, uint32_t)) {
    if (first >= end || (first & 3u) || (end & 3u)) return 0;
    uint32_t node = root;
    unsigned count = 0;
    for (;;) {
        if ((node & 3u) || (node & 0x1FFFFFFFu) >= 0x00800000u || ++count > 32768u)
            return -1;
        const uint32_t next = read32(node) & 0x00FFFFFFu;
        if (next == 0x00FFFFFFu) break;
        node = 0x80000000u | next;
    }
    int removed = 0;
    node = root;
    for (unsigned i = 0; i < count; ++i) {
        const uint32_t tag = read32(node);
        const uint32_t next = tag & 0x00FFFFFFu;
        if (next == 0x00FFFFFFu) break;
        const uint32_t address = 0x80000000u | next;
        if (address >= first && address < end) {
            write32(node, (tag & 0xFF000000u) | (read32(address) & 0x00FFFFFFu));
            ++removed;
        } else node = address;
    }
    return removed;
}
#endif
