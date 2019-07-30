package extractor.tools;

import global.structure.Line;

import org.antlr.v4.runtime.tree.ParseTree;

import java.util.List;

public class Matrix {
    //Attributes

    //Constructor

    //Methods
    public void matrixGenerator (List<Line> cppMatrix, ParseTree parseTree) {
        if (parseTree instanceof org.antlr.v4.runtime.tree.TerminalNodeImpl) {
            switch (parseTree.getText()) {
                case "{" :
                    cppMatrix.get(cppMatrix.size() - 1).setEndLineCharacter("{");
                    cppMatrix.add(new Line());
                    break;
                case ";" :
                    cppMatrix.get(cppMatrix.size() - 1).setEndLineCharacter(";");
                    cppMatrix.add(new Line());
                    break;
                case ":" :
                    cppMatrix.get(cppMatrix.size() - 1).setEndLineCharacter(":");
                    cppMatrix.add(new Line());
                    break;
                case "}" :
                    cppMatrix.get(cppMatrix.size() - 1).setEndLineCharacter("}");
                    cppMatrix.add(new Line());
                    break;
                default:
                    cppMatrix.get(cppMatrix.size() - 1).getContent().add(parseTree);
            }
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            matrixGenerator(cppMatrix, parseTree.getChild(i));
        }
    }

}
