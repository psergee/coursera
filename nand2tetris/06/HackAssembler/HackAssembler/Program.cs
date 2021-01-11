using System;
using System.IO;

namespace HackAssembler
{
    class Program
    {
        static void Main(string[] args)
        {
            if (args.Length == 0)
            {
                Console.WriteLine("Usage: HackAssembler.exe file.asm");
                return;
            }

            Assembler asmCodeProcessor = new Assembler(args[0]);
            asmCodeProcessor.ResolveSymbols();

            using (StreamWriter writer = new StreamWriter(Path.ChangeExtension(args[0], "hack")))
            {
                foreach (var i in asmCodeProcessor.Instructions())
                {
                    writer.WriteLine(i);
                }
            }
        }
    }
}
