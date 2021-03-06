#pragma once

#include <nms/core/string.h>

namespace nms::thread
{
class Mutex;
}

namespace nms::io::console
{

namespace
{

/* --- table --- */
//   ┌─┬─┐   lqwqk
//  │├─┼─┤ x tqnqu
//   └─┴─┘   mqvqj
/* ------------- */

/* --- cursor --- */
//      A
// H D ─┼─ C F
//      B
/* -------------- */

static const char $cursor_save[] = "\033[s";
static const char $cursor_load[] = "\033[u";

static const char $clr[] = "\033[2J";

static const char $rst[] = "\033[0m";       // reset

static const char $font_I[] = "\033[4m";    // +underline
static const char $font_i[] = "\033[24m";   // -underline

static const char $bg_fg[] = "\033[7m";     // bg/fg
static const char $fg_bg[] = "\033[27m";    // fg/bg

static const char $fg_blk[] = "\033[30m";
static const char $fg_red[] = "\033[31m";
static const char $fg_grn[] = "\033[32m";
static const char $fg_yel[] = "\033[33m";
static const char $fg_blu[] = "\033[34m";
static const char $fg_mag[] = "\033[35m";
static const char $fg_cyn[] = "\033[36m";
static const char $fg_wht[] = "\033[37m";

static const char $bg_blk[] = "\033[40m";
static const char $bg_red[] = "\033[41m";
static const char $bg_grn[] = "\033[42m";
static const char $bg_yel[] = "\033[43m";
static const char $bg_blu[] = "\033[44m";
static const char $bg_mag[] = "\033[45m";
static const char $bg_cyn[] = "\033[46m";
static const char $bg_wht[] = "\033[47m";
}



NMS_API void writes(const str texts[], u32 n);

inline void  write(const str& text) {
    writes(&text, 1);
}

inline void writeln() {
    static str texts[] = { "\n" };
    writes(texts, 1);
}

inline void  writeln(const str& text) {
    str texts[] = { text, "\n" };
    writes(texts, 2);
}

/*! console string buffer */
inline IString& _tls_buf_for_write() {
    static thread_local U8String<4 * 1024 * 1024> buf;   // 4 MB
    return buf;
}

template<class T, class ...U>
void write(const str& fmt, const T& t, const U& ...u) {
    auto& buf = _tls_buf_for_write();
    buf._resize(0);
    sformat(buf, fmt, t, u...);
    write(buf);
}

template<class T, class ...U>
void writeln(const str& fmt, const T& t, const U& ...u) {
    auto& buf = _tls_buf_for_write();
    buf._resize(0u);
    sformat(buf, fmt, t, u...);
    buf += '\n';
    write(buf);
}

NMS_API u32  columns();
NMS_API void show_cursor(bool value = true);
NMS_API void hide_cursor(bool value = true);
NMS_API void goto_line(i32 line);

NMS_API void progress_bar(f64 percent, const View<const str>& digits, const View<const str>& circles, const str& color);
NMS_API void progress_bar(f64 percent, const str& color={});


}
