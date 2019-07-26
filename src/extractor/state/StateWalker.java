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
            classify(cppMatrix, states);
        }
    }

    //Private Methods
    private void classify (List<List<ParseTree>> cppMatrix, List<Instruction> states) {
        //CONTINUE FROM HERE
    }

}
