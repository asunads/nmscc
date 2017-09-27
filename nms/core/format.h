#pragma once

#include <nms/core/cpp.h>
#include <nms/core/trait.h>
#include <nms/core/string.h>
#include <nms/core/exception.h>


namespace nms
{

struct FormatStyle
{
    char align   = '\0';    // [<>=^]?
    char sign    = '\0';    // [+- ]?
    u8   width   = 0;       // [0-9]*
    u8   prec    = 0;       // [0-9]*

    union {
        char type   = '\0';
        char spec[12];
    };

    NMS_API static FormatStyle from_fmt_str(const str& str);
};

NMS_API void _sformat_val(IString& buf, const FormatStyle& style, i8   val);
NMS_API void _sformat_val(IString& buf, const FormatStyle& style, u8   val);
NMS_API void _sformat_val(IString& buf, const FormatStyle& style, i16  val);
NMS_API void _sformat_val(IString& buf, const FormatStyle& style, u16  val);
NMS_API void _sformat_val(IString& buf, const FormatStyle& style, i32  val);
NMS_API void _sformat_val(IString& buf, const FormatStyle& style, u32  val);
NMS_API void _sformat_val(IString& buf, const FormatStyle& style, i64  val);
NMS_API void _sformat_val(IString& buf, const FormatStyle& style, u64  val);
NMS_API void _sformat_val(IString& buf, const FormatStyle& style, f32  val);
NMS_API void _sformat_val(IString& buf, const FormatStyle& style, f64  val);

NMS_API void _sformat_val(IString& buf, const FormatStyle& style, bool val);
NMS_API void _sformat_val(IString& buf, const FormatStyle& style, str  val);

#ifdef NMS_STDCXX_TYPEINFO
NMS_API void _sformat_val(IString& buf, const FormatStyle& style, const std::type_info& val);
#endif

template<u32 N>
void _sformat_val(IString& buf, const FormatStyle& style, const char(&val)[N]) {
    _sformat_val(buf, style, str(val));
}

inline void _sformat_val(IString& buf, const FormatStyle& style, const char* val) {
    _sformat_val(buf, style, str(val, strlen(val)));
}

namespace ns_format
{

NMS_API bool next_value(IString& outbuf, str& strfmt, str& valfmt);

/*! redirect: value.sformat(outbuf, style) */
template<class T>
auto match_version(IString& outbuf, const FormatStyle& style, const T& value, Tver<4>) -> decltype(value.sformat(outbuf, style)) {
    value.sformat(outbuf, style);
    return;
}

/*! redirect: value.sformat(outbuf) */
template<class T>
auto match_version(IString& outbuf, const FormatStyle& /*style*/, const T& value, Tver<3>) -> decltype(value.sformat(outbuf)) {
    value.sformat(outbuf);
    return;
}

/*! redirect: reflect_format */
template<class T>
auto match_version(IString& outbuf, const FormatStyle& style, const T& value, Tver<2>) -> decltype(T::_$member_cnt) {
    return 0;
}

/*! redirect: _sformat_val(..., value.name) */
template<class T>
auto match_version(IString& outbuf, const FormatStyle& style, const T& value, Tver<1>) -> $when_is<$enum, T> {
    auto name = Enum<T>{ value }.name;
    _sformat_val(outbuf, style, name);
    return;
}

/*! redirect: _sformat_val(..., value) */
template<class T>
auto match_version(IString& outbuf, const FormatStyle& style, const T& value, ...) {
    _sformat_val(outbuf, style, value);
    return;
}

inline void match_index(IString& /*outbuf*/, const FormatStyle& /*style*/, i32 /*id*/) {
}

template<class T, class ...U>
void match_index(IString& outbuf, const FormatStyle& style, i32 id, const T& t, const U& ...u) {
    if (id == 0) {
        match_version(outbuf, style, t, Tver<9>{});
    }
    else {
        match_index(outbuf, style, id - 1, u...);
    }
}

}

template<class T>
void sformat(IString& outbuf, const FormatStyle& style, const T& value) {
    ns_format::match_version(outbuf, style, value, Tver<9> {});
}

template<class ...Targs>
void sformat(IString& outbuf, str strfmt, const Targs& ...args) {
    i32 argid = 0;

    str valfmt;
    while (ns_format::next_value(outbuf, strfmt, valfmt)) {
        auto style = FormatStyle::from_fmt_str(valfmt);
        ns_format::match_index(outbuf, style, argid, args...);
    }
}

template<u32 N, class ...Targs>
void sformat(IString& outbuf, const char(&fmt)[N], const Targs& ...args) {
    sformat(outbuf, str(fmt), args...);
}

inline IString& _format_buf() {
    constexpr static auto $buff_size = 64 * 1024;   // 64K
    static thread_local U8String<$buff_size> buf;
    return buf;
}

/* format to string */
template<class ...T>
str format(str fmt, const T& ...t) {
    auto& str_buf = _format_buf();
    sformat(str_buf, fmt, t...);
    return str_buf;
}


}
