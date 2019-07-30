package global.structure;

import org.antlr.v4.runtime.tree.ParseTree;

import java.util.ArrayList;
import java.util.List;

public class Line {
    //Attributes
    private List<ParseTree> content;
    private String endLineCharacter;

    //Constructor
    public Line () {
        content = new ArrayList<>();
        endLineCharacter = "";
    }

    //Methods
    public List<ParseTree> getContent () {
        return content;
    }

    public void setEndLineCharacter (String endLineCharacter) {
        this.endLineCharacter = endLineCharacter;
    }

    public String getEndLineCharacter () {
        return endLineCharacter;
    }

}
