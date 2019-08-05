package extractor.tools;

import global.structure.Instruction;
import global.structure.State;
import global.structure.Type;

import java.util.List;

public class InstructionPrinter {
    //Attrbutes

    //Constructor

    //Methods
    public void print (List<Instruction> instructions) {
        for (Instruction instruction : instructions) {
            for (int i = 0; i < instruction.getScopeLevel(); i++) {
                System.out.print("     ");
            }
            if (instruction instanceof State) {
                System.out.println("<state label = '" + instruction.getLabel() + "' type = '" + instruction.getType() + "'>");
            } else {
                if (instruction.getType() == Type.NONE) {
                    System.out.println("</level>");
                } else {
                    System.out.println("<level label = '" + instruction.getLabel() + "' type = '" + instruction.getType() + "'>");
                }
            }
        }
    }
}
