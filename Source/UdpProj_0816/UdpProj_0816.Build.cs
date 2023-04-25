// Copyright Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class UdpProj_0816 : ModuleRules
{
	public UdpProj_0816(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new string[] { "Core", "CoreUObject", "Engine", "InputCore", "HeadMountedDisplay" ,
			"GameplayAbilities",
			"GameplayTags",
			"GameplayTasks"
		});

		PrivateDependencyModuleNames.Add("OnlineSubsystemNull");



    }
}
