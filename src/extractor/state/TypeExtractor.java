package extractor.state;

import global.structure.Line;
import global.structure.Type;
import org.antlr.v4.runtime.tree.ParseTree;

public class TypeExtractor {
    public Type getLevelType(Line line) {
        for (ParseTree keyword : line.getContent()) {
            switch (keyword.getText()) {
                case "class" :
                    return Type.CLASS;
                case "for" :
                    return Type.LOOP;
                case "do" :
                    return Type.LOOP;
                case "while" :
                    return Type.LOOP;
                case "switch" :
                    return Type.LOOP;
                case "if" :
                    return Type.DECISION;
                case "try" :
                    return Type.EXCEPTION;
                case "catch" :
                    return Type.EXCEPTION;
                default :
                    return Type.FUNCTION;
            }
        }
        return Type.NONE;
    }

    public Type getStateType(Line line) {
        for (ParseTree keyword : line.getContent()) {
            switch (keyword.getText()) {
                case "return" :
                    return Type.JUMP;
                case "break" :
                    return Type.JUMP;
                case "CONTINUE" :
                    return Type.JUMP;
                default :
                    return Type.STATEMENT;
            }
        }
        return Type.NONE;
    }
}
/*
        attribution
        declaration
        constructor
        self
        internal
        external
        jump
        exception
        IO
 */