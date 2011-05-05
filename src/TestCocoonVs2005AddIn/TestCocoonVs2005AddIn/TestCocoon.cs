using System;
using System.Diagnostics;
using System.Text;
using System.Reflection;
using System.Collections.Generic;
using System.Collections;
using Microsoft.VisualStudio.VCProjectEngine;
using EnvDTE;
using EnvDTE80;
using System.Windows.Forms;

namespace TestCocoonVs2005AddIn
{
  class TestCocoon
  {
    private Connect VsConnect;
    List<string> CoverageScannerKeywordList = null;

    public TestCocoon(Connect c)
    {
      VsConnect = c;

      CoverageScannerKeywordList = new List<string>();
      CoverageScannerKeywordList.Add("--cs-branch");
      CoverageScannerKeywordList.Add("--cs-decision");
      CoverageScannerKeywordList.Add("--cs-condition");
      CoverageScannerKeywordList.Add("--cs-full-instrumentation");
      CoverageScannerKeywordList.Add("--cs-partial-instrumentation");
      CoverageScannerKeywordList.Add("--cs-on");
      CoverageScannerKeywordList.Add("--cs-off");
      CoverageScannerKeywordList.Add("--cs-hit");
      CoverageScannerKeywordList.Add("--cs-count");
      CoverageScannerKeywordList.Add("--cs-lock-csexe");
      CoverageScannerKeywordList.Add("--cs-nolock-csexe");
      CoverageScannerKeywordList.Add("--cs-pipe");
      CoverageScannerKeywordList.Add("--cs-nopipe");
      CoverageScannerKeywordList.Add("--cs-no-exceptions");
      CoverageScannerKeywordList.Add("--cs-qt4");
      CoverageScannerKeywordList.Add("--cs-qt3");
      CoverageScannerKeywordList.Add("--cs-compiler=");
      CoverageScannerKeywordList.Add("--cs-libgen=");
      CoverageScannerKeywordList.Add("--cs-include-function-regex=");
      CoverageScannerKeywordList.Add("--cs-exclude-function-regex=");
      CoverageScannerKeywordList.Add("--cs-include-file-regex=");
      CoverageScannerKeywordList.Add("--cs-exclude-file-regex=");
      CoverageScannerKeywordList.Add("--cs-include-function-wildcard=");
      CoverageScannerKeywordList.Add("--cs-exclude-function-wildcard=");
      CoverageScannerKeywordList.Add("--cs-include-file-wildcard=");
      CoverageScannerKeywordList.Add("--cs-exclude-file-wildcard=");
      CoverageScannerKeywordList.Add("--cs-include-path=");
      CoverageScannerKeywordList.Add("--cs-exclude-path=");
      CoverageScannerKeywordList.Add("--cs-output=");
    }

    public bool isTestCocoonAvailable()
    {
      string testcocoon = System.Environment.GetEnvironmentVariable("TESTCOCOON");
      return (testcocoon != null);
    }


    private IEnumerator GetVCProjectRefs()
    {
      IVCCollection projects = null;
      // we must use this way around, since the projects collection from the applicationObject does not work
      // Google News says this is a confirmed VS bug
      for (int i = 1; i < 2; i++)
      {
        try
        {
          VCProject vcP = (VCProject)_applicationObject.Solution.Item(i).Object;
          if (vcP != null)
          {
            VCProjectEngine projEngine = (VCProjectEngine)vcP.VCProjectEngine;
            if (projEngine != null)
            {
              projects = (IVCCollection)projEngine.Projects;
              if (projects != null)
                break;
              else
              {
                Debug(String.Format("Reading item {0} not possible, projects==null", i));
              }
            }
            else
            {
              Debug(String.Format("Reading item {0} not possible, projEngine==null", i));
            }
          }
          else
          {
            Debug(String.Format("Reading item {0} not possible, vcP==null", i));
          }
        }
        catch (Exception e)
        {
          Debug(String.Format("Reading item {0} not possible", i));
          Debug(e);
        }
      }
      return projects.GetEnumerator();
    }

