using UnrealBuildTool;

public class XRFPS : ModuleRules
{
	public XRFPS(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		// 하위 폴더 헤더를 "Input/XRInputSource.h"처럼 모듈 폴더 기준 경로로 include하기 위해 (Lyra와 같은 방식)
		PublicIncludePaths.Add(ModuleDirectory);

		PublicDependencyModuleNames.AddRange(new string[]
		{
			"Core",
			"CoreUObject",
			"Engine",
			"InputCore",
			"EnhancedInput",
			"HeadMountedDisplay"	// IXRTrackingSystem: 헤드셋이 켜져 있는지 확인
		});
	}
}
