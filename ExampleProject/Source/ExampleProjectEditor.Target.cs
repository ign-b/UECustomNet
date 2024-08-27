using UnrealBuildTool;

public class ExampleProjectEditorTarget : TargetRules
{
	public ExampleProjectEditorTarget(TargetInfo Target) : base(Target)
	{
		Type = TargetType.Editor;
		DefaultBuildSettings = BuildSettingsVersion.Latest;
		IncludeOrderVersion = EngineIncludeOrderVersion.Latest;
		
		ExtraModuleNames.Add("ExampleProject");
	}
}