    public void DisableTestCocoonConfig(String config, string project)
    {
      bool foundProject = false;

      IEnumerator e = GetVCProjectRefs();
      e.Reset();
      // traverse all projects to find the right one
      while (e.MoveNext())
      {
        VCProject actVCP = (VCProject)e.Current;
        if (actVCP.Name == project)
        {
          foundProject = true;
          VCConfiguration vcC;
          vcC = (VCConfiguration)(((IVCCollection)actVCP.Configurations).Item(config));
          if (vcC != null)
          {
            Log("Modifying configuration '" + config + "' for the project '" + project + "'");

            // change settings for sepcified compiler
            IVCCollection ctools = (IVCCollection)vcC.Tools;

            VCLinkerTool linkerTool = (VCLinkerTool)(ctools.Item("VCLinkerTool"));
            VCLibrarianTool librarianTool = (VCLibrarianTool)(ctools.Item("VCLibrarianTool"));
            VCCLCompilerTool compilerTool = (VCCLCompilerTool)(ctools.Item("VCCLCompilerTool"));
            VCCustomBuildTool customBuildTool = (VCCustomBuildTool)(ctools.Item("VCCustomBuildTool"));

            DisableClConfig(ref compilerTool, config);
            DisableLinkConfig(ref linkerTool, config);
            DisableCustomBuildConfig(ref customBuildTool, config);
            DisableLibrarianConfig(ref librarianTool, config);
            DisableConfigForEachFile(ref actVCP, config);
          }
          else
            Log("Skipping configuration '" + config + "' for the project '" + project + "'");
        }
      }
      if (!foundProject)
      {
        ShowMessageBox("Could not find the project", "Warning");
      }
    }

