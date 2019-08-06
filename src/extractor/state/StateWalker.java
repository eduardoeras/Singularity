package extractor.state;

import global.structure.*;
import global.tools.IdGenerator;
import org.antlr.v4.runtime.tree.ParseTree;

import java.util.List;

public class StateWalker {
    //Attributes
    private LabelExtractor labelExtractor;
    private elementExtractor elementExtractor;
    private int scopeLevel;
    private Visibility visibility;

    //Constructor
    public StateWalker () {
        labelExtractor = new LabelExtractor();
        elementExtractor = new elementExtractor();
        scopeLevel = 0;
        visibility = Visibility.NONE;
    }

    //Public Methods
    public void walk (List<Line> cppMatrix, List<State> states) {
        for (Line line : cppMatrix) {
            classify(line, states);
        }
    }

    //Private Methods
    private void classify (Line line, List<State> states) {
        switch (line.getEndLineCharacter()) {
            case "{" :
                State newLevel = new State();
                newLevel.setType(Type.LEVEL);
                newLevel.setLabel(labelExtractor.getLevelLabel(line));
                newLevel.setElement(elementExtractor.getLevelElement(line));
                newLevel.setLine(line);
                newLevel.setScopeLevel(scopeLevel);
                if (newLevel.getElement() == Element.FUNCTION) {
                    newLevel.setVisibility(visibility);
                } else {
                    newLevel.setVisibility(Visibility.NONE);
                }
                if (newLevel.getElement() == Element.DECISION || newLevel.getElement() == Element.LOOP) {
                    newLevel.setId(IdGenerator.getIntegerId());
                }
                states.add(newLevel);
                scopeLevel++;
                break;
            case "}" :
                scopeLevel--;
                State endLevel = new State();
                endLevel.setType(Type.LEVEL);
                endLevel.setLabel("");
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
                statement.setVisibility(Visibility.NONE);
                statement.setLabel(labelExtractor.getStateLabel(line));
                statement.setElement(elementExtractor.getStateElement(line));
                statement.setLine(line);
                statement.setId(IdGenerator.getIntegerId());
                statement.setScopeLevel(scopeLevel);
                states.add(statement);
                break;
            case ":" :
                if (line.getContent().get(0).getText().equals("public")) {
                    visibility = Visibility.PUBLIC;
                    break;
                }
                if (line.getContent().get(0).getText().equals("private")) {
                    visibility = Visibility.PRIVATE;
                    break;
                }
                /*State member = new State();
                member.setLabel(labelExtractor.getStateLabel(line));
                member.setElement(typeExtractor.getStateType(line));
                member.setLine(line);
                member.setScopeLevel(scopeLevel);
                states.add(member);*/
                break;
            default:
                //Do nothing
        }
    }

}
