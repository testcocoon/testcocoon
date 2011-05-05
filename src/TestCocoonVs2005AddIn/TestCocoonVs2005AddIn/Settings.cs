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
  class Settings
  {
     static private Microsoft.Win32.RegistryKey RootRegistryKey(bool read_only)
     {
        String registry_root = @"SOFTWARE\TestCocoon";
        try 
        {
           Microsoft.Win32.Registry.CurrentUser.CreateSubKey(registry_root);
        }
        catch { }

        registry_root += @"\visualstudioaddin";
        try 
        {
           Microsoft.Win32.Registry.CurrentUser.CreateSubKey(registry_root);
        }
        catch { }

        Microsoft.Win32.RegistryKey registryKey;
        registryKey = Microsoft.Win32.Registry.CurrentUser.OpenSubKey(registry_root, read_only);
        return registryKey;
     }


     static public bool GetBool(String Key, bool default_val)
     {
        try
        {
           int registryValue;
           Microsoft.Win32.RegistryKey registryKey = RootRegistryKey(false);

           registryValue = (int)registryKey.GetValue(Key, default_val ? 1 : 0);
           return registryValue != 0;
        }
        catch
        {
           return default_val;
        }
     }

     static public void SetBool(String Key, bool Value)
     {
        try
        {
           Microsoft.Win32.RegistryKey registryKey = RootRegistryKey(true);

           registryKey.SetValue(Key, Value ? 1 : 0, Microsoft.Win32.RegistryValueKind.DWord);
        }
        catch { }
     }

     static public String GetString(String Key, String default_val)
     {
        try
        {
           Microsoft.Win32.RegistryKey registryKey = RootRegistryKey(false);
           String registryValue;

           registryValue = (String)registryKey.GetValue(Key, default_val);
           return registryValue;
        }
        catch
        {
           return default_val;
        }
     }

     static public void SetString(String Key, String Value)
     {
        try
        {
           Microsoft.Win32.RegistryKey registryKey = RootRegistryKey(true);

           registryKey.SetValue(Key, Value, Microsoft.Win32.RegistryValueKind.String);
        }
        catch { }
     }
  }
}
