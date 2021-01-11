using System;
using System.Collections.Generic;

namespace HackAssembler
{
    class Operand
    {
        public static explicit operator short(Operand reg) => 0b110000000000;
        public static short operator !(Operand reg) => 0b110001000000;
        public static short operator -(Operand reg) => 0b110011000000;
        public static short operator +(Operand reg, Constant i) => 0b110111000000;
        public static short operator -(Operand reg, Constant i) => 0b110010000000;
        public static short operator -(Operand reg, DRegister dr) => 0b000111000000;
        public static short operator -(Operand reg, Operand dr) => 0b0;
        public static short operator +(Operand reg, Operand dr) => 0b0;
        public static short operator &(Operand reg, Operand dr) => 0b0;
        public static short operator |(Operand reg, Operand dr) => 0b0;
    }

    class DRegister: Operand
    {
        public static explicit operator short(DRegister reg) => 0b001100000000;
        public static short operator !(DRegister reg) => 0b001101000000;
        public static short operator -(DRegister reg) => 0b001111000000;
        public static short operator +(DRegister reg, Constant i) => 0b011111000000;
        public static short operator -(DRegister reg, Constant i) => 0b001110000000;
        public static short operator +(DRegister reg, Operand r) => 0b000010000000;
        public static short operator -(DRegister reg, Operand r) => 0b010011000000;
        public static short operator &(DRegister reg, Operand r) => 0b0;
        public static short operator |(DRegister reg, Operand r) => 0b010101000000;
    }

    class Constant: Operand
    {
        public short Value { get; set; }

        public static explicit operator short(Constant val)
        {
            switch(val.Value)
            {
                case 0:
                    return 0b101010000000;
                case 1:
                    return 0b111111000000;
                default:
                    return val.Value;
            }
        }

        public static short operator -(Constant val) => 0b111010000000;
    }

    class Instruction
    {
        public enum InstructionType { At, Compute };

        public short Code { get; protected set; }
        public readonly InstructionType Type;

        public Instruction(InstructionType type)
        {
            Type = type;
            Code |= 0;
        }

        public override string ToString() => Convert.ToString(Code, 2).PadLeft(16, '0');

        public virtual void Resolve(Resolver resolver) { }
    }

    class AtInstruction: Instruction
    {
        private string symbol = null;

        public AtInstruction(string assemblyLine): base(InstructionType.At)
        {
            string valueToLoad = assemblyLine.Substring(1);
            short address;

            if(short.TryParse(valueToLoad, out address))
                Code |= address;
            else
                symbol = valueToLoad;
        }

        public override void Resolve(Resolver resolver)
        {
            if(symbol != null)
                Code |= resolver.GetSymbolValue(symbol);
        }
    }

    class ComputeInstruction : Instruction
    {
        private enum Jump : short { nil, JGT, JEQ, JGE, JLT, JNE, JLE, JMP };
        private enum Destination : short { nil, M, D, MD, A, AM, AD, AMD };

        static Operand A = new Operand(), M = new Operand();
        static DRegister D =  new DRegister();
        static Constant val =  new Constant();
        static List<dynamic> operands = new List<dynamic>(2);

        public ComputeInstruction(string assemblyLine) : base(InstructionType.Compute)
        {
            Code = -8192;

            string[] commands = assemblyLine.Split(';');
            ProcessJump(commands.Length > 1 ? commands[1]:null);

            string[] computeOperands = commands[0].Split('=');
            if (computeOperands.Length > 1)
            {
                ProcessDestination(computeOperands[0]);
                ProcessCompute(computeOperands[1]);
            }
            else
                ProcessCompute(computeOperands[0]);
        }

        private void ProcessJump(string jmpInstruction)
        {
            if (jmpInstruction == null)
                return;

            Code |= (short)Enum.Parse(typeof(Jump), jmpInstruction);
        }

        private void ProcessDestination(string dest)
        {
            if (dest == null)
                return;

            Code |= (short)((short)Enum.Parse(typeof(Destination), dest) << 3);
        }

        private void ProcessCompute(string compute)
        {
            char? currentOp = null;

            if (compute.Contains("M"))
                Code |= 1 << 12;

            foreach (char c in compute)
            {
                if (char.IsDigit(c))
                {
                    val.Value = short.Parse(c.ToString());
                    operands.Add(val);
                    continue;
                }
                if("!-+&|".IndexOf(c) != -1)
                {
                    currentOp = c;
                    continue;
                }
                switch(c)
                {
                    case 'A':
                        operands.Add(A);
                        break;
                    case 'M':
                        operands.Add(M);
                        break;
                    case 'D':
                        operands.Add(D);
                        break;
                    default:
                        break;
                }
            }

            switch(currentOp)
            {
                case '!':
                    Code |= !operands[0];
                    break;
                case '-':
                    if (operands.Count == 1)
                        Code |= -operands[0];
                    else
                        Code |= (operands[0] - operands[1]);
                    break;
                case '+':
                    Code |= (operands[0] + operands[1]);
                    break;
                case '&':
                    Code |= (operands[0] & operands[1]);
                    break;
                case '|':
                    Code |= (operands[0] | operands[1]);
                    break;
                default:
                    Code |= (short)operands[0];
                    break;
            }
            operands.Clear();
        }
    }
}
