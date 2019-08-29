package extractor.state;

import global.structure.Line;
import global.tools.StringTools;
import org.antlr.v4.runtime.tree.ParseTree;

public class LabelExtractor {
    //Attributes
    private StringTools stringTools;

    //Constructor
    public LabelExtractor () {
        stringTools = new StringTools();
    }

    //Methods
    public String getLevelLabel(Line line) {
        String last = "";
        for (ParseTree parseTree : line.getContent()) {
            if (parseTree.getText().equals("(")) {
                return last;
            }
            last = parseTree.getText();
        }
        return last;
    }

    public String getStateLabel(Line line) {
        String label = "";
        for (ParseTree keyword : line.getContent()) {
            label = label.concat(keyword.getText() + " ");
        }
        return stringTools.noSpecialCharacters(label);
    }

}
