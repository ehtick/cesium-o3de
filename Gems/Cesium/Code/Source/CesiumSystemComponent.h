#pragma once

#include "CesiumSystemComponentBus.h"
#include "CriticalAssetManager.h"
#include <spdlog/logger.h>
#include <CesiumAsync/ITaskProcessor.h>
#include <CesiumAsync/IAssetAccessor.h>
#include <AzCore/Jobs/JobManager.h>
#include <AzCore/Jobs/JobContext.h>
#include <AzCore/Component/Component.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>
#include <memory>

namespace Cesium
{
    class LocalFileManager;
    class HttpManager;
    class SingleThreadScheduler;

    class CesiumSystemComponent
        : public AZ::Component
        , protected CesiumRequestBus::Handler
        , public AZ::TickBus::Handler
    {
    public:
        AZ_COMPONENT(CesiumSystemComponent, "{9c897f42-76da-4cf9-8e84-18ce3f152f94}");

        static void Reflect(AZ::ReflectContext* context);

        static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided);

        static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible);

        static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required);

        static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& dependent);

        CesiumSystemComponent();

        ~CesiumSystemComponent();

        GenericIOManager& GetIOManager(IOKind kind) override;

        const std::shared_ptr<CesiumAsync::IAssetAccessor>& GetAssetAccessor(IOKind kind) const override;

        const std::shared_ptr<CesiumAsync::ITaskProcessor>& GetTaskProcessor() const override;

        const std::shared_ptr<spdlog::logger>& GetLogger() const override;

        const CriticalAssetManager& GetCriticalAssetManager() const override;

    protected:
        void Init() override;

        void Activate() override;

        void Deactivate() override;

        void OnTick(float deltaTime, AZ::ScriptTimePoint time) override;

    private:
        AZStd::unique_ptr<HttpManager> m_httpManager;
        AZStd::unique_ptr<LocalFileManager> m_localFileManager;
        std::shared_ptr<CesiumAsync::IAssetAccessor> m_httpAssetAccessor;
        std::shared_ptr<CesiumAsync::IAssetAccessor> m_localFileAssetAccessor;
        std::shared_ptr<CesiumAsync::ITaskProcessor> m_taskProcessor;
        std::shared_ptr<spdlog::logger> m_logger;
        CriticalAssetManager m_criticalAssetManager;
    };

} // namespace Cesium
