package extractor.state;

import global.structure.*;
import global.tools.IdGenerator;
import global.tools.StringTools;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.ArrayList;
import java.util.List;

public class StateWalker {
    //Attributes
    private LabelExtractor labelExtractor;
    private ElementExtractor elementExtractor;
    private int scopeLevel;
    private Visibility visibility;
    private String className;
    private StringTools stringTools;
    private List<String> namespaces;

    //Constructor
    public StateWalker () {
        labelExtractor = new LabelExtractor();
        elementExtractor = new ElementExtractor();
        scopeLevel = 0;
        visibility = Visibility.NONE;
        className = "";
        stringTools = new StringTools();
        namespaces = new ArrayList<>();
    }

    //Public Methods
    public void walk (List<Line> cppMatrix, List<State> states) {
        for (Line line : cppMatrix) {
            classify(line, states);
        }
        reset();
    }

    //Private Methods
    private void classify (Line line, List<State> states) {
        if (scopeLevel == 0) {
            visibility = Visibility.NONE;
        }
        colectNamespace(line);
        switch (line.getEndLineCharacter()) {
            case "{" :
                State newLevel = new State();
                newLevel.setType(Type.LEVEL);
                newLevel.setLine(line);
                newLevel.setLabel(labelExtractor.getLevelLabel(line));
                if (newLevel.getLabel().equals("if")) {
                    elseIfRecover(newLevel);
                }
                newLevel.setElement(elementExtractor.getLevelElement(line));
                newLevel.setScopeLevel(scopeLevel);
                if (newLevel.getElement() == Element.STRUCT) {
                    visibility = Visibility.PUBLIC;
                }
                if (newLevel.getElement() == Element.CLASS || newLevel.getElement() == Element.STRUCT) {
                    className = newLevel.getLabel();
                }
                if (newLevel.getElement() == Element.FUNCTION) {
                    if (newLevel.getLabel().equals(className) || namespaces.contains(newLevel.getLabel())) {
                        newLevel.setElement(Element.CONSTRUCTOR);
                        newLevel.setVisibility(Visibility.NONE);
                    } else {
                        newLevel.setVisibility(visibility);
                        if (stringTools.allOperators().contains(newLevel.getLabel())) {
                            newLevel.setElement(Element.OPERATOR);
                        }
                    }
                } else {
                    newLevel.setVisibility(Visibility.NONE);
                }
                if (newLevel.getElement() == Element.DECISION || newLevel.getElement() == Element.LOOP) {
                    if (!newLevel.getLabel().equals("do") && !newLevel.getLabel().equals("switch") && !newLevel.getLabel().equals("else")) {
                        newLevel.setId(IdGenerator.getIntegerId());
                    }
                }
                states.add(newLevel);
                scopeLevel++;
                break;
            case "}" :
                scopeLevel--;
                State endLevel = new State();
                endLevel.setType(Type.LEVEL);
                endLevel.setLabel("}");
                endLevel.setElement(Element.NONE);
                endLevel.setLine(line);
                endLevel.setScopeLevel(scopeLevel);
                endLevel.setVisibility(Visibility.NONE);
                states.add(endLevel);
                break;
            case ";" :
                if (line.getContent().isEmpty()) {
                    break;
                }
                State statement = new State();
                statement.setType(Type.STATE);
                statement.setLabel(labelExtractor.getStateLabel(line));
                statement.setElement(elementExtractor.getStateElement(line));
                statement.setVisibility(Visibility.NONE);
                statement.setLine(line);
                if (statement.getElement() == Element.JUMP) {
                    if (firstWordInLine(statement.getLine(), "return")) {
                        statement.setId(IdGenerator.getIntegerId());
                    }
                } else {
                    statement.setId(IdGenerator.getIntegerId());
                }
                statement.setScopeLevel(scopeLevel);
                states.add(statement);
                break;
            case ":" :
                if (firstWordInLine(line, "case") || firstWordInLine(line, "default")) {
                    State caseLevel = new State();
                    caseLevel.setType(Type.STATE);
                    caseLevel.setLabel("case_" + labelExtractor.getLevelLabel(line));
                    caseLevel.setElement(elementExtractor.getStateElement(line));
                    caseLevel.setLine(line);
                    caseLevel.setScopeLevel(scopeLevel);
                    caseLevel.setVisibility(Visibility.NONE);
                    if (caseLevel.getElement() != Element.JUMP) {
                        caseLevel.setId(IdGenerator.getIntegerId());
                    }
                    states.add(caseLevel);
                    break;
                }
                if (firstWordInLine(line, "public")) {
                    visibility = Visibility.PUBLIC;
                    break;
                }
                if (firstWordInLine(line, "private")) {
                    visibility = Visibility.PRIVATE;
                    break;
                }
                break;
            default:
                //Do nothing
        }
    }

    private void colectNamespace(Line line) {
        String last = "";
        for (ParseTree element : line.getContent()) {
            if (element.getText().equals("::")) {
                if (!namespaces.contains(last) && !stringTools.allOperators().contains(last)) {
                    namespaces.add(last);
                }
            } else {
                last = element.getText();
            }
        }
    }

    private void elseIfRecover(State state) {
        if (firstWordInLine(state.getLine(), "else")) {
            state.setLabel("else_if");
        }
    }

    private boolean firstWordInLine (Line line, String word) {
        return line.getContent().get(0).getText().equals(word);
    }

    private void reset () {
        scopeLevel = 0;
        visibility = Visibility.NONE;
        className = "";
        namespaces.clear();
    }
}
