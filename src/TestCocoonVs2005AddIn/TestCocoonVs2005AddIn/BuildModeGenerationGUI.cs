using System;
using System.Drawing;
using System.Collections;
using System.ComponentModel;
using System.Windows.Forms;
using System.Reflection;
using System.Collections.Generic;

namespace TestCocoonVs2005AddIn
{
  public class BuildModeGenerationGUI : System.Windows.Forms.Form
  {
    public enum action_t { ENABLE_COVERAGE_ENV, DISABLE_COVERAGE_ENV, ENABLE_COVERAGE_CONFIG, DISABLE_COVERAGE_CONFIG, NONE } ;
    private System.Windows.Forms.Label label6;
    public System.Windows.Forms.ComboBox _projects;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.Button cancel;
    private System.Windows.Forms.Button enableCoverage;
    public System.Windows.Forms.ComboBox _config;
    private System.Windows.Forms.Label label1;
    private Hashtable projectConfigs;
    public action_t Action;
    private System.Windows.Forms.TextBox additionalParameters;
    private CheckBox _allProjects;
    private GroupBox groupBox1;
    private GroupBox groupBox2;
    private RadioButton CoverageCount;
    private RadioButton CoverageHit;
    private RadioButton CoverageBranch;
    private RadioButton CoverageCondition;
    private RadioButton CoverageDecision;
    private GroupBox CoverageConditionAlgorithm;
    private RadioButton CoveragePartialInstrumentation;
    private RadioButton CoverageFullInstrumentation;
    private GroupBox TrolltechQt;
    private RadioButton QtNone;
    private RadioButton Qt4;
    private RadioButton Qt3;
    private Button disableCoverage;
    private ListBox additionalIncludes;
    private GroupBox groupBox3;
    private CheckBox csexe_lockfile;
    private GroupBox groupBox4;
    private RadioButton tryandcatch;
    private RadioButton tryblock;
    private System.ComponentModel.Container components = null;

    public bool AllProjects()
    {
      return _allProjects.Checked;
    }

    public void setAdditionalIncludes(List<string> files)
    {
      additionalIncludes.Items.Clear();
      foreach (string item in files)
      {
        additionalIncludes.Items.Add(item);
      }
      for (int index = 0; index < additionalIncludes.Items.Count; index++)
        additionalIncludes.SetSelected(index, true);
    }

    public List<string> getAdditionalIncludes()
    {
      List<string> files = new List<string>();
      for (int index = 0; index < additionalIncludes.Items.Count; index++)
      {
        if (additionalIncludes.SelectedIndices.Contains(index))
          files.Add((string)additionalIncludes.Items[index]);
      }
      return files;
    }

    public List<String> Parameters()
    {
      List<string> args = new List<string>();
      args.Add(additionalParameters.Text);

      args.Add("--cs-on");

      if (CoverageBranch.Checked)
        args.Add("--cs-branch");
      else if (CoverageCondition.Checked)
        args.Add("--cs-condition");
      else if (CoverageDecision.Checked)
        args.Add("--cs-decision");

      if (CoverageCount.Checked)
        args.Add("--cs-count");
      else if (CoverageHit.Checked)
        args.Add("--cs-hit");

      if (CoveragePartialInstrumentation.Checked)
        args.Add("--cs-partial-instrumentation");
      else if (CoverageFullInstrumentation.Checked)
        args.Add("--cs-full-instrumentation");

      if (Qt3.Checked)
        args.Add("--cs-qt3");
      else if (Qt4.Checked)
        args.Add("--cs-qt4");

      if (csexe_lockfile.Checked)
        args.Add("--cs-lock-csexe");

      if (tryblock.Checked)
        args.Add("--cs-no-exceptions");

      return args;
    }

    public BuildModeGenerationGUI()
    {
      InitializeComponent();
      Action = action_t.NONE;
      projectConfigs = new Hashtable();
      ResoreSettings();
    }

