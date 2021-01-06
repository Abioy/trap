#ifndef TRAP_INLINE_HOOK_H
#define TRAP_INLINE_HOOK_H

#include <common/singleton.h>
#include <Zydis/Zydis.h>

class CInlineHook {
#define gInlineHook SINGLETON_(CInlineHook)
public:
    CInlineHook();

private:
    unsigned long getCodeTail(void *address, unsigned long size);

public:
    void hook(void *address, void *replace, void **backup);
    void unhook(void *address, void *backup);

private:
    bool setCodeReadonly(void *address, unsigned long size) const;
    bool setCodeWriteable(void *address, unsigned long size) const;

private:
    unsigned long mPagesize;
    ZydisDecoder mDecoder{};
};


#endif //TRAP_INLINE_HOOK_H
