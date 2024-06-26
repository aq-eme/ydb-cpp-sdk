#include <ydb-cpp-sdk/util/system/src_location.h>

#include <ydb-cpp-sdk/util/stream/output.h>

template <>
void Out<TSourceLocation>(IOutputStream& o, const TSourceLocation& t) {
#if defined(_win_)
    std::string file(t.File);
    std::replace(file.begin(), file.vend(), '\\', '/');
    o << file;
#else
    o << t.File;
#endif
    o << ':' << t.Line;
}
