#pragma once

#include <AzCore/Component/ComponentBus.h>
#include <AzCore/std/string/string.h>

namespace Cesium
{
    class CesiumTilesetRequest : public AZ::ComponentBus
    {
    public:
        virtual void AddCameraEntity(const AZ::EntityId& cameraEntityId) = 0;

        virtual void RemoveCameraEntity(const AZ::EntityId& cameraEntityId) = 0;

        virtual void LoadTileset(const AZStd::string& filePath) = 0;
    };

    using CesiumTilesetRequestBus = AZ::EBus<CesiumTilesetRequest>;
} // namespace Cesium
