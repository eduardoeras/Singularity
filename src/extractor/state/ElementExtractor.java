package extractor.state;

import global.structure.Line;
import global.structure.Element;
import org.antlr.v4.runtime.tree.ParseTree;

public class ElementExtractor {
    public Element getLevelElement(Line line) {
        if (line.getContent().isEmpty()) {
            return Element.NONE;
        }
        for (ParseTree keyword : line.getContent()) {
            switch (keyword.getText()) {
                case "class" :
                    return Element.CLASS;
                case "struct" :
                    return Element.STRUCT;
                case "namespace" :
                    return Element.NAMESPACE;
                case "for" :
                case "do" :
                case "while" :
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
                case "catch" :
                    return Element.EXCEPTION;
                case "~":
                    return Element.DESTRUCTOR;
            }
        }
        return Element.FUNCTION;
    }

    public Element getStateElement(Line line) {
        for (ParseTree keyword : line.getContent()) {
            switch (keyword.getText()) {
                case "return" :
                case "break" :
                case "continue" :
                case "default" :
                    return Element.JUMP;
                case "=" :
                    return Element.ATTRIBUTION;
                case "while" :
                    return Element.LOOP;
                case "case" :
                    return Element.DECISION;
                case "throw" :
                    return Element.EXCEPTION;
            }
        }
        return Element.STATEMENT;
    }
}