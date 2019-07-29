package extractor.state;

import global.structure.Instruction;
import global.tools.StringTools;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.List;

public class StateWalker {
    //Attributes
    private StringTools stringTools;

    //Constructor
    public StateWalker () {
        stringTools = new StringTools();
    }

    //Public Methods
    public void walk (List<List<ParseTree>> cppMatrix, List<Instruction> states) {
        for (List<ParseTree> line : cppMatrix) {
            classify(line, states);
        }
    }

    //Private Methods
    private void classify (List<ParseTree> line, List<Instruction> states) {
        for (ParseTree keyword : line) {
            switch (keyword.getText()) {
                case "class" :
                    System.out.println("CLASS declaration");
                    break;
                case "if" :
                    System.out.println("IF statement");
                    break;
                case "for" :
                    System.out.println("FOR statement");
                    break;
                case "while" :
                    System.out.println("while statement");
                    break;
                case "do" :
                    System.out.println("DO statement");
                    break;
            }
        }
    }

}
