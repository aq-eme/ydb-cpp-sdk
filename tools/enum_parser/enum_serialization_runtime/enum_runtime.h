#pragma once

#include "dispatch_methods.h"
#include "ordered_pairs.h"
#include "serialized_enum.h"

#include <span>

#include <utility>

namespace NEnumSerializationRuntime {
    /// Stores all information about enumeration except its real type
    template <typename TEnumRepresentationType>
    class TEnumDescriptionBase {
    public:
        using TRepresentationType = TEnumRepresentationType;
        using TEnumStringPair = ::NEnumSerializationRuntime::TEnumStringPair<TRepresentationType>;

        /// Refers initialization data stored in constexpr-friendly format
        struct TInitializationData {
            const std::span<const TEnumStringPair> NamesInitializer;
            const std::span<const TEnumStringPair> ValuesInitializer;
            const std::span<const std::string_view> CppNamesInitializer;
            const std::string_view CppNamesPrefix;
            const std::string_view ClassName;
        };

    public:
        TEnumDescriptionBase(const TInitializationData& enumInitData);
        ~TEnumDescriptionBase();

        const std::string& ToString(TRepresentationType key) const;

        std::string_view ToStringBuf(TRepresentationType key) const;
        static std::string_view ToStringBufFullScan(const TRepresentationType key, const TInitializationData& enumInitData);
        static std::string_view ToStringBufSorted(const TRepresentationType key, const TInitializationData& enumInitData);
        static std::string_view ToStringBufDirect(const TRepresentationType key, const TInitializationData& enumInitData);

        std::pair<bool, TRepresentationType> TryFromString(const std::string_view name) const;
        static std::pair<bool, TRepresentationType> TryFromStringFullScan(const std::string_view name, const TInitializationData& enumInitData);
        static std::pair<bool, TRepresentationType> TryFromStringSorted(const std::string_view name, const TInitializationData& enumInitData);

        TRepresentationType FromString(const std::string_view name) const;
        static TRepresentationType FromStringFullScan(const std::string_view name, const TInitializationData& enumInitData);
        static TRepresentationType FromStringSorted(const std::string_view name, const TInitializationData& enumInitData);

        void Out(IOutputStream* os, const TRepresentationType key) const;
        static void OutFullScan(IOutputStream* os, const TRepresentationType key, const TInitializationData& enumInitData);
        static void OutSorted(IOutputStream* os, const TRepresentationType key, const TInitializationData& enumInitData);
        static void OutDirect(IOutputStream* os, const TRepresentationType key, const TInitializationData& enumInitData);

        const std::string& AllEnumNames() const noexcept {
            return AllNames;
        }

        const std::vector<std::string>& AllEnumCppNames() const noexcept {
            return AllCppNames;
        }

        const std::map<TRepresentationType, std::string>& TypelessEnumNames() const noexcept {
            return Names;
        }

        const std::vector<TRepresentationType>& TypelessEnumValues() const noexcept {
            return AllValues;
        }

    private:
        std::map<TRepresentationType, std::string> Names;
        std::map<std::string_view, TRepresentationType> Values;
        std::string AllNames;
        std::vector<std::string> AllCppNames;
        std::string ClassName;
        std::vector<TRepresentationType> AllValues;
    };

    /// Wraps TEnumDescriptionBase and performs on-demand casts
    template <typename EEnum, typename TEnumRepresentationType = typename NDetail::TSelectEnumRepresentationType<EEnum>::TType>
    class TEnumDescription: public NDetail::TMappedViewBase<EEnum, TEnumRepresentationType>, private TEnumDescriptionBase<TEnumRepresentationType> {
    public:
        using TBase = TEnumDescriptionBase<TEnumRepresentationType>;
        using TCast = NDetail::TMappedViewBase<EEnum, TEnumRepresentationType>;
        using TBase::AllEnumCppNames;
        using TBase::AllEnumNames;
        using typename TBase::TEnumStringPair;
        using typename TBase::TRepresentationType;
        using typename TBase::TInitializationData;

    private:
        static bool MapFindResult(std::pair<bool, TEnumRepresentationType> findResult, EEnum& ret) {
            if (findResult.first) {
                ret = TCast::CastFromRepresentationType(findResult.second);
                return true;
            }
            return false;
        }

    public:
        using TBase::TBase;

        // ToString
        // Return reference to singleton preallocated string
        const std::string& ToString(const EEnum key) const {
            return TBase::ToString(TCast::CastToRepresentationType(key));
        }

        // ToStringBuf
        std::string_view ToStringBuf(EEnum key) const {
            return TBase::ToStringBuf(TCast::CastToRepresentationType(key));
        }
        static std::string_view ToStringBufFullScan(const EEnum key, const TInitializationData& enumInitData) {
            return TBase::ToStringBufFullScan(TCast::CastToRepresentationType(key), enumInitData);
        }
        static std::string_view ToStringBufSorted(const EEnum key, const TInitializationData& enumInitData) {
            return TBase::ToStringBufSorted(TCast::CastToRepresentationType(key), enumInitData);
        }
        static std::string_view ToStringBufDirect(const EEnum key, const TInitializationData& enumInitData) {
            return TBase::ToStringBufDirect(TCast::CastToRepresentationType(key), enumInitData);
        }

        // TryFromString-like functons
        // Return false for unknown enumeration names
        bool FromString(const std::string_view name, EEnum& ret) const {
            return MapFindResult(TBase::TryFromString(name), ret);
        }
        static bool TryFromStringFullScan(const std::string_view name, EEnum& ret, const TInitializationData& enumInitData) {
            return MapFindResult(TBase::TryFromStringFullScan(name, enumInitData), ret);
        }
        static bool TryFromStringSorted(const std::string_view name, EEnum& ret, const TInitializationData& enumInitData) {
            return MapFindResult(TBase::TryFromStringSorted(name, enumInitData), ret);
        }

        // FromString
        // Throw exception for unknown enumeration names
        EEnum FromString(const std::string_view name) const {
            return TCast::CastFromRepresentationType(TBase::FromString(name));
        }
        static EEnum FromStringFullScan(const std::string_view name, const TInitializationData& enumInitData) {
            return TCast::CastFromRepresentationType(TBase::FromStringFullScan(name, enumInitData));
        }
        static EEnum FromStringSorted(const std::string_view name, const TInitializationData& enumInitData) {
            return TCast::CastFromRepresentationType(TBase::FromStringSorted(name, enumInitData));
        }

        // Inspection
        TMappedDictView<EEnum, std::string> EnumNames() const noexcept {
            return {TBase::TypelessEnumNames()};
        }

        TMappedArrayView<EEnum> AllEnumValues() const noexcept {
            return {TBase::TypelessEnumValues()};
        }

        // Out
        void Out(IOutputStream* os, const EEnum key) const {
            TBase::Out(os, TCast::CastToRepresentationType(key));
        }
        static void OutFullScan(IOutputStream* os, const EEnum key, const TInitializationData& enumInitData) {
            TBase::OutFullScan(os, TCast::CastToRepresentationType(key), enumInitData);
        }
        static void OutSorted(IOutputStream* os, const EEnum key, const TInitializationData& enumInitData) {
            TBase::OutSorted(os, TCast::CastToRepresentationType(key), enumInitData);
        }
        static void OutDirect(IOutputStream* os, const EEnum key, const TInitializationData& enumInitData) {
            TBase::OutDirect(os, TCast::CastToRepresentationType(key), enumInitData);
        }

        static constexpr TEnumStringPair EnumStringPair(const EEnum key, const std::string_view name) noexcept {
            return {TCast::CastToRepresentationType(key), name};
        }
    };
}
