#pragma once

#include <CesiumAsync/AsyncSystem.h>
#include <CesiumAsync/Future.h>
#include <AzCore/std/containers/vector.h>
#include <AzCore/std/string/string.h>
#include <cstddef>

namespace Cesium
{
    struct IORequestParameter
    {
        AZStd::string m_parentPath;
        AZStd::string m_path;
    };

    using IOContent = AZStd::vector<std::byte>; 

    class GenericIOManager
    {
    public:
        virtual ~GenericIOManager() = default;

        virtual AZStd::string GetParentPath(const AZStd::string& path) = 0;

        virtual IOContent GetFileContent(const IORequestParameter& request) = 0;

        virtual IOContent GetFileContent(IORequestParameter&& request) = 0;

        virtual CesiumAsync::Future<IOContent> GetFileContentAsync(
            const CesiumAsync::AsyncSystem& asyncSystem, const IORequestParameter& request) = 0;

        virtual CesiumAsync::Future<IOContent> GetFileContentAsync(
            const CesiumAsync::AsyncSystem& asyncSystem, IORequestParameter&& request) = 0;
    };
}