    public void CreateTestCocoonConfig(String config, string project, List<string> additionalParamsList, List<string> additiona_includes, bool QtConfiguration)
    {
        bool foundProject = false;

        IEnumerator ProjectsEnumaror = GetVCProjectRefs();
        ProjectsEnumaror.Reset();
        // traverse all projects to find the right one
        while (ProjectsEnumaror.MoveNext())
        {
            VCProject actVCP = (VCProject)ProjectsEnumaror.Current;
            if (actVCP.Name == project)
            {
                foundProject = true;
                VCConfiguration vcC = null;
                //vcC = (VCConfiguration)(((IVCCollection)actVCP.Configurations).Item(config));
                IEnumerator ConfigurationEnumarator = ((IVCCollection)actVCP.Configurations).GetEnumerator();
                for (ConfigurationEnumarator.Reset(); ConfigurationEnumarator.MoveNext(); )
                {
                    vcC = ConfigurationEnumarator.Current as VCConfiguration;
                    if ((vcC != null) && (vcC.ConfigurationName == config))
                    {
                        Log("Modifying configuration '" + config + "' for the project '" + project + "' for the platform '"+vcC.Name+"'");

                        // change settings for sepcified compiler
                        IVCCollection ctools = (IVCCollection)vcC.Tools;

                        VCActiveXReference cVCActiveXReference = ctools.Item("VCActiveXReference") as VCActiveXReference;
                        VCALinkTool cVCALinkTool = ctools.Item("VCALinkTool") as VCALinkTool;
                        VCAppVerifierTool cVCAppVerifierTool = ctools.Item("VCAppVerifierTool") as VCAppVerifierTool;
                        VCAssemblyReference cVCAssemblyReference = ctools.Item("VCAssemblyReference") as VCAssemblyReference;
                        VCBscMakeTool cVCBscMakeTool = ctools.Item("VCBscMakeTool") as VCBscMakeTool;
                        VCCLCompilerTool cVCCLCompilerTool = ctools.Item("VCCLCompilerTool") as VCCLCompilerTool;
                        VCConfiguration cVCConfiguration = ctools.Item("VCConfiguration") as VCConfiguration;
                        VCCustomBuildRule cVCCustomBuildRule = ctools.Item("VCCustomBuildRule") as VCCustomBuildRule;
                        VCCustomBuildTool cVCCustomBuildTool = ctools.Item("VCCustomBuildTool") as VCCustomBuildTool;
                        VCDebugSettings cVCDebugSettings = ctools.Item("VCDebugSettings") as VCDebugSettings;
                        VCFile cVCFile = ctools.Item("VCFile") as VCFile;
                        VCFileConfiguration cVCFileConfiguration = ctools.Item("VCFileConfiguration") as VCFileConfiguration;
                        VCFilter cVCFilter = ctools.Item("VCFilter") as VCFilter;
                        VCFxCopTool cVCFxCopTool = ctools.Item("VCFxCopTool") as VCFxCopTool;
                        VCLibrarianTool cVCLibrarianTool = ctools.Item("VCLibrarianTool") as VCLibrarianTool;
                        VCLinkerTool cVCLinkerTool = ctools.Item("VCLinkerTool") as VCLinkerTool;
                        VCManagedResourceCompilerTool cVCManagedResourceCompilerTool = ctools.Item("VCManagedResourceCompilerTool") as VCManagedResourceCompilerTool;
                        VCManifestTool cVCManifestTool = ctools.Item("VCManifestTool") as VCManifestTool;
                        VCMidlTool cVCMidlTool = ctools.Item("VCMidlTool") as VCMidlTool;
                        VCNMakeTool cVCNMakeTool = ctools.Item("VCNMakeTool") as VCNMakeTool;
                        VCPlatform cVCPlatform = ctools.Item("VCPlatform") as VCPlatform;
                        VCPostBuildEventTool cVCPostBuildEventTool = ctools.Item("VCPostBuildEventTool") as VCPostBuildEventTool;
                        VCPreBuildEventTool cVCPreBuildEventTool = ctools.Item("VCPreBuildEventTool") as VCPreBuildEventTool;
                        VCPreLinkEventTool cVCPreLinkEventTool = ctools.Item("VCPreLinkEventTool") as VCPreLinkEventTool;
                        VCProject cVCProject = ctools.Item("VCProject") as VCProject;
                        VCProjectEngine cVCProjectEngine = ctools.Item("VCProjectEngine") as VCProjectEngine;
                        VCProjectEngineEvents cVCProjectEngineEvents = ctools.Item("VCProjectEngineEvents") as VCProjectEngineEvents;
                        VCProjectEngineObject cVCProjectEngineObject = ctools.Item("VCProjectEngineObject") as VCProjectEngineObject;
                        VCProjectItem cVCProjectItem = ctools.Item("VCProjectItem") as VCProjectItem;
                        VCProjectReference cVCProjectReference = ctools.Item("VCProjectReference") as VCProjectReference;
                        VCPropertySheet cVCPropertySheet = ctools.Item("VCPropertySheet") as VCPropertySheet;
                        VCReference cVCReference = ctools.Item("VCReference") as VCReference;
                        VCReferences cVCReferences = ctools.Item("VCReferences") as VCReferences;
                        VCResourceCompilerTool cVCResourceCompilerTool = ctools.Item("VCResourceCompilerTool") as VCResourceCompilerTool;
                        VCRuntimeBooleanProperty cVCRuntimeBooleanProperty = ctools.Item("VCRuntimeBooleanProperty") as VCRuntimeBooleanProperty;
                        VCRuntimeEnumProperty cVCRuntimeEnumProperty = ctools.Item("VCRuntimeEnumProperty") as VCRuntimeEnumProperty;
                        VCRuntimeEnumValue cVCRuntimeEnumValue = ctools.Item("VCRuntimeEnumValue") as VCRuntimeEnumValue;
                        VCRuntimeIntegerProperty cVCRuntimeIntegerProperty = ctools.Item("VCRuntimeIntegerProperty") as VCRuntimeIntegerProperty;
                        VCRuntimeProperty cVCRuntimeProperty = ctools.Item("VCRuntimeProperty") as VCRuntimeProperty;
                        VCRuntimeStringProperty cVCRuntimeStringProperty = ctools.Item("VCRuntimeStringProperty") as VCRuntimeStringProperty;
                        VCToolFile cVCToolFile = ctools.Item("VCToolFile") as VCToolFile;
                        VCUserMacro cVCUserMacro = ctools.Item("VCUserMacro") as VCUserMacro;
                        VCWebDeploymentTool cVCWebDeploymentTool = ctools.Item("VCWebDeploymentTool") as VCWebDeploymentTool;
                        VCWebServiceProxyGeneratorTool cVCWebServiceProxyGeneratorTool = ctools.Item("VCWebServiceProxyGeneratorTool") as VCWebServiceProxyGeneratorTool;
                        VCXDCMakeTool cVCXDCMakeTool = ctools.Item("VCXDCMakeTool") as VCXDCMakeTool;
                        VCXMLDataGeneratorTool cVCXMLDataGeneratorTool = ctools.Item("VCXMLDataGeneratorTool") as VCXMLDataGeneratorTool;

                        VCLinkerTool linkerTool = ctools.Item("VCLinkerTool") as VCLinkerTool;
                        VCLibrarianTool librarianTool = ctools.Item("VCLibrarianTool") as VCLibrarianTool;
                        VCCLCompilerTool compilerTool = ctools.Item("VCCLCompilerTool") as VCCLCompilerTool;
                        VCCustomBuildTool customBuildTool = ctools.Item("VCCustomBuildTool") as VCCustomBuildTool;
                        string libgen = FindCslibConfig(ref compilerTool);
                        List<string> additionalParamsListLink = new List<string>(additionalParamsList);
                        List<string> additionalParamsListLibrarian = new List<string>(additionalParamsList);
                        if (libgen != null)
                        {
                            additionalParamsListLink.Add("--cs-libgen=" + libgen);
                            additionalParamsListLibrarian.Add("--cs-libgen=" + libgen);
                        }

                        if (compilerTool != null)
                            CreateClConfig(ref compilerTool, additionalParamsList, additiona_includes, actVCP.ProjectDirectory, config);
                        if (linkerTool != null)
                            CreateLinkConfig(ref linkerTool, additionalParamsListLink, config);
                        if (customBuildTool != null)
                            CreateCustomBuildConfig(ref customBuildTool, additionalParamsList, config);
                        if (librarianTool != null)
                            CreateLibrarianConfig(ref librarianTool, additionalParamsListLibrarian, config);
                        if (actVCP != null)
                            CreateConfigForEachFile(ref actVCP, additionalParamsList, config);
                    }
                }
            }
        }
        if (!foundProject)
        {
            ShowMessageBox("Could not find the project", "Warning");
        }
    }

