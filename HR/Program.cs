using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HR
{
	class Program
	{
		static void Main(string[] args)
		{
			Console.WriteLine("HotReload command-line utility");
			if (args.Length < 1)
			{
				Console.WriteLine("Usage: HR <process-name | pid:num> <command>");
				return;
			}

			Process process = null;
			try
			{
				if (args[0].StartsWith("pid:"))
				{
					int pid = Int32.Parse(args[0].Substring(4));
					process = Process.GetProcessById(pid);
				}
				else
				{
					string name = args[0];
					// Process.GetProcessesByName expects are name without trailing .exe
					if (name.ToLower().EndsWith(".exe"))
						name = name.Substring(0, name.Length - 4);
					var procs = Process.GetProcessesByName(name);
					if (procs.Length > 1)
					{
						Console.WriteLine(
							"Multiple processes with the name '{0}'. Specify the process by pid", name);
						return;
					}
					process = Process.GetProcessesByName(name).First();
				}
			}
			catch
			{
				Console.Error.WriteLine("Could not open process");
				Environment.Exit(-1);
			}

			Console.WriteLine("Process opened!");
			var modules = process.Modules;
			foreach (ProcessModule pmod in modules)
			{
				Console.WriteLine(pmod.FileName);
			}
		}
	}
}
