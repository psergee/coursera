using System.Collections.Generic;

namespace HackAssembler
{
    class Resolver
    {
        private static short AddressLocalVarsStart = 16;

        private Dictionary<string, short> symbols;
        private Dictionary<string, short> variables;

        public Resolver()
        {
            symbols = new Dictionary<string, short>()
            {
                {"R0", 0 },
                {"R1", 1 },
                {"R2", 2 },
                {"R3", 3 },
                {"R4", 4 },
                {"R5", 5 },
                {"R6", 6 },
                {"R7", 7 },
                {"R8", 8 },
                {"R9", 9 },
                {"R10", 10 },
                {"R11", 11 },
                {"R12", 12 },
                {"R13", 13 },
                {"R14", 14 },
                {"R15", 15 },
                {"SCREEN", 16384 },
                {"KBD", 24576 },
                {"SP", 0 },
                {"LCL", 1 },
                {"ARG", 2 },
                {"THIS", 3 },
                {"THAT", 4 }

            };

            variables = new Dictionary<string, short>();
        }

        public void AddSymbol(string name, short address) => symbols.Add(name, address);
        public short AddVariable(string name)
        {
            short address;
            if (!variables.TryGetValue(name, out address))
            {
                address = (short)(AddressLocalVarsStart + variables.Count);
                variables[name] = address;
            }
            return address;
        }

        public short GetSymbolValue(string symbol)
        {
            if (symbol == null)
                return 0;

            short value;
            if (symbols.TryGetValue(symbol, out value))
                return value;

            return AddVariable(symbol);
        }
    }
}
