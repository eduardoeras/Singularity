package extractor.state;

import global.structure.Line;

import org.antlr.v4.runtime.tree.ParseTree;

import java.util.List;

public class Matrix {
    //Attributes
    private IfRecover ifRecover;
    private boolean ifStatement;
    private boolean forLoop;
    private boolean caseCondition;
    private int open;
    private int close;

    //Constructor
    public Matrix () {
        ifRecover = new IfRecover();
        ifStatement = false;
        forLoop = false;
        caseCondition = false;
        open = 0;
        close = 0;
    }

    //Public Methods
    public void matrixGenerator (List<Line> cppMatrix, ParseTree parseTree) {
        if (parseTree instanceof org.antlr.v4.runtime.tree.TerminalNodeImpl) {
            switch (parseTree.getText()) {
                case "public":
                case "private":
                    addParseTree(cppMatrix, parseTree);
                    setEndCharacterAndAddNewLine(cppMatrix, ":");
                    break;
                case "case" :
                case "default" :
                    caseCondition = true;
                    addParseTree(cppMatrix, parseTree);
                    break;
                case "if" :
                    ifStatement = true;
                    addParseTree(cppMatrix, parseTree);
                    break;
                case "for" :
                    forLoop = true;
                    addParseTree(cppMatrix, parseTree);
                    break;
                case "(" :
                    if (forLoop) {
                        open++;
                    }
                    addParseTree(cppMatrix, parseTree);
                    break;
                case ")" :
                    if (forLoop) {
                        close++;
                    }
                    addParseTree(cppMatrix, parseTree);
                    break;
                case "{" :
                    if (open == close) {
                        open = 0;
                        close = 0;
                        forLoop = false;
                        ifStatement = false;
                        setEndCharacterAndAddNewLine(cppMatrix, "{");
                    } else {
                        addParseTree(cppMatrix, parseTree);
                    }
                    break;
                case ":" :
                    if (caseCondition) {
                        setEndCharacterAndAddNewLine(cppMatrix, ":");
                        caseCondition = false;
                    }
                    break;
                case ";" :
                    if (ifStatement) {
                        ifRecover.recover(cppMatrix, cppMatrix.get(cppMatrix.size() - 1));
                        setEndCharacterAndAddNewLine(cppMatrix, ";");
                        setEndCharacterAndAddNewLine(cppMatrix, "}");
                        ifStatement = false;
                    } else if (!forLoop) {
                        setEndCharacterAndAddNewLine(cppMatrix, ";");
                    }
                    break;
                case "}" :
                    if (!forLoop) {
                        setEndCharacterAndAddNewLine(cppMatrix, "}");
                    } else {
                        addParseTree(cppMatrix, parseTree);
                    }
                    break;
                default:
                    addParseTree(cppMatrix, parseTree);
            }
        }
        for (int i = 0; i < parseTree.getChildCount(); i++) {
            matrixGenerator(cppMatrix, parseTree.getChild(i));
        }
    }

    //Private Methods
    private void setEndCharacterAndAddNewLine (List<Line> cppMatrix, String endCharacter) {
        cppMatrix.get(cppMatrix.size() - 1).setEndLineCharacter(endCharacter);
        cppMatrix.add(new Line());
    }

    private void addParseTree (List<Line> cppMatrix, ParseTree parseTree) {
        cppMatrix.get(cppMatrix.size() - 1).getContent().add(parseTree);
    }

}
