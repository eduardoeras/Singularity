package extractor.state;

import global.structure.Line;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.List;

public class IfRecover {
    //Attributes

    //Constructor

    //Public Methods
    public void recover(List<Line> cppMatrix, Line line) {
        Line newLine = new Line();
        for (int i = findSplitPoint(line); i < line.getContent().size(); i++) {
            newLine.getContent().add(line.getContent().get(i));
            line.getContent().remove(i);
        }
        line.setEndLineCharacter("{");
        cppMatrix.add(newLine);
    }

    //Private
    private int findSplitPoint (Line line) {
        int open = 0;
        int close = 0;
        int splitPoint = 0;
        for (ParseTree parseTree: line.getContent()) {
            splitPoint ++;
            if (parseTree.getText().equals("(")) {
                open ++;
            }
            if (parseTree.getText().equals(")")) {
                close ++;
                if (open == close) {
                    return splitPoint;
                }
            }
        }
        return 0;
    }
}