     private void SaveSettings()
     {
        Settings.SetBool("LOCKFILE", csexe_lockfile.Checked);
        Settings.SetString("additionalParameters", additionalParameters.Text);
        Settings.SetBool("CoverageBranch", CoverageBranch.Checked);
        Settings.SetBool("CoverageCondition", CoverageCondition.Checked);
        Settings.SetBool("CoverageDecision", CoverageDecision.Checked);
        Settings.SetBool("CoverageCount", CoverageCount.Checked);
        Settings.SetBool("CoverageHit", CoverageHit.Checked);
        Settings.SetBool("CoveragePartialInstrumentation", CoveragePartialInstrumentation.Checked);
        Settings.SetBool("CoverageFullInstrumentation", CoverageFullInstrumentation.Checked);
        Settings.SetBool("Qt3", Qt3.Checked);
        Settings.SetBool("Qt4", Qt4.Checked);
        Settings.SetBool("tryblock", tryblock.Checked);
     }

     private void ResoreSettings()
     {
        csexe_lockfile.Checked                 = Settings.GetBool("LOCKFILE", csexe_lockfile.Checked);
        additionalParameters.Text              = Settings.GetString("additionalParameters", additionalParameters.Text);
        CoverageBranch.Checked                 = Settings.GetBool("CoverageBranch", CoverageBranch.Checked);
        CoverageCondition.Checked              = Settings.GetBool("CoverageCondition", CoverageCondition.Checked);
        CoverageDecision.Checked               = Settings.GetBool("CoverageDecision", CoverageDecision.Checked);
        CoverageCount.Checked                  = Settings.GetBool("CoverageCount", CoverageCount.Checked);
        CoverageHit.Checked                    = Settings.GetBool("CoverageHit", CoverageHit.Checked);
        CoveragePartialInstrumentation.Checked = Settings.GetBool("CoveragePartialInstrumentation", CoveragePartialInstrumentation.Checked);
        CoverageFullInstrumentation.Checked    = Settings.GetBool("CoverageFullInstrumentation", CoverageFullInstrumentation.Checked);
        Qt3.Checked                            = Settings.GetBool("Qt3", Qt3.Checked);
        Qt4.Checked                            = Settings.GetBool("Qt4", Qt4.Checked);
        tryblock.Checked                       = Settings.GetBool("tryblock", tryblock.Checked);
     }

    public void AddConfigs(String project, String[] entries)
    {
      projectConfigs.Add(project, entries);
    }

    public void FillProjects(ref ArrayList entries, string choosen)
    {
      foreach (String entry in entries) _projects.Items.Add(entry);
      if (choosen != "") _projects.SelectedItem = choosen;
    }

    protected override void Dispose(bool disposing)
    {
      if (disposing)
      {
        if (components != null)
        {
          components.Dispose();
        }
      }
      base.Dispose(disposing);
    }

