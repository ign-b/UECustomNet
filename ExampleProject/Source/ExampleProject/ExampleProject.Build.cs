using System;
using System.IO;
using UnrealBuildTool;

public class ExampleProject : ModuleRules
{
	public ExampleProject(ReadOnlyTargetRules Target) : base(Target)
	{
		PCHUsage = PCHUsageMode.UseExplicitOrSharedPCHs;

		PublicDependencyModuleNames.AddRange(new[] { "Core", "CoreUObject", "Engine", "InputCore", "EnhancedInput" });

		PrivateDependencyModuleNames.AddRange(new[] { "AIModule" });

		{
			string UECustomNetPath = Path.Combine(ModuleDirectory, "../../ThirdParty", "UECustomNet");
			string UECustomNetArch = "AMD64";
			string UECustomNetConfig = "Release";

			PublicIncludePaths.Add(Path.Combine(UECustomNetPath, "Include"));
			if (Target.Platform == UnrealTargetPlatform.Win64)
			{
				PublicAdditionalLibraries.Add(Path.Combine(UECustomNetPath, "Lib",
					"Core_" + UECustomNetArch + "_" + UECustomNetConfig + ".lib"));
				PublicAdditionalLibraries.Add(Path.Combine(UECustomNetPath, "Lib",
					"slikenet_" + UECustomNetArch + "_" + UECustomNetConfig + ".lib"));
			}
		}
	}
}