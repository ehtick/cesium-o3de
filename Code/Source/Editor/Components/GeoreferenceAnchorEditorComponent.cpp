#include "Editor/Components/GeoreferenceAnchorEditorComponent.h"
#include <Cesium/EBus/CoordinateTransformComponentBus.h>
#include <Cesium/EBus/LevelCoordinateTransformComponentBus.h>
#include <Cesium/Math/MathReflect.h>
#include <AzToolsFramework/API/ToolsApplicationAPI.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

namespace Cesium
{
    void GeoreferenceAnchorEditorComponent::Reflect(AZ::ReflectContext* context)
    {
        if (AZ::SerializeContext* serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<GeoreferenceAnchorEditorComponent, AZ::Component>()->Version(0)
                ->Field("ecefPicker", &GeoreferenceAnchorEditorComponent::m_ecefPicker)
                ->Field("o3dePosition", &GeoreferenceAnchorEditorComponent::m_o3dePosition)
                ;

            AZ::EditContext* editContext = serializeContext->GetEditContext();
            if (editContext)
            {
                editContext
                    ->Class<GeoreferenceAnchorEditorComponent>(
                        "Georeference Anchor", "The component is used to georeference and apply origin shifting to children entities")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                        ->Attribute(AZ::Edit::Attributes::Category, "Cesium")
                        ->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/Cesium_logo_only.svg")
                        ->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Cesium_logo_only.svg")
                        ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Game", 0x232b318c))
                        ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(AZ::Edit::UIHandlers::Default, &GeoreferenceAnchorEditorComponent::m_ecefPicker, "Anchor", "")
                    ;
            }
        }
    }

    void GeoreferenceAnchorEditorComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("GeoreferenceAnchorEditorService"));
    }

    void GeoreferenceAnchorEditorComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("GeoreferenceAnchorEditorService"));
        incompatible.push_back(AZ_CRC_CE("NonUniformScaleService"));
    }

    void GeoreferenceAnchorEditorComponent::GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
    {
        required.push_back(AZ_CRC("TransformService", 0x8ee22c50));
    }

    void GeoreferenceAnchorEditorComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    GeoreferenceAnchorEditorComponent::GeoreferenceAnchorEditorComponent()
    {
        m_positionChangeHandler = ECEFPositionChangeEvent::Handler(
            [this](glm::dvec3 ecefPosition)
            {
                AzToolsFramework::ScopedUndoBatch undoBatch("Change Anchor Position");

                AZ::EntityId levelGeoreferenceEntityId;
                LevelCoordinateTransformRequestBus::BroadcastResult(
                    levelGeoreferenceEntityId, &LevelCoordinateTransformRequestBus::Events::GetCoordinateTransform);

                glm::dmat4 ECEFToO3DE{ 1.0 };
                CoordinateTransformRequestBus::EventResult(
                    ECEFToO3DE, levelGeoreferenceEntityId, &CoordinateTransformRequestBus::Events::ECEFToO3DE);

                m_o3dePosition = ECEFToO3DE * glm::dvec4(ecefPosition, 1.0);
                m_georeferenceAnchorComponent.SetCoordinate(m_o3dePosition);
                undoBatch.MarkEntityDirty(GetEntityId());
            });
    }

    void GeoreferenceAnchorEditorComponent::BuildGameEntity(AZ::Entity* gameEntity)
    {
        auto georeferenceAnchorComponent = gameEntity->CreateComponent<GeoreferenceAnchorComponent>();
        georeferenceAnchorComponent->SetEntity(gameEntity);
        georeferenceAnchorComponent->Init();
        georeferenceAnchorComponent->Activate();
        georeferenceAnchorComponent->SetCoordinate(m_o3dePosition);
        georeferenceAnchorComponent->Deactivate();
    }

    void GeoreferenceAnchorEditorComponent::Init()
    {
    }

    void GeoreferenceAnchorEditorComponent::Activate()
    {
        m_georeferenceAnchorComponent.SetEntity(GetEntity());
        m_georeferenceAnchorComponent.Init();
        m_georeferenceAnchorComponent.Activate();
        m_georeferenceAnchorComponent.SetCoordinate(m_o3dePosition);

        m_positionChangeHandler.Connect(m_ecefPicker.m_onPositionChangeEvent);
    }

    void GeoreferenceAnchorEditorComponent::Deactivate()
    {
        m_positionChangeHandler.Disconnect();
        m_georeferenceAnchorComponent.Deactivate();
        m_georeferenceAnchorComponent.SetEntity(nullptr);
    }
}