    private string replaceCaseInsensitive(string str, string oldString, string newString)
    {
      string s = str;
      for (int i = 0; i < s.Length - oldString.Length; i++)
      {
        if (s.Substring(i, oldString.Length).ToLower() == oldString.ToLower())
        {
          string news = s.Substring(0, i);
          news += newString;
          int newi = news.Length - 1;
          news += s.Substring(i + oldString.Length);
          s = news;
          i = newi;
        }
      }
      return s;
    }

    private string replacePathInCommandLine(string paths, string oldvariable, string newvariable)
    {
      return replaceCaseInsensitive(paths, oldvariable, newvariable);
    }


    private string replacePath(string paths, string oldvariable, string newvariable)
    {
      string _newvariable = newvariable;
      string _oldvariable = oldvariable;
      string s = paths;
      s = replaceCaseInsensitive(s, "\\" + _oldvariable + "\\", "\\" + _newvariable + "\\");
      s = replaceCaseInsensitive(s, "\\" + _oldvariable + ",", "\\" + _newvariable + ",");
      s = replaceCaseInsensitive(s, "," + _oldvariable + "\\", "," + _newvariable + "\\");
      s = replaceCaseInsensitive(s, "," + _oldvariable + ",", "," + _newvariable + ",");
      try
      {
        if (s.Substring(s.Length - _oldvariable.Length).ToLower() == _oldvariable.ToLower())
        {
          s = s.Substring(0, s.Length - _oldvariable.Length) + _newvariable;
        }
      }
      catch (Exception e)
      {
        Debug(e);
      }
      try
      {
        if (s.Substring(0, _oldvariable.Length).ToLower() == _oldvariable.ToLower())
        {
          s = _newvariable + s.Substring(_oldvariable.Length);
        }
      }
      catch (Exception e)
      {
        Debug(e);
      }
      return s;
    }

    private void DisableCustomBuildConfig(ref VCCustomBuildTool customBuildTool, string BuildMode)
    {
      if (customBuildTool != null)
      {
      }
    }

    private void CreateCustomBuildConfig(ref VCCustomBuildTool customBuildTool, List<string> additionalParamsList, string BuildMode)
    {
      if (customBuildTool != null)
      {

      }
    }

    private string generateParamString(List<String> newParams, string oldParams)
    {
      string r = "";
      foreach (string p in newParams)
      {
        if (!oldParams.Contains(p))
        {
          r += " ";
          r += p;
        }
      }
      return r;
    }

    private void DisableLinkConfig(ref VCLinkerTool linkerTool, string config)
    {
      Log("\tLinker Configuration");
      if (linkerTool != null)
      {
        if (linkerTool.AdditionalOptions != null)
        {
          string newargs = DisableCodeCoverageCommandLineArguments(linkerTool.AdditionalOptions);
          if (linkerTool.AdditionalOptions != newargs)
          {
            linkerTool.AdditionalOptions = newargs;
            Log("\t\tAdditional command line arguments set to '" + newargs + "'");
          }
        }
      }
    }

