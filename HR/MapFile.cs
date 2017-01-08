﻿using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace HR
{
	public class MapFileSymbol
	{
		public string Name { get; set; }
		public string ObjectFileName { get; set; }
		public ulong RVA { get; set; }
		public bool IsFunction { get; set; }

		public override string ToString()
		{
			return String.Format("{0}: '{1}' at {2:X16} in '{3}'",
				IsFunction ? "Function" : "Symbol", Name, RVA, ObjectFileName);
		}
	}

	public class MapFile
	{
		public ulong BaseAddress { get; set; }
		public List<MapFileSymbol> Symbols { get; } = new List<MapFileSymbol>();

		/// <summary>
		/// A very simple map file parser that strictly relies on the layout 
		/// generated by the MSVC 2017 linker. Returns null if parsing failed.
		/// </summary>
		public static MapFile ParseFromFile(string path, bool filter = true)
		{
			using (var sr = new StreamReader(File.OpenRead(path)))
			{
				bool found = false;
				string line = "";

				while (!sr.EndOfStream)
				{
					line = sr.ReadLine();
					if (line.Contains("Preferred load address is"))
					{
						found = true;
						break;
					}
				}

				if (!found)
					return null;

				string[] parts = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
				string baseAddrStr = parts[4];
				ulong baseAddr = UInt64.Parse(baseAddrStr, System.Globalization.NumberStyles.HexNumber);
				
				while (!sr.EndOfStream)
				{
					line = sr.ReadLine();
					if (line.Contains("Publics by Value"))
					{
						found = true;
						break;
					}
				}

				if (!found)
					return null;

				var mf = new MapFile();
				mf.BaseAddress = baseAddr;
				sr.ReadLine();

				while (!sr.EndOfStream)
				{
					line = sr.ReadLine();

					if (line.Length == 0)
						continue;

					if (line.Contains("entry point at") ||
						line.Contains("Static symbols"))
					{
						// Don't parse static symbols yet if filter is true.
						if (filter)
							break;

						continue;
					}

					parts = line.Split(new char[] { ' ' }, StringSplitOptions.RemoveEmptyEntries);
					string module = parts[parts.Length - 1];
					string symbol = parts[1];
					string addrStr = parts[2];

					// RVA is addr - baseAddr
					ulong rva = UInt64.Parse(addrStr, System.Globalization.NumberStyles.HexNumber);
					if (rva > baseAddr)
						rva = rva - baseAddr;
					
					bool isFunction = false;
					if (parts.Length > 3 && parts[3] == "f")
						isFunction = true;

					if (filter)
					{
						// Ignore symbols contained in a static linked library
						// like "MSVCRTD:ucrt_stubs.obj" for now. Detect this by looking for a colon.
						if (module.Contains(":") || !module.Contains(".obj"))
							continue;
					}

					var mfe = new MapFileSymbol();
					mfe.RVA = rva;
					mfe.Name = symbol;
					mfe.ObjectFileName = module;
					mfe.IsFunction = isFunction;
					mf.Symbols.Add(mfe);
				}
				return mf;
			}
		}
	}

}