    #region Windows Form Designer generated code
    /// <summary>
    /// Required method for Designer support - do not modify
    /// the contents of this method with the code editor.
    /// </summary>
    private void InitializeComponent()
    {
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(BuildModeGenerationGUI));
      this.label6 = new System.Windows.Forms.Label();
      this._projects = new System.Windows.Forms.ComboBox();
      this.label3 = new System.Windows.Forms.Label();
      this.cancel = new System.Windows.Forms.Button();
      this.enableCoverage = new System.Windows.Forms.Button();
      this._config = new System.Windows.Forms.ComboBox();
      this.label1 = new System.Windows.Forms.Label();
      this.additionalParameters = new System.Windows.Forms.TextBox();
      this._allProjects = new System.Windows.Forms.CheckBox();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.CoverageCount = new System.Windows.Forms.RadioButton();
      this.CoverageHit = new System.Windows.Forms.RadioButton();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.CoverageBranch = new System.Windows.Forms.RadioButton();
      this.CoverageCondition = new System.Windows.Forms.RadioButton();
      this.CoverageDecision = new System.Windows.Forms.RadioButton();
      this.CoverageConditionAlgorithm = new System.Windows.Forms.GroupBox();
      this.CoveragePartialInstrumentation = new System.Windows.Forms.RadioButton();
      this.CoverageFullInstrumentation = new System.Windows.Forms.RadioButton();
      this.TrolltechQt = new System.Windows.Forms.GroupBox();
      this.QtNone = new System.Windows.Forms.RadioButton();
      this.Qt4 = new System.Windows.Forms.RadioButton();
      this.Qt3 = new System.Windows.Forms.RadioButton();
      this.disableCoverage = new System.Windows.Forms.Button();
      this.additionalIncludes = new System.Windows.Forms.ListBox();
      this.groupBox3 = new System.Windows.Forms.GroupBox();
      this.csexe_lockfile = new System.Windows.Forms.CheckBox();
      this.groupBox4 = new System.Windows.Forms.GroupBox();
      this.tryandcatch = new System.Windows.Forms.RadioButton();
      this.tryblock = new System.Windows.Forms.RadioButton();
      this.groupBox1.SuspendLayout();
      this.groupBox2.SuspendLayout();
      this.CoverageConditionAlgorithm.SuspendLayout();
      this.TrolltechQt.SuspendLayout();
      this.groupBox3.SuspendLayout();
      this.groupBox4.SuspendLayout();
      this.SuspendLayout();
      // 
      // label6
      // 
      this.label6.Location = new System.Drawing.Point(9, 64);
      this.label6.Name = "label6";
      this.label6.Size = new System.Drawing.Size(152, 14);
      this.label6.TabIndex = 23;
      this.label6.Text = "Additional Parameters:";
      this.label6.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // _projects
      // 
      this._projects.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this._projects.Location = new System.Drawing.Point(131, 4);
      this._projects.Name = "_projects";
      this._projects.Size = new System.Drawing.Size(445, 21);
      this._projects.TabIndex = 21;
      this._projects.SelectedIndexChanged += new System.EventHandler(this.OnChange);
      // 
      // label3
      // 
      this.label3.Location = new System.Drawing.Point(8, 32);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(80, 20);
      this.label3.TabIndex = 20;
      this.label3.Text = "Configuration:";
      this.label3.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // cancel
      // 
      this.cancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.cancel.Location = new System.Drawing.Point(546, 372);
      this.cancel.Name = "cancel";
      this.cancel.Size = new System.Drawing.Size(88, 24);
      this.cancel.TabIndex = 19;
      this.cancel.Text = "Cancel";
      this.cancel.Click += new System.EventHandler(this.cancel_Click);
      // 
      // enableCoverage
      // 
      this.enableCoverage.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.enableCoverage.Location = new System.Drawing.Point(382, 372);
      this.enableCoverage.Name = "enableCoverage";
      this.enableCoverage.Size = new System.Drawing.Size(136, 24);
      this.enableCoverage.TabIndex = 18;
      this.enableCoverage.Text = "Enable code coverage";
      this.enableCoverage.Click += new System.EventHandler(this.enableCoverageClicked);
      // 
      // _config
      // 
      this._config.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this._config.Location = new System.Drawing.Point(131, 31);
      this._config.Name = "_config";
      this._config.Size = new System.Drawing.Size(500, 21);
      this._config.TabIndex = 15;
      this._config.SelectedIndexChanged += new System.EventHandler(this.BaseConfiguration_Changed);
      // 
      // label1
      // 
      this.label1.Location = new System.Drawing.Point(8, 9);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(152, 16);
      this.label1.TabIndex = 14;
      this.label1.Text = "Project:";
      this.label1.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
      // 
      // additionalParameters
      // 
      this.additionalParameters.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.additionalParameters.Location = new System.Drawing.Point(131, 58);
      this.additionalParameters.Name = "additionalParameters";
      this.additionalParameters.Size = new System.Drawing.Size(500, 20);
      this.additionalParameters.TabIndex = 25;
      // 
      // _allProjects
      // 
      this._allProjects.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
      this._allProjects.AutoSize = true;
      this._allProjects.Location = new System.Drawing.Point(597, 6);
      this._allProjects.Name = "_allProjects";
      this._allProjects.Size = new System.Drawing.Size(37, 17);
      this._allProjects.TabIndex = 26;
      this._allProjects.Text = "All";
      this._allProjects.UseVisualStyleBackColor = true;
      this._allProjects.CheckedChanged += new System.EventHandler(this.allProjects_Checked);
      // 
      // groupBox1
      // 
      this.groupBox1.Controls.Add(this.CoverageCount);
      this.groupBox1.Controls.Add(this.CoverageHit);
      this.groupBox1.Location = new System.Drawing.Point(12, 95);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(97, 72);
      this.groupBox1.TabIndex = 27;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "Instrumentation";
      // 
      // CoverageCount
      // 
      this.CoverageCount.AutoSize = true;
      this.CoverageCount.Location = new System.Drawing.Point(16, 42);
      this.CoverageCount.Name = "CoverageCount";
      this.CoverageCount.Size = new System.Drawing.Size(53, 17);
      this.CoverageCount.TabIndex = 1;
      this.CoverageCount.Text = "Count";
      this.CoverageCount.UseVisualStyleBackColor = true;
      // 
      // CoverageHit
      // 
      this.CoverageHit.AutoSize = true;
      this.CoverageHit.Checked = true;
      this.CoverageHit.Location = new System.Drawing.Point(16, 19);
      this.CoverageHit.Name = "CoverageHit";
      this.CoverageHit.Size = new System.Drawing.Size(38, 17);
      this.CoverageHit.TabIndex = 0;
      this.CoverageHit.TabStop = true;
      this.CoverageHit.Text = "Hit";
      this.CoverageHit.UseVisualStyleBackColor = true;
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.CoverageBranch);
      this.groupBox2.Controls.Add(this.CoverageCondition);
      this.groupBox2.Controls.Add(this.CoverageDecision);
      this.groupBox2.Location = new System.Drawing.Point(115, 95);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(101, 100);
      this.groupBox2.TabIndex = 28;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Method";
      // 
      // CoverageBranch
      // 
      this.CoverageBranch.AutoSize = true;
      this.CoverageBranch.Location = new System.Drawing.Point(16, 19);
      this.CoverageBranch.Name = "CoverageBranch";
      this.CoverageBranch.Size = new System.Drawing.Size(59, 17);
      this.CoverageBranch.TabIndex = 2;
      this.CoverageBranch.Text = "Branch";
      this.CoverageBranch.UseVisualStyleBackColor = true;
      // 
      // CoverageCondition
      // 
      this.CoverageCondition.AutoSize = true;
      this.CoverageCondition.Checked = true;
      this.CoverageCondition.Location = new System.Drawing.Point(16, 65);
      this.CoverageCondition.Name = "CoverageCondition";
      this.CoverageCondition.Size = new System.Drawing.Size(69, 17);
      this.CoverageCondition.TabIndex = 1;
      this.CoverageCondition.TabStop = true;
      this.CoverageCondition.Text = "Condition";
      this.CoverageCondition.UseVisualStyleBackColor = true;
      // 
      // CoverageDecision
      // 
      this.CoverageDecision.AutoSize = true;
      this.CoverageDecision.Location = new System.Drawing.Point(16, 42);
      this.CoverageDecision.Name = "CoverageDecision";
      this.CoverageDecision.Size = new System.Drawing.Size(66, 17);
      this.CoverageDecision.TabIndex = 0;
      this.CoverageDecision.Text = "Decision";
      this.CoverageDecision.UseVisualStyleBackColor = true;
      // 
      // CoverageConditionAlgorithm
      // 
      this.CoverageConditionAlgorithm.Controls.Add(this.CoveragePartialInstrumentation);
      this.CoverageConditionAlgorithm.Controls.Add(this.CoverageFullInstrumentation);
      this.CoverageConditionAlgorithm.Location = new System.Drawing.Point(222, 100);
      this.CoverageConditionAlgorithm.Name = "CoverageConditionAlgorithm";
      this.CoverageConditionAlgorithm.Size = new System.Drawing.Size(152, 77);
      this.CoverageConditionAlgorithm.TabIndex = 29;
      this.CoverageConditionAlgorithm.TabStop = false;
      this.CoverageConditionAlgorithm.Text = "Conditions/Decision";
      // 
      // CoveragePartialInstrumentation
      // 
      this.CoveragePartialInstrumentation.AutoSize = true;
      this.CoveragePartialInstrumentation.Checked = true;
      this.CoveragePartialInstrumentation.Location = new System.Drawing.Point(16, 42);
      this.CoveragePartialInstrumentation.Name = "CoveragePartialInstrumentation";
      this.CoveragePartialInstrumentation.Size = new System.Drawing.Size(124, 17);
      this.CoveragePartialInstrumentation.TabIndex = 1;
      this.CoveragePartialInstrumentation.TabStop = true;
      this.CoveragePartialInstrumentation.Text = "Partially instrumented";
      this.CoveragePartialInstrumentation.UseVisualStyleBackColor = true;
      // 
      // CoverageFullInstrumentation
      // 
      this.CoverageFullInstrumentation.AutoSize = true;
      this.CoverageFullInstrumentation.Location = new System.Drawing.Point(16, 19);
      this.CoverageFullInstrumentation.Name = "CoverageFullInstrumentation";
      this.CoverageFullInstrumentation.Size = new System.Drawing.Size(104, 17);
      this.CoverageFullInstrumentation.TabIndex = 0;
      this.CoverageFullInstrumentation.Text = "Full instrumented";
      this.CoverageFullInstrumentation.UseVisualStyleBackColor = true;
      // 
      // TrolltechQt
      // 
      this.TrolltechQt.Controls.Add(this.QtNone);
      this.TrolltechQt.Controls.Add(this.Qt4);
      this.TrolltechQt.Controls.Add(this.Qt3);
      this.TrolltechQt.Location = new System.Drawing.Point(380, 100);
      this.TrolltechQt.Name = "TrolltechQt";
      this.TrolltechQt.Size = new System.Drawing.Size(84, 100);
      this.TrolltechQt.TabIndex = 30;
      this.TrolltechQt.TabStop = false;
      this.TrolltechQt.Text = "Trolltech Qt";
      // 
      // QtNone
      // 
      this.QtNone.AutoSize = true;
      this.QtNone.Checked = true;
      this.QtNone.Location = new System.Drawing.Point(16, 19);
      this.QtNone.Name = "QtNone";
      this.QtNone.Size = new System.Drawing.Size(51, 17);
      this.QtNone.TabIndex = 2;
      this.QtNone.TabStop = true;
      this.QtNone.Text = "None";
      this.QtNone.UseVisualStyleBackColor = true;
      // 
      // Qt4
      // 
      this.Qt4.AutoSize = true;
      this.Qt4.Location = new System.Drawing.Point(16, 65);
      this.Qt4.Name = "Qt4";
      this.Qt4.Size = new System.Drawing.Size(42, 17);
      this.Qt4.TabIndex = 1;
      this.Qt4.Text = "Qt4";
      this.Qt4.UseVisualStyleBackColor = true;
      // 
      // Qt3
      // 
      this.Qt3.AutoSize = true;
      this.Qt3.Location = new System.Drawing.Point(16, 42);
      this.Qt3.Name = "Qt3";
      this.Qt3.Size = new System.Drawing.Size(42, 17);
      this.Qt3.TabIndex = 0;
      this.Qt3.Text = "Qt3";
      this.Qt3.UseVisualStyleBackColor = true;
      // 
      // disableCoverage
      // 
      this.disableCoverage.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
      this.disableCoverage.Location = new System.Drawing.Point(208, 373);
      this.disableCoverage.Name = "disableCoverage";
      this.disableCoverage.Size = new System.Drawing.Size(139, 23);
      this.disableCoverage.TabIndex = 31;
      this.disableCoverage.Text = "Disable code coverage";
      this.disableCoverage.UseVisualStyleBackColor = true;
      this.disableCoverage.Click += new System.EventHandler(this.disableCoverageClicked);
      // 
      // additionalIncludes
      // 
      this.additionalIncludes.Dock = System.Windows.Forms.DockStyle.Fill;
      this.additionalIncludes.FormattingEnabled = true;
      this.additionalIncludes.Location = new System.Drawing.Point(3, 16);
      this.additionalIncludes.Name = "additionalIncludes";
      this.additionalIncludes.SelectionMode = System.Windows.Forms.SelectionMode.MultiExtended;
      this.additionalIncludes.Size = new System.Drawing.Size(616, 134);
      this.additionalIncludes.TabIndex = 32;
      this.additionalIncludes.SelectedIndexChanged += new System.EventHandler(this.additionalIncludes_SelectedIndexChanged);
      // 
      // groupBox3
      // 
      this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom)
                  | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.groupBox3.Controls.Add(this.additionalIncludes);
      this.groupBox3.Location = new System.Drawing.Point(12, 211);
      this.groupBox3.Name = "groupBox3";
      this.groupBox3.Size = new System.Drawing.Size(622, 155);
      this.groupBox3.TabIndex = 33;
      this.groupBox3.TabStop = false;
      this.groupBox3.Text = "Include Directories";
      // 
      // csexe_lockfile
      // 
      this.csexe_lockfile.AutoSize = true;
      this.csexe_lockfile.Location = new System.Drawing.Point(470, 188);
      this.csexe_lockfile.Name = "csexe_lockfile";
      this.csexe_lockfile.Size = new System.Drawing.Size(66, 17);
      this.csexe_lockfile.TabIndex = 34;
      this.csexe_lockfile.Text = "Lock file";
      this.csexe_lockfile.UseVisualStyleBackColor = true;
      this.csexe_lockfile.CheckedChanged += new System.EventHandler(this.csexe_lockfile_CheckedChanged);
      // 
      // groupBox4
      // 
      this.groupBox4.Controls.Add(this.tryandcatch);
      this.groupBox4.Controls.Add(this.tryblock);
      this.groupBox4.Location = new System.Drawing.Point(470, 105);
      this.groupBox4.Name = "groupBox4";
      this.groupBox4.Size = new System.Drawing.Size(166, 77);
      this.groupBox4.TabIndex = 35;
      this.groupBox4.TabStop = false;
      this.groupBox4.Text = "Exceptions";
      // 
      // tryandcatch
      // 
      this.tryandcatch.AutoSize = true;
      this.tryandcatch.Checked = true;
      this.tryandcatch.Location = new System.Drawing.Point(20, 46);
      this.tryandcatch.Name = "tryandcatch";
      this.tryandcatch.Size = new System.Drawing.Size(121, 17);
      this.tryandcatch.TabIndex = 1;
      this.tryandcatch.TabStop = true;
      this.tryandcatch.Text = "Try and Catch block";
      this.tryandcatch.UseVisualStyleBackColor = true;
      this.tryandcatch.CheckedChanged += new System.EventHandler(this.tryandcatch_CheckedChanged);
      // 
      // tryblock
      // 
      this.tryblock.AutoSize = true;
      this.tryblock.Location = new System.Drawing.Point(20, 23);
      this.tryblock.Name = "tryblock";
      this.tryblock.Size = new System.Drawing.Size(91, 17);
      this.tryblock.TabIndex = 0;
      this.tryblock.Text = "Try block only";
      this.tryblock.UseVisualStyleBackColor = true;
      this.tryblock.CheckedChanged += new System.EventHandler(this.tryblock_CheckedChanged);
      // 
      // BuildModeGenerationGUI
      // 
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Inherit;
      this.AutoSize = true;
      this.ClientSize = new System.Drawing.Size(646, 408);
      this.Controls.Add(this.groupBox4);
      this.Controls.Add(this.csexe_lockfile);
      this.Controls.Add(this.groupBox3);
      this.Controls.Add(this.disableCoverage);
      this.Controls.Add(this.TrolltechQt);
      this.Controls.Add(this.CoverageConditionAlgorithm);
      this.Controls.Add(this.groupBox2);
      this.Controls.Add(this.groupBox1);
      this.Controls.Add(this._allProjects);
      this.Controls.Add(this.additionalParameters);
      this.Controls.Add(this.label6);
      this.Controls.Add(this._projects);
      this.Controls.Add(this.label3);
      this.Controls.Add(this.cancel);
      this.Controls.Add(this.enableCoverage);
      this.Controls.Add(this._config);
      this.Controls.Add(this.label1);
      this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
      this.Icon = ((System.Drawing.Icon)(resources.GetObject("$this.Icon")));
      this.MinimumSize = new System.Drawing.Size(652, 350);
      this.Name = "BuildModeGenerationGUI";
      this.SizeGripStyle = System.Windows.Forms.SizeGripStyle.Show;
      this.Text = "Configuration Generation for TestCocoon";
      this.Load += new System.EventHandler(this.BuildModeGenerationGUI_Load);
      this.groupBox1.ResumeLayout(false);
      this.groupBox1.PerformLayout();
      this.groupBox2.ResumeLayout(false);
      this.groupBox2.PerformLayout();
      this.CoverageConditionAlgorithm.ResumeLayout(false);
      this.CoverageConditionAlgorithm.PerformLayout();
      this.TrolltechQt.ResumeLayout(false);
      this.TrolltechQt.PerformLayout();
      this.groupBox3.ResumeLayout(false);
      this.groupBox4.ResumeLayout(false);
      this.groupBox4.PerformLayout();
      this.ResumeLayout(false);
      this.PerformLayout();

    }
    #endregion

    private void enableCoverageClicked(object sender, System.EventArgs e)
    {
      Action = action_t.ENABLE_COVERAGE_CONFIG;
      SaveSettings();
      this.Close();
    }

    private void cancel_Click(object sender, System.EventArgs e)
    {
      Action = action_t.NONE;
      SaveSettings();
      this.Close();
    }

    private void OnChange(object sender, System.EventArgs e)
    {
      // change in the project choice, update config section
      _config.Items.Clear();
      String[] actProjectConfigs = (String[])projectConfigs[_projects.SelectedItem];
      _config.Items.AddRange(actProjectConfigs);
      _config.SelectedItem = actProjectConfigs[0];
    }

    private void allProjects_Checked(object sender, EventArgs e)
    {
      _projects.Enabled = (!AllProjects());
    }

    private void BaseConfiguration_Changed(object sender, EventArgs e)
    {
    }

    private void BuildModeGenerationGUI_Load(object sender, EventArgs e)
    {
      string qtdir = System.Environment.GetEnvironmentVariable("QTDIR");
      TrolltechQt.Enabled = (qtdir != null);
      if (!TrolltechQt.Enabled)
        QtNone.Checked = true;
    }

    public bool GenerateQtConfiguration
    {
      get
      {
        if (!TrolltechQt.Enabled)
          return false;
        if (QtNone.Checked)
          return false;
        return true;
      }
    }

    private void disableCoverageClicked(object sender, EventArgs e)
    {
      Action = action_t.DISABLE_COVERAGE_CONFIG;
      this.Close();
    }

    private void additionalIncludes_SelectedIndexChanged(object sender, EventArgs e)
    {

    }

    private void tryblock_CheckedChanged(object sender, EventArgs e)
    {

    }

    private void tryandcatch_CheckedChanged(object sender, EventArgs e)
    {

    }

    private void csexe_lockfile_CheckedChanged(object sender, EventArgs e)
    {

    }

  }
}