    private void CreateLinkConfig(ref VCLinkerTool linkerTool, List<string> additionalParamsList, string config)
    {
      Log("\tLinker Configuration");
      if (linkerTool != null)
      {
        if (linkerTool.AdditionalOptions == null)
          linkerTool.AdditionalOptions = " ";
        string additionalParams = generateParamString(additionalParamsList, linkerTool.AdditionalOptions);
        if (additionalParams.Length > 0)
        {
          linkerTool.AdditionalOptions += " " + additionalParams;
          Log("\t\tAdditional command line arguments '" + additionalParams + "' are appended");
        }
      }
    }

    public List<string> FindListOfIncludeDirectories()
    {
      List<string> files = new List<string>();

      IEnumerator e = GetVCProjectRefs();
      e.Reset();
      while (e.MoveNext())
      {
        VCProject actVCP = (VCProject)e.Current;
        IVCCollection filesCollection = (IVCCollection)actVCP.Files;
        if (filesCollection != null)
        {
          for (int item_idx = 0; item_idx < filesCollection.Count + 1; ++item_idx)
          {
            try
            {
              VCFile file = (VCFile)(filesCollection.Item(item_idx));

              if (file != null)
              {
                switch (file.FileType)
                {
                  default:
                    break;
                  case eFileType.eFileTypeCppHeader:
                    {
                      string path = System.IO.Path.GetDirectoryName(file.FullPath);
                      bool found = false;
                      foreach (string item in files)
                      {
                        if (item.ToLower() == path.ToLower())
                        {
                          found = true;
                          break;
                        }
                      }
                      if (!found)
                        files.Add(path);
                    }
                    break;
                }
              }
            }
            catch (Exception exp)
            {
              Debug(exp);
            }
          }
        }
      }
      return files;
    }


    private void DisableConfigForEachFile(ref VCProject actVCP, string BuildMode)
    {
      Log("\tFile Specific Configuration");
      IVCCollection filesCollection = (IVCCollection)actVCP.Files;
      if (filesCollection != null)
      {

        for (int item_idx = 0; item_idx < filesCollection.Count + 1; ++item_idx)
        {
          try
          {
            VCFile file = (VCFile)(filesCollection.Item(item_idx));

            if (file != null)
            {
              try
              { // Preprocessed headers
                VCFileConfiguration fileConfiguration = (VCFileConfiguration)(((IVCCollection)file.FileConfigurations).Item(BuildMode)); // Access the release configuration of this file.
                VCCLCompilerTool compilerTool = (VCCLCompilerTool)fileConfiguration.Tool; // Get the compiler tool associated with this file.

                if (compilerTool.AdditionalOptions != null)
                {
                  string newargs = DisableCodeCoverageCommandLineArguments(compilerTool.AdditionalOptions);
                  if (compilerTool.AdditionalOptions != newargs)
                  {
                    compilerTool.AdditionalOptions = newargs;
                    Log("\t\tAdditional command line arguments set to '" + newargs + "' for the file '" + file.Name + "'");
                  }
                }
              }
              catch (Exception e)
              {
                Debug(e);
              }
            }
          }
          catch (Exception e)
          {
            Debug(e);
          }
        }
      }
    }

    private void CreateConfigForEachFile(ref VCProject actVCP, List<string> additionalParamsList, string BuildMode)
    {
      Log("\tFile Specific Configuration");
      IVCCollection filesCollection = (IVCCollection)actVCP.Files;
      if (filesCollection != null)
      {

        for (int item_idx = 0; item_idx < filesCollection.Count + 1; ++item_idx)
        {
          try
          {
            VCFile file = (VCFile)(filesCollection.Item(item_idx));

            if (file != null)
            {
              try
              { // Preprocessed headers
                VCFileConfiguration fileConfiguration = (VCFileConfiguration)(((IVCCollection)file.FileConfigurations).Item(BuildMode)); // Access the release configuration of this file.
                VCCLCompilerTool compilerTool = (VCCLCompilerTool)fileConfiguration.Tool; // Get the compiler tool associated with this file.

                switch (compilerTool.UsePrecompiledHeader)
                {
                  case pchOption.pchCreateUsingSpecific:
                    break;
                  case pchOption.pchUseUsingSpecific:
                  case pchOption.pchNone:
                    if (compilerTool.UsePrecompiledHeader != pchOption.pchNone)
                    {
                      compilerTool.UsePrecompiledHeader = pchOption.pchNone;
                      Log("\t\tPrecompiled headers are deactivated for the file '" + file.Name + "'");
                      if (compilerTool.PrecompiledHeaderThrough != null)
                      {
                        string directoryToAppend = System.IO.Path.GetDirectoryName(compilerTool.PrecompiledHeaderThrough);
                        if (appendToAdditionalDirectories(ref compilerTool, directoryToAppend))
                          Log("\t\tAdditional of '" + directoryToAppend + "'a in the list of additional include directories for the file '" + file.Name + "'");
                      }
                    }
                    if (compilerTool.AdditionalOptions == null)
                      compilerTool.AdditionalOptions = " ";
                    string additionalParams = generateParamString(additionalParamsList, compilerTool.AdditionalOptions);
                    if (additionalParams.Length > 0)
                    {
                      compilerTool.AdditionalOptions += " " + additionalParams;
                      Log("\t\tAdditional command line arguments '" + additionalParams + "' are appended for the file '" + file.Name + "'");
                    }
                    break;
                }
              }
              catch (Exception e)
              {
                Debug(e);
              }
            }
          }
          catch (Exception e)
          {
            Debug(e);
          }
        }
      }
    }

