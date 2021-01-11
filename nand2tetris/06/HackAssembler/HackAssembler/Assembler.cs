using System.Collections.Generic;
using System.IO;

namespace HackAssembler
{
    class Assembler
    {
        private List<Instruction> instructions;
        private Resolver symbolResolver;

        public Assembler(string fileName)
        {
            instructions = new List<Instruction>();
            symbolResolver = new Resolver();

            using (StreamReader asmReader = File.OpenText(fileName))
            {
                while(!asmReader.EndOfStream)
                {
                    string asmLine  = asmReader.ReadLine();
                    RemoveComments(ref asmLine);

                    if (asmLine.Length == 0)
                        continue;

                    if (asmLine.StartsWith("("))
                        symbolResolver.AddSymbol(asmLine.Trim('(', ')'), (short)instructions.Count);
                    else if (asmLine.StartsWith("@"))
                        instructions.Add(new AtInstruction(asmLine));
                    else
                        instructions.Add(new ComputeInstruction(asmLine));
                }
            }
        }

        void RemoveComments(ref string asmCode)
        {
            int commentStart = asmCode.IndexOf('/');
            if (commentStart != -1)
                asmCode = asmCode.Remove(commentStart);

            asmCode = asmCode.Trim();
        }

        public void ResolveSymbols()
        {
            foreach (Instruction inst in instructions)
                inst.Resolve(symbolResolver);
        }

        public IEnumerable<Instruction> Instructions()
        {
            foreach (Instruction i in instructions)
                yield return i;
        }
    }
}
