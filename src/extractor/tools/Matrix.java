package extractor.tools;

import org.antlr.v4.runtime.tree.ParseTree;

import java.util.ArrayList;
import java.util.List;

public class Matrix {
    //Attributes

    //Constructor

    //Methods
    public void matrixGenerator (List<List<ParseTree>> cppMatrix, ParseTree parseTree) {
        if (parseTree instanceof org.antlr.v4.runtime.tree.TerminalNodeImpl) {
            if (parseTree.getText().equals("{") || parseTree.getText().equals("}")) {
                if (!cppMatrix.get(cppMatrix.size() - 1).isEmpty()) {
                    newLine(cppMatrix);
                }
                cppMatrix.get(cppMatrix.size() - 1).add(parseTree);
                newLine(cppMatrix);
            }else if (!parseTree.getText().equals(";") && !parseTree.getText().equals(":")) {
                cppMatrix.get(cppMatrix.size() - 1).add(parseTree);
            } else {
                newLine(cppMatrix);
            }
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            matrixGenerator(cppMatrix, parseTree.getChild(i));
        }
    }

    //Private Methods
    private void newLine (List<List<ParseTree>> cppMatrix) {
        List<ParseTree> line = new ArrayList<>();
        cppMatrix.add(line);
    }

}
