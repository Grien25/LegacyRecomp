#pragma once
#include <stdafx.h>
#include <cpu/ppc_context.h>
#include <cpu/guest_thread.h>
#include <apu/audio.h>
#include "function.h"
#include "xex.h"
#include "xbox.h"
#include "heap.h"
#include "memory.h"
#include <memory>
#include "xam.h"
#include "xdm.h"
#include <os/logger.h>
#include <algorithm>


void swap_u16_str(uint16_t* str)
{
    if (!str) return;

    while (*str)
    {
        uint16_t ch = *str;
        // Swap high and low byte
        *str = (uint16_t)((ch << 8) | (ch >> 8));
        str++;
    }
}

size_t u16_strlen(uint16_t* str) {
    size_t len = 0;
    if (!str) return 0;

    while (str[len] != 0) {
        len++;
    }
    return len;
}




// return a swapped copy of the u16 string at ptr
std::u16string load_and_swap_u16Str(uint16_t* ptr)
{
    std::u16string value;
    for (int i = 0;; ++i) {
        be<uint16_t> c = *((uint16_t*)ptr + i);
        if (!c) {
            break;
        }
        value.push_back(c.value);
    }
    return value;
}

inline std::string u16_to_utf8(std::u16string str) {
    return std::string(str.begin(), str.end());
}
inline void println_u16str(std::u16string str) {
    fmt::println("{}", u16_to_utf8(str).c_str());
}
inline void print_u16str(std::u16string str) {
    fmt::print("{}", u16_to_utf8(str).c_str());
}






//
// everything under here is copied from xenia:
// https://github.com/xenia-canary/xenia-canary/blob/765073021a3fd0ec193f89ff224aad5a3af31551/src/xenia/kernel/xboxkrnl/xboxkrnl_strings.cc
//
//


template <typename T, size_t N>
constexpr size_t countof(T(&arr)[N]) {
    return std::extent<T[N]>::value;
}

enum FormatState {
    FS_Invalid = 0,
    FS_Unknown,
    FS_Start,
    FS_Flags,
    FS_Width,
    FS_PrecisionStart,
    FS_Precision,
    FS_Size,
    FS_Type,
    FS_End,
};

enum FormatFlags {
    FF_LeftJustify = 1 << 0,
    FF_AddLeadingZeros = 1 << 1,
    FF_AddPositive = 1 << 2,
    FF_AddPositiveAsSpace = 1 << 3,
    FF_AddNegative = 1 << 4,
    FF_AddPrefix = 1 << 5,
    FF_IsShort = 1 << 6,
    FF_IsLong = 1 << 7,
    FF_IsLongLong = 1 << 8,
    FF_IsWide = 1 << 9,
    FF_IsSigned = 1 << 10,
    FF_ForceLeadingZero = 1 << 11,
    FF_InvertWide = 1 << 12,
};

enum ArgumentSize {
    AS_Default = 0,
    AS_Short,
    AS_Long,
    AS_LongLong,
};


inline uint32_t get_arg_stack_ptr(PPCContext* ppc_context, uint8_t index) {
    return ((uint32_t)ppc_context->r1.u32) + 0x54 + index * 8;
}

inline uint8_t get_arg_8(PPCContext* ppc_context, uint8_t index) {
    if (index <= 7) {
        switch (index) {
        case 0:
            return ppc_context->r3.u8;
        case 1:
            return ppc_context->r4.u8;
        case 2:
            return ppc_context->r5.u8;
        case 3:
            return ppc_context->r6.u8;
        case 4:
            return ppc_context->r7.u8;
        case 5:
            return ppc_context->r8.u8;
        case 6:
            return ppc_context->r9.u8;
        case 7:
            return ppc_context->r10.u8;
        }
    }
    uint32_t stack_address = get_arg_stack_ptr(ppc_context, index - 8);
    return (uint8_t)g_memory.Translate(stack_address);
}

inline uint16_t get_arg_16(PPCContext* ppc_context, uint8_t index) 
{
    if (index <= 7) 
    {
        switch (index) 
        {
        case 0:
            return ppc_context->r3.u16;
        case 1:
            return ppc_context->r4.u16;
        case 2:
            return ppc_context->r5.u16;
        case 3:
            return ppc_context->r6.u16;
        case 4:
            return ppc_context->r7.u16;
        case 5:
            return ppc_context->r8.u16;
        case 6:
            return ppc_context->r9.u16;
        case 7:
            return ppc_context->r10.u16;
        }
    }
    uint32_t stack_address = get_arg_stack_ptr(ppc_context, index - 8);
    return (uint16_t)g_memory.Translate(stack_address);
}

