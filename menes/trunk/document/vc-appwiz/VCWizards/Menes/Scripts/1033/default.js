// Copyright (c) Microsoft Corporation. All rights reserved.

function OnFinish(project, selObj) {
    try {
        var strProjectPath = wizard.FindSymbol("PROJECT_PATH");
        var strProjectName = wizard.FindSymbol("PROJECT_NAME");
        wizard.AddSymbol("PCH_FILE", "standard.cpp");

        if (!CanUseDrive(strProjectPath))
            return VS_E_WIZARDBACKBUTTONPRESS;

        project = CreateProject(strProjectName, strProjectPath);
        project.Object.Keyword = "Win32Proj";
        AddCommonConfig(project, strProjectName);

        {
            var config = project.Object.Configurations("Debug");
            var LinkTool = config.Tools("VCLinkerTool");
            LinkTool.AdditionalDependencies = "eisodos.lib menes.lib $(NoInherit)";
            LinkTool.SubSystem = subSystemConsole;

            var CLTool = config.Tools("VCCLCompilerTool");
            CLTool.RuntimeLibrary = rtMultiThreadedDebugDLL;
            CLTool.PrecompiledHeaderThrough = "menes/standard.hh";
        }

        {
            var config = project.Object.Configurations("Release");
            var LinkTool = config.Tools("VCLinkerTool");
            LinkTool.AdditionalDependencies = "eisodos.lib menes.lib $(NoInherit)";
            LinkTool.SubSystem = subSystemConsole;

            var CLTool = config.Tools("VCCLCompilerTool");
            CLTool.RuntimeLibrary = rtMultiThreadedDLL;
            CLTool.PrecompiledHeaderThrough = "menes/standard.hh";
        }

        AddFilesToNewProjectWithInfFile(project, strProjectName);
        project.Object.Save();
    } catch(e) {
        if (e.description.length != 0)
            SetErrorInfo(e);
        return e.number
    }
}

function SetFileProperties(projfile, strName) {
    return false;
}

function GetTargetName(strName, strProjectName, strResPath, strHelpPath) {
    try {
        var strTarget = strName;
        return strTarget; 
    } catch(e) {
        throw e;
    }
}
