#include "CesiumLevelSettingsEditorComponent.h"
#include <Cesium/OriginShiftAwareComponentBus.h>
#include <AzCore/Serialization/SerializeContext.h>
#include <AzCore/Serialization/EditContext.h>

namespace Cesium
{
    void CesiumLevelSettingsEditorComponent::Reflect(AZ::ReflectContext* context)
    {
        if (auto serializeContext = azrtti_cast<AZ::SerializeContext*>(context))
        {
            serializeContext->Class<CesiumLevelSettingsEditorComponent, AZ::Component>()
                ->Version(0)
                ->Field("defaultCoordinateTransformEntityId", &CesiumLevelSettingsEditorComponent::m_defaultCoordinateTransformEntityId)
                ;

            auto editContext = serializeContext->GetEditContext();
            if (editContext)
            {
                editContext->Class<CesiumLevelSettingsEditorComponent>("Level Settings", "")
                    ->ClassElement(AZ::Edit::ClassElements::EditorData, "")
                    ->Attribute(AZ::Edit::Attributes::Category, "Cesium")
                    ->Attribute(AZ::Edit::Attributes::Icon, "Editor/Icons/Components/Cesium_logo_only.svg")
                    ->Attribute(AZ::Edit::Attributes::ViewportIcon, "Editor/Icons/Components/Cesium_logo_only.svg")
                    ->Attribute(AZ::Edit::Attributes::AppearsInAddComponentMenu, AZ_CRC("Level", 0x9aeacc13))
                    ->Attribute(AZ::Edit::Attributes::AutoExpand, true)
                    ->DataElement(
                        AZ::Edit::UIHandlers::Default, &CesiumLevelSettingsEditorComponent::m_defaultCoordinateTransformEntityId,
                        "Default Coordinate Transform Entity", "")
                    ->Attribute(
                        AZ::Edit::Attributes::ChangeNotify, &CesiumLevelSettingsEditorComponent::OnDefaultCoordinateTransformEntityChanged);
            }
        }
    }

    void CesiumLevelSettingsEditorComponent::GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
    {
        provided.push_back(AZ_CRC_CE("CesiumLevelSettingsService"));
    }

    void CesiumLevelSettingsEditorComponent::GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
    {
        incompatible.push_back(AZ_CRC_CE("CesiumLevelSettingsService"));
    }

    void CesiumLevelSettingsEditorComponent::GetRequiredServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& required)
    {
    }

    void CesiumLevelSettingsEditorComponent::GetDependentServices([[maybe_unused]] AZ::ComponentDescriptor::DependencyArrayType& dependent)
    {
    }

    void CesiumLevelSettingsEditorComponent::Init()
    {
        AzToolsFramework::Components::EditorComponentBase::Init();
    }

    void CesiumLevelSettingsEditorComponent::Activate()
    {
        m_levelComponent.SetEntity(GetEntity());
        m_levelComponent.Init();
        m_levelComponent.Activate();
        m_levelComponent.SetCoordinateTransform(m_defaultCoordinateTransformEntityId);
    }

    void CesiumLevelSettingsEditorComponent::Deactivate()
    {
        m_levelComponent.Deactivate();
        m_levelComponent.SetEntity(nullptr);
    }

    void CesiumLevelSettingsEditorComponent::BuildGameEntity(AZ::Entity* gameEntity)
    {
        auto component = gameEntity->CreateComponent<CesiumLevelSettingsComponent>();
        component->SetEntity(gameEntity);
        component->Init();
        component->SetCoordinateTransform(m_defaultCoordinateTransformEntityId);
    }

    void CesiumLevelSettingsEditorComponent::OnDefaultCoordinateTransformEntityChanged()
    {
        m_levelComponent.SetCoordinateTransform(m_defaultCoordinateTransformEntityId);
    }
} // namespace Cesium
