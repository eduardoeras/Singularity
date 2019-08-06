package extractor.state;

import global.structure.Line;
import global.structure.Element;
import org.antlr.v4.runtime.tree.ParseTree;

public class elementExtractor {
    public Element getLevelElement(Line line) {
        if (line.getContent().isEmpty()) {
            return Element.NONE;
        }
        for (ParseTree keyword : line.getContent()) {
            switch (keyword.getText()) {
                case "class" :
                    return Element.CLASS;
                case "for" :
                    return Element.LOOP;
                case "do" :
                    return Element.LOOP;
                case "while" :
                    return Element.LOOP;
                case "switch" :
                    return Element.LOOP;
                case "if" :
                    return Element.DECISION;
                case "else" :
                    if (line.getContent().size() == 1) {
                        return Element.JUMP;
                    }
                    break;
                case "try" :
                    return Element.EXCEPTION;
                case "catch" :
                    return Element.EXCEPTION;
            }
        }
        return Element.FUNCTION;
    }

    public Element getStateElement(Line line) {
        for (ParseTree keyword : line.getContent()) {
            switch (keyword.getText()) {
                case "return" :
                    return Element.JUMP;
                case "break" :
                    return Element.JUMP;
                case "continue" :
                    return Element.JUMP;
                case "=" :
                    return Element.ATTRIBUTION;
            }
        }
        return Element.STATEMENT;
    }
}