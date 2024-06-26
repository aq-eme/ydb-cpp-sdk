#pragma once

#include <ydb-cpp-sdk/util/system/defaults.h>

#include <ydb-cpp-sdk/util/generic/ptr.h>

#define Y_GET_FUNC(dll, name) FUNC_##name((dll).Sym(#name))
#define Y_GET_FUNC_OPTIONAL(dll, name) FUNC_##name((dll).SymOptional(#name))

#ifdef _win32_
    #define DEFAULT_DLLOPEN_FLAGS 0
#else
    #include <dlfcn.h>

    #ifndef RTLD_GLOBAL
        #define RTLD_GLOBAL (0)
    #endif

    #define DEFAULT_DLLOPEN_FLAGS (RTLD_NOW | RTLD_GLOBAL)
#endif

class TDynamicLibrary {
public:
    TDynamicLibrary() noexcept;
    TDynamicLibrary(const std::string& path, int flags = DEFAULT_DLLOPEN_FLAGS);
    ~TDynamicLibrary();

    void Open(const char* path, int flags = DEFAULT_DLLOPEN_FLAGS);
    void Close() noexcept;
    void* SymOptional(const char* name) noexcept;
    void* Sym(const char* name);
    bool IsLoaded() const noexcept;
    void SetUnloadable(bool unloadable); // Set to false to avoid unloading on destructor

private:
    class TImpl;
    THolder<TImpl> Impl_;
};

// a wrapper for a symbol
template <class TLib>
class TExternalSymbol {
private:
    TLib* PLib;
    TDynamicLibrary* DLib;
    std::string lname;
    std::string vname;

public:
    TExternalSymbol() noexcept {
        PLib = nullptr;
        DLib = nullptr;
    }
    TExternalSymbol(const TExternalSymbol& es) {
        PLib = nullptr;
        DLib = nullptr;
        if (es.IsDynamic())
            Open(es.LibName().data(), es.VtblName().data());
        else if (es.IsStatic())
            SetSym(es.Symbol());
    }
    TExternalSymbol& operator=(const TExternalSymbol& es) {
        if (this != &es) {
            Close();
            if (es.IsDynamic())
                Open(es.LibName().data(), es.VtblName().data());
            else if (es.IsStatic())
                SetSym(es.Symbol());
        }
        return *this;
    }
    ~TExternalSymbol() {
        delete DLib;
    }
    // set the symbol from dynamic source
    void Open(const char* lib_name, const char* vtbl_name) {
        if (DLib != nullptr || PLib != nullptr)
            return;
        try {
            DLib = new TDynamicLibrary();
            DLib->Open(lib_name);
            PLib = (TLib*)DLib->Sym(vtbl_name);
        } catch (...) {
            delete DLib;
            DLib = nullptr;
            throw;
        }
        lname = lib_name;
        vname = vtbl_name;
    }
    // set the symbol from static source
    void SetSym(TLib* pl) noexcept {
        if (DLib == nullptr && PLib == nullptr)
            PLib = pl;
    }
    void Close() noexcept {
        delete DLib;
        DLib = 0;
        PLib = 0;
        lname.erase();
        vname.erase();
    }
    TLib* Symbol() const noexcept {
        return PLib;
    }
    const std::string& LibName() const noexcept {
        return lname;
    }
    const std::string& VtblName() const noexcept {
        return vname;
    }
    bool IsStatic() const noexcept {
        return DLib == nullptr && PLib != nullptr;
    }
    bool IsDynamic() const noexcept {
        return DLib && DLib->IsLoaded() && PLib != nullptr;
    }
};