    private void DisableLibrarianConfig(ref VCLibrarianTool librarianTool, string config)
    {
      if (librarianTool != null)
      {
        Log("\tLibrarian Configuration");
        if (librarianTool.AdditionalOptions != null)
        {
          string newargs = DisableCodeCoverageCommandLineArguments(librarianTool.AdditionalOptions);
          if (librarianTool.AdditionalOptions != newargs)
          {
            librarianTool.AdditionalOptions = newargs;
            Log("\t\tAdditional command line arguments set to '" + newargs + "'");
          }
        }
      }
    }


    private void CreateLibrarianConfig(ref VCLibrarianTool librarianTool, List<string> additionalParamsList, string config)
    {
      if (librarianTool != null)
      {
        Log("\tLibrarian Configuration");
        if (librarianTool.AdditionalOptions == null)
          librarianTool.AdditionalOptions = " ";
        string additionalParams = generateParamString(additionalParamsList, librarianTool.AdditionalOptions);
        if (additionalParams.Length > 0)
        {
          librarianTool.AdditionalOptions += " " + additionalParams;
          Log("\t\tAdditional command line arguments '" + additionalParams + "' are appended");
        }
      }
    }


    private string FindCslibConfig(ref VCCLCompilerTool compilerTool)
    {
      if (compilerTool != null)
      {
        switch (compilerTool.RuntimeLibrary)
        {
          case runtimeLibraryOption.rtMultiThreaded:
            return "/MT";
          case runtimeLibraryOption.rtMultiThreadedDebug:
            return "/MTd";
          case runtimeLibraryOption.rtMultiThreadedDebugDLL:
            return "/MDd";
          case runtimeLibraryOption.rtMultiThreadedDLL:
            return "/MD";
        }
        return null;
      }
      else
        return null;
    }

    private string DisableCodeCoverageCommandLineArguments(string param)
    {

      string newparam = param.Substring(0);
      foreach (string p in CoverageScannerKeywordList)
      {
        int pos;
        for (; ; )
        {
          pos = newparam.IndexOf(p);
          if (pos >= 0)
          {
            bool single_argument = (p[p.Length - 1] != '=');
            int length_to_remove = 0;
            length_to_remove = p.Length;
            if (!single_argument)
            {
              bool end = false;
              bool escape = false;
              for (int i = pos + length_to_remove;
                    (!end) && i < newparam.Length;
                    i++)
              {
                if (newparam[i] == ' ' && !escape)
                  end = true;
                else
                  length_to_remove++;
                if (newparam[i] == '\\')
                  escape = !escape;
                else
                  escape = false;
              }
            }
            newparam = newparam.Remove(pos, length_to_remove);
          }
          else
            break;
        }
      }
      newparam = newparam.TrimEnd(null);
      newparam = newparam.TrimStart(null);
      return newparam;
    }

    private void DisableClConfig(ref VCCLCompilerTool compilerTool, string config)
    {
      if (compilerTool != null)
      {
        Log("\tCompiler Configuration");
        if (compilerTool.AdditionalOptions != null)
        {
          string newargs = DisableCodeCoverageCommandLineArguments(compilerTool.AdditionalOptions);
          if (compilerTool.AdditionalOptions != newargs)
          {
            compilerTool.AdditionalOptions = newargs;
            Log("\t\tAdditional command line arguments set to '" + newargs + "'");
          }
        }
      }
    }

