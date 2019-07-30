package extractor.state;

import global.structure.Line;
import global.structure.Instruction;
import global.tools.StringTools;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.List;

public class StateWalker {
    //Attributes
    private int scopeLevel;
    private StringTools stringTools;

    //Constructor
    public StateWalker () {
        scopeLevel = 0;
        stringTools = new StringTools();
    }

    //Public Methods
    public void walk (List<Line> cppMatrix, List<Instruction> states) {
        for (Line line : cppMatrix) {
            classify(line, states);
        }
    }

    //Private Methods
    private void classify (Line line, List<Instruction> states) {
        switch (line.getEndLineCharacter()) {
            case "{" :
                printLineContents(line);
                System.out.println();
                indent();
                System.out.print("{\n");
                scopeLevel++;
                break;
            case "}" :
                scopeLevel--;
                indent();
                System.out.print("}\n");
                break;
            case ";" :
                printLineContents(line);
                System.out.print(";\n");
                break;
            case ":" :
                printLineContents(line);
                System.out.print(":\n");
                break;
            default:
                System.out.println("No supposed to be here");
        }
    }

    private void printLineContents (Line line) {
        indent();
        for (ParseTree keyword : line.getContent()) {
            System.out.print(keyword.getText() + " ");
        }
    }

    private void indent () {
        for (int i = 0; i < scopeLevel; i++) {
            System.out.print("  ");
        }
    }

}
