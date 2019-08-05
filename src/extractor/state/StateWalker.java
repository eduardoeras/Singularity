package extractor.state;

import global.structure.*;
import global.tools.IdGenerator;
import global.tools.StringTools;

import java.util.List;

public class StateWalker {
    //Attributes
    private LabelExtractor labelExtractor;
    private TypeExtractor typeExtractor;
    private int scopeLevel;
    private StringTools stringTools;

    //Constructor
    public StateWalker () {
        labelExtractor = new LabelExtractor();
        typeExtractor = new TypeExtractor();
        scopeLevel = 0;
        stringTools = new StringTools();
    }

    //Public Methods
    public void walk (List<Line> cppMatrix, List<Instruction> states) {
        for (Line line : cppMatrix) {
            classify(line, states);
        }
    }

    //Private Methods
    private void classify (Line line, List<Instruction> states) {
        switch (line.getEndLineCharacter()) {
            case "{" :
                Level newLevel = new Level();
                newLevel.setLabel(labelExtractor.getLevelLabel(line));
                newLevel.setType(typeExtractor.getLevelType(line));
                newLevel.setLine(line);
                newLevel.setScopeLevel(scopeLevel);
                states.add(newLevel);
                scopeLevel++;
                break;
            case "}" :
                scopeLevel--;
                Level endLevel = new Level();
                endLevel.setLabel(labelExtractor.getLevelLabel(line));
                endLevel.setType(typeExtractor.getLevelType(line));
                endLevel.setLine(line);
                endLevel.setScopeLevel(scopeLevel);
                states.add(endLevel);
                break;
            case ";" :
                State statement = new State();
                statement.setLabel(labelExtractor.getStateLabel(line));
                statement.setType(typeExtractor.getStateType(line));
                statement.setLine(line);
                statement.setId(IdGenerator.getIntegerId());
                statement.setScopeLevel(scopeLevel);
                states.add(statement);
                break;
            case ":" :
                State member = new State();
                member.setLabel(labelExtractor.getStateLabel(line));
                member.setType(typeExtractor.getStateType(line));
                member.setLine(line);
                member.setScopeLevel(scopeLevel);
                states.add(member);
                break;
            default:
                //Do nothing
        }
    }

}