inline uint32_t get_arg_32(PPCContext* ppc_context, uint8_t index) {
    if (index <= 7) {
        switch (index) {
        case 0:
            return ppc_context->r3.u32;
        case 1:
            return ppc_context->r4.u32;
        case 2:
            return ppc_context->r5.u32;
        case 3:
            return ppc_context->r6.u32;
        case 4:
            return ppc_context->r7.u32;
        case 5:
            return ppc_context->r8.u32;
        case 6:
            return ppc_context->r9.u32;
        case 7:
            return ppc_context->r10.u32;
        }
    }
    uint32_t stack_address = get_arg_stack_ptr(ppc_context, index - 8);
    return (uint32_t)g_memory.Translate(stack_address);
}

inline uint64_t get_arg_64(PPCContext* ppc_context, uint8_t index) {
    if (index <= 7) {
        switch (index) {
        case 0:
            return ppc_context->r3.u64;
        case 1:
            return ppc_context->r4.u64;
        case 2:
            return ppc_context->r5.u64;
        case 3:
            return ppc_context->r6.u64;
        case 4:
            return ppc_context->r7.u64;
        case 5:
            return ppc_context->r8.u64;
        case 6:
            return ppc_context->r9.u64;
        case 7:
            return ppc_context->r10.u64;
        }
    }
    uint32_t stack_address = get_arg_stack_ptr(ppc_context, index - 8);
    return (uint64_t)g_memory.Translate(stack_address);
}


#define SHIM_GET_ARG_8(n) get_arg_8(&ctx, n)
#define SHIM_GET_ARG_16(n) get_arg_16(&ctx, n)
#define SHIM_GET_ARG_32(n) get_arg_32(&ctx, n)
#define SHIM_GET_ARG_64(n) get_arg_64(&ctx, n)


class FormatData {
public:
    virtual uint16_t get() = 0;
    virtual uint16_t peek(int32_t offset) = 0;
    virtual void skip(int32_t count) = 0;
    virtual bool put(uint16_t c) = 0;
};

class ArgList {
public:
    virtual uint32_t get32() = 0;
    virtual uint64_t get64() = 0;
};

class StackArgList : public ArgList {
public:
    StackArgList(PPCContext ctx, int32_t index)
        : ctx(ctx), index_(index) {
    }

    uint32_t get32() { return (uint32_t)get64(); }

    uint64_t get64() {
        auto value = SHIM_GET_ARG_64(index_);
        ++index_;
        return value;
    }

private:
    PPCContext ctx;
    int32_t index_;
};

class ArrayArgList : public ArgList {
public:
    ArrayArgList(PPCContext* ppc_context, uint32_t arg_ptr)
        : ppc_context(ppc_context), arg_ptr_(arg_ptr), index_(0) {
    }

    uint32_t get32() { return (uint32_t)get64(); }

    uint64_t get64() {
        auto value = *(uint64_t*)(g_memory.Translate(arg_ptr_ + (8 * index_)));
        ++index_;
        return be<uint64_t>(value).value;
    }

private:
    PPCContext* ppc_context;
    uint32_t arg_ptr_;
    int32_t index_;
};

class StringFormatData : public FormatData {
public:
    StringFormatData(const uint8_t* input) : input_(input) {}

    uint16_t get() {
        uint16_t result = *input_;
        if (result) {
            input_++;
        }
        return result;
    }

    uint16_t peek(int32_t offset) { return input_[offset]; }

    void skip(int32_t count) {
        while (count-- > 0) {
            if (!get()) {
                break;
            }
        }
    }

    bool put(uint16_t c) {
        if (c >= 0x100) {
            return false;
        }
        output_.push_back(char(c));
        return true;
    }

    const std::string& str() const { return output_; }

private:
    const uint8_t* input_;
    std::string output_;
};

class WideStringFormatData : public FormatData {
public:
    WideStringFormatData(const uint16_t* input) : input_(input) {}

    uint16_t get() {
        be<uint16_t> result = *input_;
        if (result) {
            input_++;
        }
        return result.value;
    }

    uint16_t peek(int32_t offset) { return be<uint16_t>(input_[offset]).value; }

    void skip(int32_t count) {
        while (count-- > 0) {
            if (!get()) {
                break;
            }
        }
    }

    bool put(uint16_t c) {
        output_.push_back(char16_t(c));
        return true;
    }

    const std::u16string& wstr() const { return output_; }

private:
    const uint16_t* input_;
    std::u16string output_;
};

class WideCountFormatData : public FormatData {
public:
    WideCountFormatData(const uint16_t* input) : input_(input), count_(0) {}

    uint16_t get() {
        be<uint16_t> result = *input_;
        if (result) {
            input_++;
        }
        return result.value;
    }

    uint16_t peek(int32_t offset) { return  be<uint16_t>(input_[offset]).value; }

    void skip(int32_t count) {
        while (count-- > 0) {
            if (!get()) {
                break;
            }
        }
    }

    bool put(uint16_t c) {
        ++count_;
        return true;
    }

    const int32_t count() const { return count_; }

private:
    const uint16_t* input_;
    int32_t count_;
};