    private bool appendToAdditionalDirectories(ref VCCLCompilerTool compilerTool, string directoryToAppend)
    {
      if (directoryToAppend.Length == 0)
        directoryToAppend = ".";
      string compilerAdditionalInclude;
      if (compilerTool.AdditionalIncludeDirectories == null)
        compilerAdditionalInclude = "";
      else
        compilerAdditionalInclude = compilerTool.AdditionalIncludeDirectories;

      string newCompilerAdditionalInclude = compilerAdditionalInclude;
      string[] addinc_array = compilerAdditionalInclude.Split(',');
      bool found = false;
      foreach (string s in addinc_array)
      {
        if (s == directoryToAppend)
        {
          found = true;
          break;
        }
      }
      if (!found)
      {
        if (newCompilerAdditionalInclude.Length == 0)
          newCompilerAdditionalInclude = directoryToAppend;
        else
        {
          newCompilerAdditionalInclude += ",";
          newCompilerAdditionalInclude += directoryToAppend;
        }
      }

      if (newCompilerAdditionalInclude != compilerAdditionalInclude)
      {
        compilerTool.AdditionalIncludeDirectories = newCompilerAdditionalInclude;
        return true;
      }
      return false;
    }

    private void CreateClConfig(ref VCCLCompilerTool compilerTool, List<string> additionalParamsList, List<string> additionalIncludeDirectories, string project_directory, string config)
    {
      if (compilerTool != null)
      {
        List<string> _additionalParamsList = new List<string>(additionalParamsList);
        Log("\tCompiler Configuration");
        if (compilerTool.UsePrecompiledHeader != pchOption.pchNone)
        {
          compilerTool.UsePrecompiledHeader = pchOption.pchNone;
          Log("\t\tUsage of precompiled headers are deactivated");
          if (compilerTool.PrecompiledHeaderThrough != null)
          {
            try
            {
              string directoryToAppend = System.IO.Path.GetDirectoryName(compilerTool.PrecompiledHeaderThrough);
              if (appendToAdditionalDirectories(ref compilerTool, directoryToAppend))
                Log("\t\tAdditional of '" + directoryToAppend + "'a in the list of additional include directories");
            }
            catch (Exception e)
            {
              Debug(e);
            }
          }
        }

        if (additionalIncludeDirectories != null && compilerTool.FullIncludePath != null)
        {
          string addinc = compilerTool.FullIncludePath;
          string[] addinc_array = addinc.Split(';');
          List<string> addinc_list = new List<string>();
          foreach (string s in addinc_array)
          {
            string ss = s.Replace("\"", "");
            addinc_list.Add(ss);
          }

          foreach (string path in additionalIncludeDirectories)
          {
            try
            {
              string path_abs = System.IO.Path.GetFullPath(path).ToLower();
              foreach (string addinc_rel in addinc_list)
              {
                try
                {
                  string addinc_abs = System.IO.Path.GetFullPath(addinc_rel).ToLower();
                  string addinc_ver = addinc_rel.ToLower();
                  if (addinc_abs != addinc_rel.ToLower())
                  {
                    try
                    {
                      addinc_abs = System.IO.Path.GetFullPath(project_directory + "\\" + addinc_rel).ToLower();
                    }
                    catch (Exception e)
                    {
                      Debug(e);
                    }
                  }
                  if (addinc_abs == path_abs)
                  {
                    _additionalParamsList.Add("--cs-include-path=\"" + addinc_rel + "\"");
                    break;
                  }
                }
                catch (Exception e)
                {
                  Debug(e);
                }
              }
            }
            catch (Exception e)
            {
              Debug(e);
            }
          }
        }



        if (compilerTool.AdditionalOptions == null)
          compilerTool.AdditionalOptions = " ";
        string additionalParams = generateParamString(_additionalParamsList, compilerTool.AdditionalOptions);
        if (additionalParams.Length > 0)
        {
          compilerTool.AdditionalOptions += " " + additionalParams;
          Log("\t\tAdditional command line arguments '" + additionalParams + "' are appended");
        }
      }
    }


