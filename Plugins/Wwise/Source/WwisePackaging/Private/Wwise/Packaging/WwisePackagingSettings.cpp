/*******************************************************************************
The content of this file includes portions of the proprietary AUDIOKINETIC Wwise
Technology released in source code form as part of the game integration package.
The content of this file may not be used without valid licenses to the
AUDIOKINETIC Wwise Technology.
Note that the use of the game engine is subject to the Unreal(R) Engine End User
License Agreement at https://www.unrealengine.com/en-US/eula/unreal
 
License Usage
 
Licensees holding valid licenses to the AUDIOKINETIC Wwise Technology may use
this file in accordance with the end user license agreement provided with the
software or, alternatively, in accordance with the terms contained
in a written agreement between you and Audiokinetic Inc.
Copyright (c) 2026 Audiokinetic Inc.
*******************************************************************************/

#include "Wwise/Packaging/WwisePackagingSettings.h"

#include "AssetRegistry/AssetRegistryModule.h"
#include "Wwise/Packaging/WwisePackagingUtils.h"


#if WITH_EDITOR
#include "Engine/Engine.h"
#include "Misc/ConfigCacheIni.h"
#include "AssetToolsModule.h"
#include "FileHelpers.h"
#include "ISourceControlModule.h"
#include "ModuleDescriptor.h"
#include "SSettingsEditorCheckoutNotice.h"
#include "Interfaces/IPluginManager.h"
#include "Wwise/Packaging/WwiseAssetLibrary.h"
#include "Wwise/Packaging/WwiseAssetLibraryFilter.h"
#include "Wwise/Stats/Packaging.h"
#endif

#define LOCTEXT_NAMESPACE "WwisePackaging"

#if WITH_EDITOR

void UWwisePackagingSettings::ProcessGameplayModulesUsingDefaultGroup()
{
	GameplayModuleUsingDefaultGroup.Empty();
	GetGameplayModuleUsingDefaultGroup(GameplayModuleUsingDefaultGroup);
}

void UWwisePackagingSettings::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	OnSettingsChanged.Broadcast(this);
	Super::PostEditChangeProperty(PropertyChangedEvent);
}

void UWwisePackagingSettings::PostInitProperties()
{
	Super::PostInitProperties();
}

bool UWwisePackagingSettings::EnsurePostEngineInit()
{
	if (const ELoadingPhase::Type CurrentPhase{ IPluginManager::Get().GetLastCompletedLoadingPhase() };
		CurrentPhase == ELoadingPhase::None || CurrentPhase < ELoadingPhase::PostDefault || !GEngine)
	{
		if (!PostEngineInitDelegate.IsValid())
		{
			PostEngineInitDelegate = FCoreDelegates::OnPostEngineInit.AddUObject(this, &UWwisePackagingSettings::OnPostEngineInit);
		}
		return false;
	}

	return true;
}

bool UWwisePackagingSettings::SaveConfigFile()
{
	const FString ConfigFilename = GetDefaultConfigFilename();
	if(ISourceControlModule::Get().IsEnabled())
	{
		if (!SettingsHelpers::IsCheckedOut(ConfigFilename, true))
		{
			if (!SettingsHelpers::CheckOutOrAddFile(ConfigFilename, true))
			{
				return false;
			}
		}
	}

	return TryUpdateDefaultConfigFile();
}

void UWwisePackagingSettings::GetGameplayModuleUsingDefaultGroup(TArray<FString>& OutGameplayModuleUsedInDefaultGroup)
{
	if (DefaultAssetLibraryGroup.GetLongPackageFName().IsNone())
	{
		return;	
	}
	auto AssetRegistryModule = &FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
	TArray<FAssetData> LibraryGroupAssets;
	FARFilter GroupFilter;
	GroupFilter.bRecursivePaths = true;
	GroupFilter.ClassPaths.Add(UWwiseAssetLibraryGroup::StaticClass()->GetClassPathName());
	AssetRegistryModule->Get().GetAssets(GroupFilter, LibraryGroupAssets);

	TSet<FString> ModulesNotUsingDefaultGroup;
	const auto DefaultRoot = WwisePackagingUtils::GetGameplayModuleRoot(DefaultAssetLibraryGroup.GetLongPackageName());
	OutGameplayModuleUsedInDefaultGroup.Add(DefaultRoot.RightChop(1));
	for (auto& LibraryGroupPath : LibraryGroupAssets)
	{
		auto GroupRoot = WwisePackagingUtils::GetGameplayModuleRoot(LibraryGroupPath.GetObjectPathString());
		if (DefaultRoot != GroupRoot)
		{
			ModulesNotUsingDefaultGroup.Add(GroupRoot);
		}
	}
	
	TArray<FAssetData> WwiseAssets;
	FARFilter Filter;
	Filter.bRecursivePaths = true;
	Filter.TagsAndValues.Add("WwiseGuid");
	AssetRegistryModule->Get().GetAssets(Filter, WwiseAssets);

	for (auto& WwiseAsset : WwiseAssets)
	{
		auto Root = WwisePackagingUtils::GetGameplayModuleRoot(WwiseAsset.GetObjectPathString());
		if (ModulesNotUsingDefaultGroup.Contains(Root))
		{
			continue;
		}
		OutGameplayModuleUsedInDefaultGroup.AddUnique(Root.RightChop(1));
	}
}
#endif

#undef LOCTEXT_NAMESPACE
