package extractor.state;

import global.structure.Instruction;
import global.tools.StringTools;
import org.antlr.v4.runtime.tree.ParseTree;
import reader.CPP14Parser;

import java.util.List;

public class StateWalker {
    //Attributes
    private StringTools stringTools;

    //Constructor
    public StateWalker () {
        stringTools = new StringTools();
    }

    //Public Methods
    public void walk (ParseTree parseTree, List<Instruction> states) {
        classify(parseTree, states);
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            walk(parseTree.getChild(i), states);
        }
    }

    //Private Methods
    private void classify (ParseTree parseTree, List<Instruction> states) {
        /*if (parseTree instanceof CPP14Parser.ClassheadnameContext) {
            System.out.println("CLASS " + parseTree.getText());
        }
        if (parseTree instanceof CPP14Parser.FunctiondefinitionContext) {
            System.out.println("FUNCTION " + stringTools.noSpecialCharacters(stringTools.beforeParenthesis(parseTree.getText())));
        }
        if (parseTree instanceof CPP14Parser.ConditionContext) {
            System.out.println("CONDITION" + parseTree.getText());
        }*/
        if (parseTree instanceof org.antlr.v4.runtime.tree.TerminalNodeImpl) {
            //if (parseTree.getText().equals(";")) {
                System.out.println("STATEMENT -> " + parseTree.getText());
            //}
        }
    }

}