    public String[] FindConfigs(String project, String platform)
    {
      String[] entries = null;
      int actCount = 0;

      IEnumerator e = GetVCProjectRefs();
      e.Reset();
      while (e.MoveNext())
      {
        VCProject actVCP = (VCProject)e.Current;
        if (actVCP.Name == project)
        {
          IVCCollection configs = (IVCCollection)actVCP.Configurations;
          entries = new String[configs.Count];
          IEnumerator eConfs = configs.GetEnumerator();
          eConfs.Reset();
          while (eConfs.MoveNext())
          {
            String actEntry = ((VCConfiguration)eConfs.Current).Name;
            // configuration name comes in the form "Debug|Win32"
            actEntry = actEntry.Substring(0, actEntry.IndexOf("|"));
            entries[actCount++] = actEntry;
          }
        }
      }
      return entries;
    }


    public void FindProjects(ref ArrayList entries)
    {
      IEnumerator e = GetVCProjectRefs();
      e.Reset();
      while (e.MoveNext())
      {
        VCProject actVCP = (VCProject)e.Current;
        entries.Add(actVCP.Name);
      }
    }

    public string FindActiveProject()
    {
      try
      {
        Array vcps = (Array)_applicationObject.ActiveSolutionProjects;
        EnvDTE.Project theProject = (EnvDTE.Project)(vcps.GetValue(0));
        return theProject.Name;
      }
      catch (Exception e)
      {
        Debug(e);
        return "";
      }
    }



    public void AdjustPath()
    {
        CorrectVCppDirectories();
    }

    private bool CorrectVCppDirectories()
    {
        string testcocoon_install_path = TestCocoonInstallPath();
        try
        {
            for (int iplfCol = 1; iplfCol <= _applicationObject.Solution.Count; iplfCol++)
            {
                try
                {
                    VCProject prj = (VCProject)_applicationObject.Solution.Item(iplfCol).Object;
                    IVCCollection plfColl = (IVCCollection)prj.Platforms;
                    for (int iPlf = 1; iPlf <= plfColl.Count; iPlf++)
                    {
                        try
                        {
                            VCPlatform plf = (VCPlatform)plfColl.Item(iPlf);
                            string dirs = plf.ExecutableDirectories;
                            string[] dirList = dirs.Split(';');
                            bool append_testcocoon_path = false;
                            if (dirList.Length == 0)
                                append_testcocoon_path = true;
                            else if (dirList[0] != testcocoon_install_path)
                                append_testcocoon_path = true;

                            if (append_testcocoon_path)
                            {
                                plf.ExecutableDirectories = testcocoon_install_path + ";" + dirs;
                                prj.Save();
                                Log("Directory '" + testcocoon_install_path + 
                                    "' added in the list of executable directories for the platform '" + plf.Name + "'");
                            }
                        }
                        catch (Exception e)
                        {
                            // maybe the plugin was loaded without an open solution
                            Debug(e);
                        }
                    }
                }
                catch (Exception e)
                {
                    // maybe the plugin was loaded without an open solution
                    Debug(e);
                }
            }
            return true;
        }
        catch (Exception e)
        {
            // maybe the plugin was loaded without an open solution
            Debug(e);
            return false;
        }
    }

    private void Debug(Exception l)
    {
      VsConnect.Debug(l);
    }

    private void Debug(string l)
    {
      VsConnect.Debug(l);
    }

    private void Log(string l)
    {
      VsConnect.Log(l);
    }

    private void ShowMessageBox(string text, string title)
    {
      VsConnect.ShowMessageBox(text, title);
    }


    static bool InstallationErrorDisplayed = false;
    public string TestCocoonInstallPath()
    {
      string TESTCOCOON = "TESTCOCOON";
      if (!InstallationErrorDisplayed)
      {
        try
        {
          string TestCocoonPath = System.Environment.GetEnvironmentVariable(TESTCOCOON);
          if (TestCocoonPath == null || TestCocoonPath.Length == 0)
          {
            InstallationErrorDisplayed = true;
            ShowMessageBox("TESTCOCOON system variable is not defined.", "Error");
          }
        }
        catch (Exception e)
        {
          Debug(e);
          InstallationErrorDisplayed = true;
          ShowMessageBox("TESTCOCOON system variable is not defined.", "Error");
        }
      }
      string installPath = "$(" + TESTCOCOON + ")";
      installPath += "\\visualstudio";
      return installPath;
    }

    private OutputWindowPane _outputWindowPane { get { return VsConnect._outputWindowPane; } }
    private DTE2 _applicationObject { get { return VsConnect._applicationObject; } }
    private AddIn _addInInstance { get { return VsConnect._addInInstance; } }
  }
}
