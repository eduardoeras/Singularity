package extractor.transition;

import global.structure.Transition;

import java.util.ArrayList;
import java.util.List;

public class BreakControl {
    //Attributes
    private static BreakControl uniqueInstance;
    private boolean breakFlag;
    private List<Transition> response;

    //Constructor
    private BreakControl () {
        breakFlag = false;
        response = new ArrayList<>();
    }

    //Methods
    public static BreakControl getInstance () {
        if (uniqueInstance == null) {
            uniqueInstance = new BreakControl();
        }
        return uniqueInstance;
    }

    public void addResponse (List<Transition> response) {
        this.response.addAll(response);
    }

    public List<Transition> getResponse () {
        return response;
    }

    public void setBreak () {
        breakFlag = true;
    }

    public boolean getBreak () {
        return breakFlag;
    }

    public void reset () {
        breakFlag = false;
        response.clear();
    }

}
