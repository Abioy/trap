#include "inline_hook.h"
#include <common/log.h>
#include <unistd.h>
#include <sys/mman.h>
#include <memory>

constexpr unsigned char TRAP_TEMPLATE[] = { 0x48, 0xB8, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0xFF, 0xE0 };
constexpr unsigned char ESCAPE_TEMPLATE[] = { 0x49, 0xBC, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x90, 0x41, 0xFF, 0xE4 };

constexpr auto GUIDE = 2;
constexpr auto TRAP_SIZE = sizeof(TRAP_TEMPLATE);
constexpr auto ESCAPE_SIZE = sizeof(ESCAPE_TEMPLATE);

CInlineHook::CInlineHook() {
    mPagesize = sysconf(_SC_PAGESIZE);
    ZydisDecoderInit(&mDecoder, ZYDIS_MACHINE_MODE_LONG_64, ZYDIS_ADDRESS_WIDTH_64);
}

unsigned long CInlineHook::getCodeTail(void *address, unsigned long size) {
    ZydisDecodedInstruction instruction = {};
    ZyanUSize length = ZYDIS_MAX_INSTRUCTION_LENGTH + size;

    unsigned long tail = 0;

    do {
        if (!ZYAN_SUCCESS(ZydisDecoderDecodeBuffer(&mDecoder, (char *)address + tail, length - tail, &instruction))) {
            LOG_ERROR("decode failed");
            return 0;
        }

        tail += instruction.length;

    } while (tail < size);

    return tail;
}

void CInlineHook::hook(void *address, void *replace, void **backup) {
    unsigned long tail = getCodeTail(address, TRAP_SIZE);

    if (tail == 0) {
        LOG_ERROR("get code tail failed");
        return;
    }

    std::unique_ptr<char> escape(new char[tail + ESCAPE_SIZE]());

    if (!setCodeWriteable(escape.get()))
        return;

    memcpy(escape.get(), address, tail);
    memcpy(escape.get() + tail, ESCAPE_TEMPLATE, ESCAPE_SIZE);

    *(void **)(escape.get() + tail + GUIDE) = (char *)address + tail;

    if (!setCodeWriteable(address))
        return;

    memcpy(address, TRAP_TEMPLATE, TRAP_SIZE);
    *(void **)((char *)address + GUIDE) = replace;

    *backup = escape.release();
}

void CInlineHook::unhook(void *address, void *backup) {
    if (!setCodeWriteable(address))
        return;

    memcpy(address, backup, TRAP_SIZE);

    delete [](char*)backup;
}

bool CInlineHook::setCodeReadonly(void *address) {
    unsigned long start = ((unsigned long)address &~ ((mPagesize) - 1));

    if (mprotect((void *)start, mPagesize, PROT_READ | PROT_EXEC) < 0) {
        LOG_ERROR("set code page readonly attr failed");
        return false;
    }

    return true;
}

bool CInlineHook::setCodeWriteable(void *address) {
    unsigned long start = ((unsigned long)address &~ ((mPagesize) - 1));

    if (mprotect((void *)start, mPagesize, PROT_READ | PROT_WRITE | PROT_EXEC) < 0) {
        LOG_ERROR("set code page writeable attr failed");
        return false;
    }

    return true;
}
